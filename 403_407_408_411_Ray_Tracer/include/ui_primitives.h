#ifndef UI_PRIMITIVES_H
#define UI_PRIMITIVES_H
#include <float.h>
#include "HandmadeMath.h"
#include "common.h"
struct Ray {
  v3 start;
  v3 direction;
  v3 inv_dir;
  int sign[3];

  inline v3 point_at( float t ) const{
    return start + t*(direction);
  }

  Ray ( v3 origin, v3 dir ):start(origin), direction( dir ) {
    sign[0] = dir.X < 0;
    sign[1] = dir.Y < 0;
    sign[2] = dir.Z < 0;

    inv_dir = v3{ 1/dir.X, 1/dir.Y, 1/dir.Z };
  }
  Ray (){}
};

union AABB{
  enum {
    LOWER = 0,
    UPPER = 1,
  };
  v3 bounds[2];
  struct {
    v3 l,u;
  };

  AABB () {
    bounds[0] = v3{ FLT_MAX, FLT_MAX, FLT_MAX }; 
    bounds[1] = v3{ -FLT_MAX, -FLT_MAX, -FLT_MAX }; 
  }

  AABB (v3 lower, v3 upper){
    bounds[UPPER] = upper;
    bounds[LOWER]=lower;
  }

};

typedef AABB (*GetAABBFunc)( void *obj, const m4 &model );
typedef void (*TranslateFunc)( void *obj, float t, v3 dir, m4 &m );
typedef void (*RotateFunc)( void *obj, float deg, v3 axis, m4 &m );
typedef void (*ScaleFunc)( void *obj, v3 scale, m4 &m );

enum ObjectType {
  OBJECT_SPHERE = 0,
  OBJECT_CUBE_INSTANCE,
  OBJECT_GRID,
  OBJECT_RECT,

  OBJECT_LIGHT_SPHERE,
  OBJECT_LIGHT_CUBE_INSTANCE,
  OBJECT_LIGHT_GRID,
  OBJECT_LIGHT_RECT,
  _OBJECT_MAX
};
struct Object {
  ObjectType type;
  union {
    void *object;
    uint index;
  };
};


struct HitRecord{
  float t;
  v3 p;
  v3 n;
  float u,v;
  
  Object obj;

  void print( ){
    fprintf( stdout, "Hit Record Info\n" );
    fprintf( stdout, "Parameter t = %f\n", t );
    fprintf( stdout, "Point p = " );
    print_v3( p );
    fprintf( stdout, "\nNormal n = " );
    print_v3( n );
    fprintf( stdout, "\nu = %f, v = %f\n",u,v );
  }
};
struct Sphere {
  v3 c;
  float r;
  AABB box;
};

inline Sphere create_sphere( v3 center, float radius ){
  Sphere s;
  s.c = center; 
  s.r = radius;
  v3 rad = { s.r, s.r, s.r };
  s.box = AABB( s.c-rad, s.c+rad ); 
  return s;
}

#if 0
inline AABB sphere_aabb( const Sphere &sph ){
  v3 r = { sph.r, sph.r, sph.r };
  return AABB( sph.c - r, sph.c + r );
}
#endif

inline AABB sphere_aabb( void *s, const m4 &m ){
  Sphere *sph = ( Sphere * )s;
  v3 r = { sph->r, sph->r, sph->r };
  sph->box = AABB( sph->c - r, sph->c + r );
  return sph->box;
}

struct Plane {
  v3 p;
  v3 n;
  AABB box;
  Plane (){ }

  Plane ( v3 point, v3 normal ):p(point), n(normal){}

  Plane ( v3 point, v3 normal, AABB b ):
    p(point), n(normal){ box = b; }

  Plane ( v3 p0, v3 p1, v3 p3, AABB &b ):
    p(p0),
    n( HMM_Cross( p1-p0, p3-p0 ) ){
      box = b;
    }
};

struct AARect{
  enum RectType {
    PLANE_XY = 0,
    PLANE_YZ = 1,
    PLANE_ZX = 2
  };
  RectType type;
  float d;
  v3 n;
  AABB bounds;
  int ndim, d0,d1;
  v3 corner;
  AARect(){}
  AARect ( RectType t, float dist, AABB b,int flip  ):
    type(t),d(dist), bounds(b), d0( t ), d1( (t+1)%3 )
  {
    int sign = ( flip )?-1:1;
    switch ( t ){
      case PLANE_XY:
        n = v3{ 0.0f, 0.0f, sign * 1.0f };
        ndim = 2;
        corner = v3{ b.l[0], b.l[1],d};
        break;
      case PLANE_YZ:
        n = v3{ sign*1.0f,0.0f, 0.0f };
        ndim = 0;
        corner = v3{ d, b.l[1], b.l[2] };
        break;
      case PLANE_ZX:
        n = v3{ 0.0f, sign*1.0f, 0.0f };
        ndim = 1;
        corner = v3{ b.l[0], d, b.l[2] };
        break;
    }
    bounds.l[ ndim ] = d - 0.001f;
    bounds.u[ ndim ] = d + 0.001f;
  }
};

bool AABB_hit( 
    const AABB &box,
    const Ray &ray,
    float t0,
    float t1, 
    HitRecord &record );



bool hit_AARect(
    const AARect &rect,
    const Ray &ray,
    float tmin,
    float tmax,
    HitRecord &record );

AABB AABB_union( AABB box, v3 v );

AABB AABB_union( AABB b1, AABB b2 );

#endif
