#ifndef PRIMITIVES_H
#define PRIMITIVES_H
#include <float.h>
#include "HandmadeMath.h"
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
struct Material;

struct Sphere {
  v3 c;
  float r;
  Material *m;
  AABB box;
  Sphere ( v3 center, float radius ):c(center), r(radius){}

  Sphere ( v3 center, float radius, Material *x ):
    c(center), r(radius), m(x){}

};

AABB sphere_aabb( const Sphere &sph ){
  v3 r = { sph.r, sph.r, sph.r };
  return AABB( sph.c - r, sph.c + r );
}

AABB sphere_aabb( Sphere *sph ){
  v3 r = { sph->r, sph->r, sph->r };
  return AABB( sph->c - r, sph->c + r );
}
struct Plane {
  v3 p;
  v3 n;
  Material *m;
  AABB box;
  Plane (): m(NULL){}

  Plane ( v3 point, v3 normal ):p(point), n(normal){}

  Plane ( v3 point, v3 normal, Material *x, AABB b ):
    p(point), n(normal), m(x){ box = b; }

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
  Material *m;
  AARect(){}
  AARect ( RectType t, float dist, AABB b,int flip, Material *mat ):
    type(t),d(dist), bounds(b), d0( t ), d1( (t+1)%3 ), m( mat )
  {
    int sign = ( flip )?-1:1;
    switch ( t ){
      case PLANE_XY:
        n = v3{ 0.0f, 0.0f, sign * 1.0f };
        ndim = 2;
        break;
      case PLANE_YZ:
        n = v3{ sign*1.0f,0.0f, 0.0f };
        ndim = 0;
        break;
      case PLANE_ZX:
        n = v3{ 0.0f, sign*1.0f, 0.0f };
        ndim = 1;
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
    float t1 )
{
  
  // Calculate for x and y
  // For eg. for direction = { -1.0f, -1.0f ,-1.0f }
  // and bounds lower=[ -5, -5, -5 ] and  upper=[ -3, -3, -3 ]
  // ray.sign = { 1, 1,1 }
  // xnear = box.bounds[ 1 ].X = -3
  // xfar = box.bounds[ 1 - 1 ].X = -5
  float xnear = box.bounds[ ray.sign[0] ].X; 
  float xfar = box.bounds[ 1-ray.sign[0] ].X;

  float ynear = box.bounds[ ray.sign[1] ].Y; 
  float yfar = box.bounds[ 1-ray.sign[1] ].Y;

  float znear = box.bounds[ ray.sign[2] ].Z; 
  float zfar = box.bounds[ 1-ray.sign[2] ].Z;

  float tmin = ( xnear - ray.start.X ) * ray.inv_dir.X;
  float tmax = ( xfar - ray.start.X ) * ray.inv_dir.X;

  float ytmin = ( ynear - ray.start.Y ) * ray.inv_dir.Y;
  float ytmax = ( yfar - ray.start.Y ) * ray.inv_dir.Y;
  
  // Check if ray lies inside the bounds
  if ( ytmin > tmax || tmin > ytmax ){ return false; }
  
  if ( ytmin > tmin ){ tmin = ytmin; } // Choose max of the two tmin values
  if ( ytmax < tmax ){ tmax = ytmax; } // Chost min of the two tmax values
  
  // this way tmin and tmax lie at the boundary of the AABB

  float ztmin = ( znear - ray.start.Z ) * ray.inv_dir.Z;
  float ztmax = ( zfar - ray.start.Z ) * ray.inv_dir.Z;

  if ( ztmin > tmax || tmin > ztmax ){ return false; }

  if ( ztmin > tmin ){ tmin = ztmin; } // Choose max of the two tmin values
  if ( ztmax < tmax ){ tmax = ztmax; } // Chost min of the two tmax values
  
  // tmin < tmax is always true
  return ( ( tmin < t1 ) && ( tmax > t0 ) );
}

struct Rectangle {
  v3 p0,p1,p2,p3; //four points 
  v3 s1, s2; // two unit vectors
  v3 n; // unit normal vector

  f32 l1,l2; // length along s1 and s2 resp
  Material *m;
  AABB box;
};

AABB rectangle_AABB( Rectangle &r ){
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

#endif
