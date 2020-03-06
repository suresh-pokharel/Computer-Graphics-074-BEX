#define HANDMADE_MATH_IMPLEMENTATION
#include "ui_objects.h"
static v4 DefaultCubeAABBVertex[] = {
  v4{ 0.5f, 0.5f, 0.5f, 1.0f },
  v4{ -0.5f, 0.5f, 0.5f,1.0f },
  v4{ -0.5f, -0.5f, 0.5f,1.0f },
  v4{ 0.5f, -0.5f, 0.5f, 1.0f},
  v4{ 0.5f, 0.5f, -0.5f, 1.0f },
  v4{ -0.5f, 0.5f, -0.5f, 1.0f },
  v4{ -0.5f, -0.5f, -0.5f,1.0f },
  v4{ 0.5f, -0.5f, -0.5f, 1.0f }
};
Line create_line( const v3 &start, const v3 &end, const v3 &color ){
  Line l = { start, end };
  l.color = color;
  return l;
}

Line create_line_from_ray( const Ray &r, f32 len, const v3 &color ){
  Line l;
  l.start = r.start;
  l.end = r.point_at( len );
  l.color= color;
  return l;
}
bool hit_cube(
    const Cube &c,
    const Ray &ray,
    float tmin, float tmax,
    HitRecord &record )
{
  return AABB_hit( c.bounds, ray, tmin, tmax, record );
}

AABB cube_get_AABB( void *c, const m4 &m ){
  Cube *cube = ( Cube * )c;
  v3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
  v3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
  for ( int i = 0; i < 8 ; i++ ){
    v4 vertex = m* DefaultCubeAABBVertex[i];
    min.X = ( min.X < vertex.X )?min.X:vertex.X;
    min.Y = ( min.Y < vertex.Y )?min.Y:vertex.Y;
    min.Z = ( min.Z < vertex.Z )?min.Z:vertex.Z;

    max.X = ( max.X > vertex.X )?max.X:vertex.X;
    max.Y = ( max.Y > vertex.Y )?max.Y:vertex.Y;
    max.Z = ( max.Z > vertex.Z )?max.Z:vertex.Z;
  }


  cube->bounds = AABB( min, max );
  return cube->bounds;
}

Cube create_cube_one_color( float len, v3 pos, v3 color )
{

  Cube cube;
  cube.pos = pos;
  cube.length = len;
  for ( int i = 0; i < 6; i++ ){
    cube.color[i] = color;
  }
  cube.base_transform = HMM_Translate(pos) *
                        HMM_Scale( v3{ len, len, len } ); 
  cube.orientation = HMM_QuaternionFromAxisAngle( v3{0.0f,1.0f,0.0f}, 0.0f );
  cube.bounds = cube_get_AABB( &cube, cube.base_transform );
  return cube;
}

Grid create_grid(
    AARect::RectType type,
    const AABB &b,
    f32 d,
    f32 width,
    const v3 &color )
{
  Grid g;
  g.rect = AARect( type, d, b, 0 ); 
  g.w = width;
  g.color = color;
  switch ( type ){
    case AARect::PLANE_XY:
     g.dir1 = v3{ 1.0f, 0.0f, 0.0f };
     g.dir2 = v3{ 0.0f, 1.0f, 0.0f };
      break;
    case AARect::PLANE_YZ:
     g.dir1 = v3{ 0.0f, 1.0f, 0.0f };
     g.dir2 = v3{ 0.0f, 0.0f, 1.0f };
      break;
    case AARect::PLANE_ZX:
     g.dir1 = v3{ 0.0f, 0.0f, 1.0f };
     g.dir2 = v3{ 1.0f, 0.0f, 0.0f };
      break;
    default:
      print_error( "Unknown Case!\n" );
      break;
  }

  f32 x = g.rect.bounds.u[ g.rect.d0 ];
  f32 y = g.rect.bounds.l[ g.rect.d0 ];

  f32 z = g.rect.bounds.u[ g.rect.d1 ];
  f32 w = g.rect.bounds.l[ g.rect.d1 ];

  // u is along rect.d0, v is along rect.d1
  g.unit_u = g.w / ( x - y );
  g.unit_v = g.w / ( z - w );


  g.nlines = Float2Int( ( g.rect.bounds.u[g.rect.d0]-g.rect.bounds.l[g.rect.d0] )/width );
  return g;
}

bool hit_grid(
    const Grid &g,
    const Ray &ray,
    float tmin, float tmax,
    HitRecord &record )
{
  return hit_AARect( g.rect, ray, tmin, tmax, record );
}

v3 grid_get_corner_point( const Grid &grid, f32 u, f32 v ){
  int32 nu = Float2Int( u / grid.unit_u );
  int32 nv = Float2Int( v / grid.unit_v );
  return grid.rect.corner +
         nu*grid.w*grid.dir1 +
         nv*grid.w*grid.dir2;
}

bool hit_sphere(
    const Sphere &sph,
    const Ray &ray,
    float tmin, float tmax,
    HitRecord &record )
{
  v3 v = ray.start - sph.c;
  float b = 2 * HMM_DotVec3( ray.direction, v );
  float a = HMM_DotVec3( ray.direction, ray.direction );
  float c = HMM_DotVec3( v,v ) - sph.r*sph.r;

  float dis = b * b - 4 * a * c;
  if ( dis > 0 ){
    float t = ( -b-sqrt( dis ) )/(2.0f*a);
    if ( t > tmin && t < tmax ){
      record.t = t;
      record.p = ray.point_at( t );
      record.n = HMM_NormalizeVec3( record.p - sph.c );
      return true;
    }

    t = ( -b+sqrt( dis ) )/(2.0f*a);
    if ( t > tmin && t < tmax ){
      record.t = t;
      record.p = ray.point_at( t );
      record.n = HMM_NormalizeVec3( record.p - sph.c );
      return true;
    }
  }
  return false;
}

AABB rectangle_AABB( void *rect, const m4 &m ){
  Rectangle &r = *( Rectangle * )rect;
  v3 z[4] = { r.p0, r.p1, r.p2, r.p3 };
  
  v3 min = {FLT_MAX,FLT_MAX,FLT_MAX};
  v3 max = {-FLT_MAX,-FLT_MAX,-FLT_MAX};
  for ( uint i = 0; i < 4; i++ ){
    v3 p = z[i];
    min.X = ( min.X > p.X )?p.X:min.X;
    min.Y = ( min.Y > p.Y )?p.Y:min.Y;
    min.Z = ( min.Z > p.Z )?p.Z:min.Z;

    max.X = ( max.X < p.X )?p.X:max.X;
    max.Y = ( max.Y < p.Y )?p.Y:max.Y;
    max.Z = ( max.Z < p.Z )?p.Z:max.Z;
  }
  for ( uint i = 0; i < 3; i++ ){
    if ( fabs( min[i]-max[i] ) < TOLERANCE ){
      min[i] -= 0.01f;
      max[i] += 0.01f;
    }
  }
  r.box = AABB( min, max );
  return r.box;
}
v3 rotate_vector_by_quaternion(const v3& v, const q4& q){
    // extract the vector part of the quaternion
    v3 u = q.XYZ; 
    // Extract the scalar part of the quaternion
    f32 s = q.W;

    return 2.0f * HMM_DotVec3(u, v) * u
          + (s*s - HMM_DotVec3(u, u)) * v
          + 2.0f * s * HMM_Cross(u, v);
}

v3 rotate_point_by_quaternion( const v3 &p,const v3 &about, const q4 &q ){
  v3 dir = p - about;
  v3 x = rotate_vector_by_quaternion( dir, q );
  return x + about;
}


Rectangle create_rectangle( v3 p0 ) {
  Rectangle rect = {};
  rect.p0 = p0;
  rect.n = v3{0.0f,1.0f,0.0f};
  rect.s1 = v3{1.0f,0.0f,0.0f};
  rect.s2 = v3{0.0f,0.0f,1.0f};
  rect.l1 = 1.0f;
  rect.l2 = 1.0f;
  rect.p1 = rect.p0 + rect.l1 * rect.s1;
  rect.p2 = rect.p0 + rect.l1 * rect.s1 + rect.l2 * rect.s2;
  rect.p3 = rect.p0 + rect.l2 * rect.s2;

  rect.orientation = { 0, 1, 1,1 };
  
  rect.box = rectangle_AABB( &rect, HMM_Mat4d(1.0f) );
  return rect;
}

bool hit_rect(
    Rectangle &r,
    const Ray &ray,
    float tmin,
    float tmax,
    HitRecord &record )
{
  float d = ( HMM_DotVec3( ray.direction, r.n ) );
  if ( fabs( d ) < TOLERANCE )
    return false;
  v3 temp = r.p0 - ray.start;
  float t = HMM_DotVec3( temp, r.n )/d ;
  if ( t > tmin && t < tmax ){
    v3 point = ray.point_at( t );
    v3 t1 = point - r.p0;
    f32 d1 = HMM_DotVec3( r.s1, t1 );
    f32 d2 = HMM_DotVec3( r.s2, t1 );

    if ( ( d1 > 0 && d1 < r.l1 ) && ( d2 > 0 && d2 < r.l2 ) ){

      return true;
    }
    return false;  
  }
  return false;
}
