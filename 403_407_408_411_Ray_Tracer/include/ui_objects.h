#include <stdio.h>
#include "ui_primitives.h"
#include "common.h"
struct Line {
  v3 start, end,color;
};
Line create_line( const v3 &start, const v3 &end, const v3 &color );
Line create_line_from_ray( const Ray &r, f32 len, const v3 &color );

struct Grid {
  AARect rect;
  Line l1, l2;
  v3 dir1, dir2;  
  f32 w; // width
  f32 unit_u;
  f32 unit_v;
  v3 color;
  int32 nlines;

};


enum CubeState {
  CUBE_ANIMATING,
  CUBE_STATIC
};

struct TranslateAnimation {
  v3 start;
  v3 end;
};

struct RotateAnimation {
  q4 start;
  q4 end;
  float t;
  uint duration;
  uint elapsed;
  float angle;
  m4 rot;
};

struct Cube {

  enum Faces {
    FRONT = 0,
    BACK = 1,
    RIGHT = 2,
    LEFT = 3,
    TOP = 4,
    BOT = 5
  };
  CubeState state;
  TranslateAnimation tanim;
  RotateAnimation ranim;

  AABB bounds;

  m4 base_transform;
  v3 pos;
  f32 length;

  q4 orientation;
  f32 angle;

  union {
    v3 color[6];
  };

};

bool hit_cube(
    const Cube &c,
    const Ray &ray,
    float tmin, float tmax,
    HitRecord &record );

bool hit_sphere(
    const Sphere &s,
    const Ray &ray,
    float tmin, float tmax,
    HitRecord &record );

bool hit_grid(
    const Grid &g,
    const Ray &ray,
    float tmin, float tmax,
    HitRecord &record );

inline void cube_move( void *c, float t, v3 dir, m4 &model ){
  Cube *cube = (Cube *)c;
  cube->pos += t * dir;
  model = HMM_Translate(cube->pos) *
                         HMM_QuaternionToMat4( cube->orientation ) *
                        HMM_Scale( v3{ cube->length,
                            cube->length, cube->length } ); 
}
inline void cube_scale( void *c, v3 dir, m4 &model ){
  Cube *cube = (Cube *)c;
  cube->length = dir.X;
  model = HMM_Translate(cube->pos) *
                         HMM_QuaternionToMat4( cube->orientation ) *
                        HMM_Scale( v3{ cube->length,
                            cube->length, cube->length } ); 
}

inline void cube_rotate( void *c, float deg, v3 axis, m4 &model ){
  q4 quat = HMM_QuaternionFromAxisAngle( axis, HMM_RADIANS(deg) );
  Cube *cube = (Cube *)c;
  cube->orientation = HMM_MultiplyQuaternion(quat, cube->orientation);
  model = HMM_Translate(cube->pos) *
                         HMM_QuaternionToMat4( cube->orientation ) *
                        HMM_Scale( v3{ cube->length,
                            cube->length, cube->length } ); 
}


inline void sphere_move( void *s, float t, v3 dir, m4 &model ){
  Sphere *sphere = ( Sphere *)s;
  sphere->c += t * dir;
  model = HMM_Translate(sphere->c) *
                        HMM_Scale( v3{ sphere->r,
                            sphere->r, sphere->r } );
}

inline void sphere_scale( void *s, v3 dim, m4 &model ){
  Sphere *sphere = ( Sphere *)s;
  sphere->r = dim.X;
  model = HMM_Translate(sphere->c) *
                        HMM_Scale( v3{ sphere->r,
                            sphere->r, sphere->r } );
}
inline void sphere_rotate( void *s, float deg, v3 axis, m4 &model ){
  return;
}

inline void cube_update_matrix( Cube *cube ){
  cube->base_transform = HMM_Translate(cube->pos) *
                         HMM_QuaternionToMat4( cube->orientation ) *
                        HMM_Scale( v3{ cube->length,
                            cube->length, cube->length } ); 
}


AABB cube_get_AABB( void *cube, const m4 &model );

v3 grid_get_corner_point( const Grid &grid, f32 u, f32 v );
Cube create_cube_one_color( float len, v3 pos, v3 color );
Grid create_grid(
    AARect::RectType type,
    const AABB &b,
    f32 d,
    f32 width,
    const v3 &color );


struct Rectangle {
  v3 p0,p1,p2,p3; //four points 
  v3 s1, s2; // two unit vectors
  v3 n; // unit normal vector

  f32 l1,l2; // length along s1 and s2 resp
  q4 orientation;
  AABB box;
};

AABB rectangle_AABB( void *rect, const m4 &m );

inline void rectangle_move( void *rect, float t, v3 dir, m4 &m ){
  Rectangle &r = *(Rectangle *)rect;
  r.p0 += t *dir;
  r.p1 += t *dir;
  r.p2 += t *dir;
  r.p3 += t *dir;
  
  m = HMM_QuaternionToMat4( r.orientation );
}

inline void rectangle_scale( void *c, v3 dir, m4 &model ){
  return;
}

v3 rotate_vector_by_quaternion(const v3& v, const q4& q);

v3 rotate_point_by_quaternion( const v3 &p,const v3 &about, const q4 &q );



inline void rectangle_rotate( void *c, float deg, v3 axis, m4 &model ){
  q4 quat = HMM_QuaternionFromAxisAngle( axis, HMM_RADIANS(deg) );
  Rectangle &r = *(Rectangle *)c;
  v3 rect_mid =  r.p0 + (r.l1/2)*r.s1 + (r.l2/2)*r.s2 ;
  r.p0 = rotate_point_by_quaternion( r.p0, rect_mid, quat );
  r.s1 = rotate_vector_by_quaternion( r.s1, quat );
  r.s2 = rotate_vector_by_quaternion( r.s2, quat );

  r.p1 = r.p0 + r.l1 * r.s1;
  r.p2 = r.p0 + r.l1 * r.s1 + r.l2 * r.s2;
  r.p3 = r.p0 + r.l2 * r.s2;

  r.n = rotate_vector_by_quaternion( r.n, quat );
}

inline void rectangle_scale( void *rect, f32 l1, f32 l2 ){
  Rectangle &r = *(Rectangle *)rect;
  r.l1 = l1;
  r.l2 = l2;
  r.p1 = r.p0 + r.l1 * r.s1;
  r.p2 = r.p0 + r.l1 * r.s1 + r.l2 * r.s2;
  r.p3 = r.p0 + r.l2 * r.s2;
}


Rectangle create_rectangle( v3 p0 );
bool hit_rect(
    Rectangle &r,
    const Ray &ray,
    float tmin,
    float tmax,
    HitRecord &record );
