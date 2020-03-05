#include "ui_primitives.h"
bool AABB_hit( 
    const AABB &box,
    const Ray &ray,
    float t0,
    float t1, 
    HitRecord &record )
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
#if 1
  if ( ( tmin < t1 ) && ( tmax > t0 ) ){
    record.t = tmin;
    return true;
  }
  return false;
#else
  return ( ( tmin < t1 ) && ( tmax > t0 ) );
#endif

}



bool hit_AARect(
    const AARect &rect,
    const Ray &ray,
    float tmin,
    float tmax,
    HitRecord &record )
{

  //if ( fabs( ray.direction[ rect.ndim ] ) < TOLERANCE ) return false;
  // TODO: We MOST probably don't need a dot product here! Simple multiplication
  // should do
  //float check = HMM_DotVec3( ray.direction, rect.n );
  //if ( check > 0.0f ) return false;
  float t = ( rect.d - ray.start[rect.ndim] )/ray.direction[rect.ndim];
  if ( t < tmin || t > tmax ) return false; 
  float a = ray.start[ rect.d0 ] + t * ray.direction[ rect.d0 ];
  float b = ray.start[ rect.d1 ] + t * ray.direction[ rect.d1 ];

 
  if ( a < rect.bounds.l[ rect.d0 ] || a > rect.bounds.u[ rect.d0 ] ){
    return false;
  }
  if ( b < rect.bounds.l[ rect.d1 ] || b > rect.bounds.u[ rect.d1 ] ){
    return false;
  }

  // UV calculation
  float lower0 = rect.bounds.l[ rect.d0 ];
  float lower1 = rect.bounds.l[ rect.d1 ];
  record.u = ( a - lower0 )/( rect.bounds.u[rect.d0] - rect.bounds.l[rect.d0] );
  record.v = ( b - lower1 )/( rect.bounds.u[rect.d1] - rect.bounds.l[rect.d1] );

  record.p[rect.d0] = a;
  record.p[rect.d1] = b;
  record.p[rect.ndim] = rect.d;
  record.n = rect.n;
  record.t = t;
  return true;

}

AABB AABB_union( AABB box, v3 v ){
  v3 lower = {
    MIN( box.l[0], v[0] ),
    MIN( box.l[1], v[1] ),
    MIN( box.l[2], v[2] ),
  };

  v3 upper = {
    MAX( box.u[0], v[0] ),
    MAX( box.u[1], v[1] ),
    MAX( box.u[2], v[2] ),
  };
  return AABB( lower, upper );
}

AABB AABB_union( AABB b1, AABB b2 ){
  v3 lower = {
    MIN( b1.l[0], b2.l[0] ),
    MIN( b1.l[1], b2.l[1] ),
    MIN( b1.l[2], b2.l[2] ),
  };

  v3 upper = {
    MAX( b1.u[0], b2.u[0] ),
    MAX( b1.u[1], b2.u[1] ),
    MAX( b1.u[2], b2.u[2] ),
  };
  return AABB( lower, upper );
}
