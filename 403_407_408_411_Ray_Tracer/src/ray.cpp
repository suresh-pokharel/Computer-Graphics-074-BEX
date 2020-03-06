#define HANDMADE_MATH_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define OS_LINUX_CPP
//#define OS_WINDOWS_CPP
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <float.h>
#include <vector>

#include "HandmadeMath.h"
#include "prng.h"
#include "common.h"
#include "stb_image_write.h"
#include "primitives.h"
#include "texture.h"
#include "ray_data.h"
#define ANTI_ALIASING_ON 
#if 1
typedef unsigned int uint;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
#endif


inline bool refract( const v3 &n, const v3 &v, float ri, v3 &ret ){
  v3 unit_v = HMM_NormalizeVec3( v );
  float angle = ( HMM_DotVec3( n, unit_v ) );
  float dis = 1.0f - ( ri * ri )*( 1.0f - angle * angle );
  if ( dis < 0.0f ) return false;
  ret = ri*(unit_v-angle*n) - sqrt( dis ) * n;
  return true;
}

inline float schlick( float c, float ri ){
  // Gives a parameter that is used to decide whether the 
  // light is supposed to reflect of refract
  float r0 = ( 1-ri )/(1+ri);
  r0 *= r0;
  return r0 + ( 1-r0 )*pow( 1-c, 5 );
}

v3 random_in_unit_disk( ){
  v3 y;
  float dist;
  do {
    v3 p = { prng_float(), prng_float(), 0.0f };
    y = 2 * p - v3{1.0f,1.0f,0.0f};
    dist = HMM_DotVec3( p, p );
  } while( dist >= 1.0f );
  return y;
}


#if 0

v3 random_in_unit_sphere( void ){
  v3 v = { prng_float(), prng_float(), prng_float() };
  float m = HMM_DotVec3( v, v );
  if ( m < 1.0f ) return v;
  return HMM_NormalizeVec3( v )/m;
}

#else
v3 random_in_unit_sphere( void ){
  float dist;
  v3 p,y;
  do {
    // Generate a random point between (-1,-1,-1 ) and ( 1,1,1 )
    p = { prng_float(),prng_float(), prng_float()}; 
    y = v3{ -1.0f, -1.0f, -1.0f } + 2 * p;
    dist = HMM_DotVec3( y , y );
  } while ( dist >= 1.0f );
  return y;
}

#endif




struct Camera {
  v3 origin;
  v3 lower_left;
  v3 front, right, up;
  v3 horizontal;
  v3 vertical;
  float lens_radius; 

  Camera ( float vfov,float aspect_ratio ){ 
    float half_height = tan( HMM_ToRadians(vfov)/2 );
    float half_width = aspect_ratio * half_height;
    lower_left = { -half_width, -half_height, -1.0f};
    horizontal = { 2*half_width, 0.0f, 0.0f };
    vertical = { 0.0f, 2*half_height, 0.0f };
    origin = { 0.0f, 0.0f, 0.0f };
  }

  Camera (
      v3 look_from,
      v3 look_at,
      float z,
      float vfov,
      float aspect_ratio,
      float aperture,
      float focal_dist)
  {
    float fd = focal_dist;
    origin = look_from;
    float half_height = z * tan( HMM_ToRadians(vfov)/2 );
    float half_width = aspect_ratio * half_height;
    
    front = HMM_NormalizeVec3( look_from-look_at );   
    v3 aup = { 0.0f, 1.0f, 0.0 };

    right = HMM_Cross( aup, front);
    up = HMM_Cross( front, right);

    lower_left =origin-fd*z*front-right*fd* half_width-up*fd*half_height;
    lens_radius = aperture/2;
    horizontal = 2 * half_width*fd * right;
    vertical = 2 * half_height *fd* up;
  }

  Camera () {}

  inline Ray get_ray( float u, float v ){
    v3 x = lens_radius * random_in_unit_disk();
    v3 off = x.X * right + x.Y * up;
    v3 start = origin + off;
    return Ray( start, lower_left - start + u * horizontal+ v *vertical);
  }
};
struct HitRecord{
  float t;
  v3 p;
  v3 n;
  Material *m;
};



enum MaterialType {
  MATERIAL_PURE_DIFFUSE,
  MATERIAL_DIFFUSE,
  MATERIAL_METALLIC,
  MATERIAL_GLASS,
  MATERIAL_DIFFUSE_LIGHT,
  MATERIAL_SPOT_LIGHT
};


typedef bool (*ScatterFunc)(
    const HitRecord &h,
    const Ray &in,
    v3 &attenuation,
    Ray &out );

struct World {
  Sphere *spheres;
  uint sph_count;
  uint sph_cap;

  Plane *planes;
  uint plane_count;
  uint plane_cap;

  AARect *aa_rects;
  uint aa_rect_count;
  uint aa_rect_cap;

  Rectangle *rectangles;
  uint rect_count;
  uint rect_cap;

};


struct Material {
  MaterialType type;
  ScatterFunc scatter;
  Texture *albedo;
  union {
    struct {
      float fuzz;
    };

    struct {
      float ri; // w.r.to air
    };

    struct {
      v3 diff_light_color;
    };

    struct {
      v3 spot_light_color;
      float angle;
    };
  };
  
  Material (){}
  Material ( MaterialType t, ScatterFunc func,Texture *a, float f ):
    type(t), scatter( func ), albedo( a )
  {
    switch ( t ){
      case MATERIAL_PURE_DIFFUSE:
      case MATERIAL_DIFFUSE:
        break;
      case MATERIAL_METALLIC:
        fuzz = f; 
        break;
      case MATERIAL_GLASS:
        ri = f;
        break;
      default:
        fprintf( stderr, "Unknown material type!" );
        assert( 0 );
        break;
    }
  }
};

bool hit_plane(
    Plane &p,
    const Ray &ray,
    float tmin,
    float tmax,
    HitRecord &record )
{
  float d = HMM_DotVec3( ray.direction, p.n );
  if ( abs( d ) < TOLERANCE )
    return false;
#if 0 
  v3 temp = ray.start - p.p;
#else
  v3 temp = p.p - ray.start;
#endif
  float t = HMM_DotVec3( temp, p.n )/d ;
  if ( t > tmin && t < tmax ){
    record.t = t;
    record.p = ray.point_at( t );
    record.n = p.n;
    record.m = p.m;
    return true;
  } 
  return false;
}

bool pure_diffuse_scatter(
    const HitRecord &rec,
    const Ray &in,
    v3 &attenuation,
    Ray &out )
{
  v3 dir = rec.n + random_in_unit_sphere();
  out = Ray( rec.p , dir );
  Texture *t = rec.m->albedo;
  attenuation = t->get_color( t, 0,0, rec.p );
  return true;
}


bool metallic_scatter(
    const HitRecord &rec,
    const Ray &in,
    v3 &attenuation,
    Ray &out )
{
  v3 dir = HMM_Reflect( HMM_NormalizeVec3(in.direction), rec.n ) +
           rec.m->fuzz * random_in_unit_sphere();
  out = Ray( rec.p, dir );
  Texture *t = rec.m->albedo;
  attenuation = t->get_color( t, 0,0, rec.p );
  return ( HMM_DotVec3( out.direction, rec.n ) > 0 ) ;
}

bool refraction_scatter(
    const HitRecord &rec,
    const Ray &in,
    v3 &attenuation,
    Ray &out )
{
  Material *m = rec.m;
  float ri;
  Texture *t = rec.m->albedo;
  attenuation = t->get_color( t, 0,0, rec.p );

  v3 outward_normal;
  float cosine = HMM_DotVec3( HMM_NormalizeVec3( in.direction ), rec.n );
  if ( cosine > 0.0f ){
    // Entering from medium to air
    ri = m->ri; // ( n1=ri, n2 = 1.0f )
    outward_normal = -rec.n;
    cosine *= m->ri ;
  } else {
    ri = 1/m->ri; // ( n1 = 1.0f, n2 = ri )
    outward_normal = rec.n;
    cosine = -cosine;
  }
  
  v3 refract_dir = {0};
  float reflect_prob;
  v3 reflect_dir = HMM_Reflect( HMM_NormalizeVec3(in.direction), rec.n ); 
  if ( refract( outward_normal, in.direction,ri, refract_dir ) ){
    // if refraction is possible,
    // calculate the possibility of reflection
    reflect_prob = schlick( cosine, m->ri ); 
  } else {
    // reflect the ray if refraction is not possible
    reflect_prob = 1.0f; 
  }
  
  if ( prng_float() < reflect_prob ){
    out = Ray( rec.p, reflect_dir );
  } else {
    out = Ray( rec.p, refract_dir );
  }

  return true;
}
inline bool no_scatter(
    const HitRecord &h,
    const Ray &in,
    v3 &attenuation,
    Ray &out ){ return false; }

Material create_diffuse_light( const v3 &color ){
  Material m;
  m.type = MATERIAL_DIFFUSE_LIGHT;
  m.scatter = no_scatter;
  m.diff_light_color= color;
  m.albedo = NULL;
  return m;
}

Material create_spot_light( const v3 &color, float angle ){
  Material m;
  m.type = MATERIAL_SPOT_LIGHT;
  m.scatter = no_scatter;
  m.spot_light_color= color;
  m.angle = HMM_CosF( HMM_RADIANS(angle) );
  m.albedo = NULL;
  return m;
}

Material create_material_metallic( Texture &tex, float fuzz ){
  Material m;
  m.type = MATERIAL_METALLIC;
  m.scatter = metallic_scatter;
  m.albedo = &tex;
  m.fuzz = fuzz;
  return m;  
}

Material create_material_glass( Texture &tex, float ri ){
  Material m;
  m.type = MATERIAL_GLASS;
  m.scatter = refraction_scatter;
  m.albedo = &tex;
  m.ri = ri;
  return m;
}

Material create_material_pure_diffuse( Texture &tex ){
  Material m;
  m.type = MATERIAL_PURE_DIFFUSE;
  m.scatter = pure_diffuse_scatter;
  m.albedo = &tex;
  return m;
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
      record.t = t;
      record.p = ray.point_at( t );
      record.n = r.n;
      record.m = r.m; 
      return true;
    }
    return false;  
  }
  return false;
}


bool hit_sphere(
    Sphere &sph,
    const Ray &ray,
    float tmin,
    float tmax,
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
      record.m = sph.m;
      return true;
    }

    t = ( -b+sqrt( dis ) )/(2.0f*a);
    if ( t > tmin && t < tmax ){
      record.t = t;
      record.p = ray.point_at( t );
      record.n = HMM_NormalizeVec3( record.p - sph.c );
      record.m = sph.m;
      return true;
    }
  }
  return false;
}

bool hit_AARect(
    AARect &rect,
    const Ray &ray,
    float tmin,
    float tmax,
    HitRecord &record )
{

  //if ( fabs( ray.direction[ rect.ndim ] ) < TOLERANCE ) return false;
  float check = HMM_DotVec3( ray.direction, rect.n );
  if ( check > 0.0f ) return false;
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

  record.p[rect.d0] = a;
  record.p[rect.d1] = b;
  record.p[rect.ndim] = rect.d;
  
  record.n = rect.n;
  record.m = rect.m; 
  record.t = t;
  return true;

}


bool world_check_hit(
    World &w,
    const Ray &ray,
    HitRecord &rec )
{
  bool hit_anything = false;
  HitRecord tr;
  float tmin = 0.001f;
  float tmax = 1000.0f;
  for ( uint i = 0; i < w.sph_count; i++ ){
    if ( hit_sphere( w.spheres[i], ray, tmin, tmax, tr )){
      tmax = tr.t;
      hit_anything = true;
      rec = tr;
    }
  }

  for ( uint i = 0; i < w.plane_count; i++ ){
    if ( hit_plane( w.planes[i], ray, tmin, tmax, tr )){
      tmax = tr.t;
      hit_anything = true;
      rec = tr;
    }
  }
  return hit_anything;
}



void world_add_sphere( World &w, const Sphere &sph ){
  assert( w.sph_count + 1 <= w.sph_cap );
  w.spheres[ w.sph_count++ ] = sph;
}

void world_add_plane( World &w, Plane p ){
  assert( w.plane_count + 1 <= w.plane_cap );
  w.planes[ w.plane_count++ ] = p;
}
void world_add_rect( World &w, AARect r ){
  assert( w.rect_count+ 1 <= w.rect_cap );
  w.aa_rects[ w.rect_count++ ] = r;
}
void world_add_rectangle( World &w, Rectangle &r ){
  assert( w.rect_count+ 1 <= w.rect_cap );
  w.rectangles[ w.rect_count++ ] = r;
}

struct PrimInfo {
  typedef enum PrimType {
    SPHERE,
    PLANE,
    RECTANGLE
  } PrimType;

  PrimType type;
  AABB box;
  v3 centroid;
  void *data;

  PrimInfo ( PrimType t, void *p, AABB b ){
    type = t;
    box = b;
    centroid = 0.5f * (box.l + box.u);
    data = p;
  }
};


int partition(
    void *start,
    int length,
    size_t size,
    int (*filter)(void *, void *),
    void *extra_args )
{
  int left = 0;
  char temp[1024];
  assert( sizeof( temp ) >= size );
  for ( int i = 0; i < length; i++ ){
    char *current = ( char *)start + i * size;
    if ( filter( (void *)current, extra_args ) ){
      //swap with the leftmost element
      char *l = (char *)start + left *size;
      char *c = (char *)start + i * size;
      memcpy( temp, l , size );
      memcpy( l, c, size );
      memcpy( c, temp, size );
      left++;
    }
  }
  return left;
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

struct PrimInfoExArgs{
  int dim;
  float pmid;
};

int mid_point_filter( void *x, void *ex_args ){
  PrimInfo *pi = ( PrimInfo *)x;
  PrimInfoExArgs *args = (PrimInfoExArgs *)ex_args;
  return pi->centroid[ args->dim ] < args->pmid;
}
struct BVHNode{
  AABB box;
  BVHNode *left, *right; // NULL for leaf primitives
  int split_axis;
  int first_offset, num_prim; // only valid for leaf nodes
  
};

BVHNode *bvh_create_leaf( Arena *arena, int first, int n, const AABB &bound){
  BVHNode *node = (BVHNode *)arena_alloc( arena, sizeof( BVHNode ), 8);
  node->first_offset = first;
  node->num_prim = n;
  node->box = bound;
  return node;
}

BVHNode *bvh_create_interior( Arena *arena, BVHNode *l, BVHNode *r, int axis ){
  BVHNode *node = (BVHNode *)arena_alloc( arena, sizeof( BVHNode ), 8);
  node->num_prim = 0;
  node->left = l;
  node->right = r;
  node->split_axis = axis;
  node->box = AABB_union( l->box , r->box );
  return node;
}

int get_max_bound_dim( const AABB &b ){
  v3 diff = b.u - b.l;

  if ( diff.X > diff.Y && diff.X > diff.Z )
    return 0;
  else if ( diff.Y > diff.Z )
    return 1;
  else 
    return 0;
}

BVHNode *bvh_recursive_build(
    Arena *arena,
    PrimInfo *info,
    int start,
    int end,
    std::vector<PrimInfo> &ordered_prims )
{

  AABB total_bound;
  for ( int i = start; i < end; i++ ){
    total_bound = AABB_union( total_bound, info[i].box );
  }
  int len = end - start;
  if ( len == 1 ){
    // only a single primitive
    BVHNode *n = bvh_create_leaf( arena, ordered_prims.size(),1,total_bound );
    ordered_prims.push_back( info[start] );
    return n;
  }

  AABB bounds; 
  for ( int i = start ;i < end; i++ ){
    bounds = AABB_union( bounds, info[i].centroid );
  }
  
  int dim = get_max_bound_dim( bounds );
  
  if ( bounds.l[ dim ] == bounds.u[dim] ){
    // Only one AABB exists, i.e. there are
    // a number of primitive with the same AABB
    BVHNode *n = bvh_create_leaf( arena, ordered_prims.size(),len,total_bound );
    for ( int i = start; i < end; i++ ){
      ordered_prims.push_back( info[i] );
    }
    return n;
  }

  float pmid = 0.5f * ( bounds.l[dim] + bounds.u[dim] );
  PrimInfoExArgs args = { dim, pmid };

  int mid = start + partition(
              (void *)(info + start),
              len,
              sizeof(*info),
              mid_point_filter,
              &args );
  if ( mid == start || mid == end ){
    // We were not able to find a good partition
    BVHNode *n = bvh_create_leaf( arena, ordered_prims.size(),len,total_bound );
    for ( int i = start; i < end; i++ ){
      ordered_prims.push_back( info[i] );
    }
    return n;
  } else {
    BVHNode *l = bvh_recursive_build( arena, info, start, mid, ordered_prims);
    BVHNode *r = bvh_recursive_build( arena, info, mid, end, ordered_prims);
    return bvh_create_interior( arena, l, r, dim ); 
  }
}
    

BVHNode *create_bvh_tree(
    Arena *arena,
    const World &w,
    std::vector<PrimInfo> &ordered_prims)
{
  std::vector<PrimInfo> prim;
  for ( size_t i = 0; i < w.sph_count; i++ ){
    prim.push_back(
        PrimInfo( PrimInfo::SPHERE,
          (void *)( w.spheres+i ),
          sphere_aabb( *( w.spheres + i ) )
        )
    );
  }

  for ( size_t i = 0; i < w.rect_count; i++ ){
    prim.push_back(
        PrimInfo( PrimInfo::RECTANGLE,
          (void *)( w.rectangles+ i ),
          rectangle_AABB( *(w.rectangles+i) ) )
        );
  }
#if 0
  for ( size_t i = 0; i < w.plane_count; i++ ){
    prim.push_back(
        PrimInfo( PrimInfo::PLANE,
          (void *)( w.planes+i ),
          (w.planes+i)->box
          )
    );
  }
  for ( size_t i = 0; i < w.rect_count; i++ ){
    prim.push_back(
        PrimInfo( PrimInfo::RECTANGLE,
          (void *)( w.rects+i ),
          (w.rects+i)->bounds
          )
    );
  }
#endif

  return bvh_recursive_build( arena, &prim[0], 0, prim.size(), ordered_prims );
}

bool bvh_leaf_hit( 
    BVHNode *node,
    const Ray &r,
    float tmin,
    float tmax,
    HitRecord &rec,
    std::vector<PrimInfo> &ordered_prims)
{
  bool hit_anything = false;
  HitRecord temp;
  for ( int i = 0;
        i < node->num_prim;
        i++ )
  {
    PrimInfo &p = ordered_prims[ node->first_offset+ i];
    switch ( p.type ){
      case PrimInfo::SPHERE:
        if( hit_sphere( *( (Sphere*)p.data), r, tmin, tmax, temp) ){
          hit_anything = true;
          tmax = temp.t;
          rec = temp;
        }
        break;
      case PrimInfo::PLANE:
        if( hit_plane( *( (Plane*)p.data), r, tmin, tmax, temp) ){
          hit_anything = true;
          tmax = temp.t;
          rec = temp;
        }
        break;
      case PrimInfo::RECTANGLE:
#if 0
        if( hit_AARect( *( (AARect*)p.data), r, tmin, tmax, temp) ){
          hit_anything = true;
          tmax = temp.t;
          rec = temp;
        }
#endif
        if( hit_rect( *( (Rectangle *)p.data),
              r, tmin, tmax, temp) )
        {
          hit_anything = true;
          tmax = temp.t;
          rec = temp;
        }
        break;
      default:
        break;
    }
  }
  return hit_anything;
}

bool bvh_traversal_hit( 
    BVHNode *root,
    const Ray &r,
    float tmin,
    float tmax,
    HitRecord &rec,
    std::vector<PrimInfo> &ordered_prims)
{
  if ( AABB_hit( root->box, r, tmin, tmax ) ){
    HitRecord lrec , rrec;
    bool lhit, rhit;
    if ( root->num_prim > 0 ){
      // we are in a leaf node
      return bvh_leaf_hit( root,r, tmin, tmax,rec, ordered_prims );
    } else {
      lhit = bvh_traversal_hit( root->left,
              r, tmin, tmax ,lrec,ordered_prims );
      rhit = bvh_traversal_hit( root->right,
              r,tmin,tmax,rrec,ordered_prims );
    }

    if ( lhit && rhit ){
      if ( lrec.t < rrec.t ) 
        rec = lrec;
      else rec = rrec;
      return true;
    } else if ( lhit ){
      rec = lrec;
      return true;
    } else if ( rhit ){
      rec = rrec;
      return true;
    } 
  } 
  return false;
}
v3 get_ray_color(
    BVHNode *root,
    const Ray &ray,
    int depth,
    std::vector<PrimInfo> &ordered_prims )
{
  v3 direction = HMM_NormalizeVec3( ray.direction );
  
  HitRecord rec;
  v3 attn;
  Ray out;

  if ( bvh_traversal_hit( root, ray,0.001f,FLT_MAX, rec,ordered_prims ) ){
    v3 emitted = { 0.0f, 0.0f, 0.0f };
    switch ( rec.m->type ){
      case MATERIAL_DIFFUSE_LIGHT: 
        return rec.m->diff_light_color;
      case MATERIAL_SPOT_LIGHT: {
        if ( depth == 0 ) return rec.m->spot_light_color; 
        float x= MAX(-HMM_DotVec3(
              HMM_NormalizeVec3(ray.direction),
              rec.n ), 0 );
        if ( x > rec.m->angle )
          return rec.m->spot_light_color;
        else
          return HMM_PowerF(x,4) * rec.m->spot_light_color;
        break;
      }
      default:
        break;
    }
    if ( rec.m->scatter( rec, ray, attn, out ) && ( depth < 30 ) ){
        return attn * get_ray_color( root, out, depth+1,ordered_prims );
    } else if ( depth >= 30  ){
      Texture *t = rec.m->albedo;
      emitted = t->get_color( t, 0,0, rec.p );
    }
    return emitted;
  }
#if 0    
    float t = 0.5f * ( direction.Y + 1.0f );
    
    v3 start= { 1.0f, 1.0f, 1.0f };
    v3 end = { 0.5f, 0.7f, 1.0f };
    return ( 1.0 - t ) * start + t * end;
#else
    return v3{0.0f,0.0f,0.0f};
#endif
}


void print_aabb( const AABB &b ){
  fprintf( stdout, "Max. bound: " );
  print_v3( b.u );
  fprintf( stdout,"\nMin. bound: " );
  print_v3( b.l );
}

void print_bvh_info( BVHNode *node ){
struct BVHNode{
  AABB box;
  BVHNode *left, *right; // NULL for leaf primitives
  int split_axis;
  int first_offset, num_prim; // only valid for leaf nodes
  
};
  fprintf(stdout,"The bounding box is: \n" );
  print_aabb( node->box );
  fprintf( stdout, "\n" );
  if ( node->num_prim > 0 ){
    fprintf( stdout,"leaf node with %d primitives, starting at %d.\n",
            node->num_prim, node->first_offset );
  } else {
    fprintf( stdout, "Interior node\n" );
  }
}

void bvh_tree_print( BVHNode *node ){
  print_bvh_info( node );
  fprintf(stdout,"========================================\n");
  if(node->left){
    fprintf(stdout,"Left Node:\n");
    bvh_tree_print( node->left ); 
    fprintf(stdout,"-----------------------------------------\n");
  }
  if(node->right){
    fprintf(stdout,"Right Node:\n");
    bvh_tree_print( node->right );
    fprintf(stdout,"-----------------------------------------\n");
  }
}

void print_sphere_info( Sphere *sph ){
  fprintf(stdout,"Center: ");
  print_v3( sph->c );
  fprintf(stdout,"\nRadius: %f", sph->r );
}
void print_plane_info( Plane *p){
  fprintf(stdout,"Normal: ");
  print_v3( p->n );
  fprintf(stdout,"\nPoint: ");
  print_v3( p->p );
}
void print_rect_info( Rectangle *r ){
  fprintf(stdout,"Normal: " );
  print_v3( r->n );
  fprintf( stdout, "\ncorner point: " );
  print_v3( r->p0 );

  fprintf( stdout, "\nVector s1: " );
  print_v3( r->s1 );
  fprintf( stdout, "\nVector s2: " );
  print_v3( r->s2 );

  fprintf( stdout, "\nl1 = %f, l2 = %f\n", r->l1, r->l2 );

}

void print_priminfo( PrimInfo *p ){
  switch ( p->type ){
    case PrimInfo::SPHERE:
      fprintf(stdout,"Sphere\n");
      print_sphere_info( (Sphere *)p->data );
      break;
    case PrimInfo::PLANE:
      fprintf(stdout,"Plane\n" );
      print_plane_info( (Plane *)p->data );
      break;
    case PrimInfo::RECTANGLE:
      fprintf(stdout,"Rectangle\n" );
      print_rect_info( (Rectangle *)p->data );
      break;
  }
}

void world_decode_sphere_data( 
    World &w,
    const DumpObjectData &data,
    Texture *textures,
    Material *materials)
{
  
}


Texture dump_get_texture( const DumpObjectData &d, Perlin *perlin){
  Texture t;
  switch( d.texture_type ){
    case DumpObjectData::TEXTURE_PLAIN_COLOR:
      t = create_texture_plain( d.texture_data.color );
      break;
    case DumpObjectData::TEXTURE_CHECKER:
      t = create_texture_checker( d.texture_data.checker_color[0],
                                  d.texture_data.checker_color[1],
                                  d.texture_data.freq );
      break;
    case DumpObjectData::TEXTURE_MARBLE:
      t = create_texture_marble( perlin,
                                 d.texture_data.marble_color);
      break;
    default:
      fprintf( stderr, "Unknown Texture detected!\n" );
      break;
  }
  return t;
}
Material dump_get_material(
    const DumpObjectData &d,
    Texture &tex )
{
  Material m;
  switch( d.material_type ){
    case DumpObjectData::MATERIAL_DIFFUSE_LIGHT:
      m = create_diffuse_light( d.material_data.diff_light_color );
      break;
    case DumpObjectData::MATERIAL_PURE_DIFFUSE:
      m = create_material_pure_diffuse( tex );
      break;
    case DumpObjectData::MATERIAL_METALLIC:
      m = create_material_metallic( tex, d.material_data.fuzz );
      break;
    case DumpObjectData::MATERIAL_GLASS:
      printf("Creating glass material!\n");
      m = create_material_glass( tex, d.material_data.ri);
      break;
    case DumpObjectData::MATERIAL_SPOT_LIGHT:
      m = create_spot_light( d.material_data.spot_light_color, 
                             d.material_data.angle );
      break;
  }
  return m;
}

void world_get_from_file(
    const char *path,
    Texture *textures,
    Material *materials,
    Perlin *perlin,
    World &w, 
    Camera &camera,
    float *aspect_ratio )
{
  FILE *fp = fopen( path, "rb" );
  
  DumpCameraData camera_data;
  fread( &camera_data, sizeof(camera_data),1, fp );

  Camera c(
      camera_data.look_from,
      camera_data.look_at,
      camera_data.z,
      camera_data.vfov,
      camera_data.aspect_ratio,
      camera_data.aperture,
      camera_data.focal_dist
  );
  camera = c;
  *aspect_ratio = camera_data.aspect_ratio;
  uint32 data_count;
  fread( &data_count, sizeof(data_count), 1, fp );
  DumpObjectData *object_data = array_allocate( DumpObjectData,
                                         data_count );
  fread( object_data, sizeof(*object_data), data_count, fp );

  
  for ( uint i = 0; i < data_count; i++ ){
    const DumpObjectData &data = object_data[i];
    array_push( textures, dump_get_texture( data, perlin ) );
    array_push( materials, dump_get_material( data,
          textures[ array_length(textures)- 1] ) );
    Material *m = &materials[ array_length(materials) - 1];
    switch ( data.type ){
      case DumpObjectData::RECTANGLE:
        {
          Rectangle rect;
          rect.p0 = data.object_data.p0;
          rect.s1 = data.object_data.s1;
          rect.s2 = data.object_data.s2;
          rect.n = data.object_data.n;
          rect.l1 = data.object_data.l1;
          rect.l2 = data.object_data.l2;
          rect.p1 = rect.p0 + rect.l1 * rect.s1;
          rect.p2 = rect.p0 + rect.l1 * rect.s1 + rect.l2 * rect.s2;
          rect.p3 = rect.p0 + rect.l2 * rect.s2;
          rect.box = rectangle_AABB( rect ) ;
          rect.m = m;
          world_add_rectangle( w, rect );
        }
        break;

      case DumpObjectData::SPHERE:
        {
          Sphere sphere( data.object_data.center, 
              data.object_data.radius, m );
          world_add_sphere( w, sphere );
        }
        break;
      default:
        break;
    }
  }
  array_free( object_data );
}



int main( ){
  prng_seed();
  int ny = 300;
  uint64 samples = 100;
  Arena perlin_arena = new_arena();
  Perlin perlin = create_perlin( &perlin_arena, 4.0f,256 );
//  Texture tex_perlin = create_texture_perlin( &perlin );
  
  Texture *textures = array_allocate( Texture, 100 );
  Material *materials = array_allocate( Material, 100 );
  
  World world = {};
  Camera camera;


  v3 lf = { 0,1,5.5 };
  v3 lat = { 0,1,-1 };
  float f = HMM_LengthVec3( lat - lf );
  world.sph_cap = 20;
  world.spheres = ( Sphere * )malloc(
                  sizeof(Sphere )* world.sph_cap );

  world.plane_cap = 4;
  world.planes = ( Plane * )malloc(
                  sizeof(Plane)* world.plane_cap );

  world.aa_rect_cap= 50;
  world.aa_rects = ( AARect* )malloc(
                  sizeof(AARect)* world.rect_cap);

  world.rect_cap= 50;
  world.rectangles = ( Rectangle * )malloc( 
                  sizeof(Rectangle) * world.rect_cap );
  assert( world.spheres );
  float aspect_ratio = 0;
  world_get_from_file("./bin/dump_file0.dat",
      textures,
      materials,
      &perlin,
      world,
      camera,
      &aspect_ratio );
  int nx = (int)( ny * aspect_ratio );
  float total_pixels = nx * ny; 
#if 0
  AARect rect1(
      AARect::PLANE_XY,
      -2.0f,
      AABB( v3{ -2.0f, 0.1f ,0.0f }, v3{ 4.0f ,2.0f, 0.0f  } ),
      0,
      &mat_light
      ); 
  world_add_sphere( world,
      Sphere( {0.0f, -1000.0f, 0.0f}, 1000.0f,&mat_pure_diffuse_red));
  //world_add_sphere( world,
  //    Sphere( {0.0f, 2.0f, 0.0f}, 2.0f,&mat_pure_diffuse_red));
  world_add_sphere( world,
      Sphere( {1.0f,0.5f,-1.0f}, 0.5f, &mat_matte_marble) );
  //world_add_sphere( world,
  //    Sphere( {1.0f,1.9f,-1.0f}, 0.5f, &mat_light) );
   world_add_rect( world, rect1 );
  //world_add_rect( world, rect2 );
  //world_add_plane( world,
  //                 Plane( v3{ 0.0f, -1.0f, 0.0f },
  //                        v3{ 0.0f, 1.0f, 0.0f },
  //                        &mat_matte_checker,
  //                  AABB(v3{-100.0f,-0.6f,-100.0f},v3{100.0f, -0.4f, 100.0f}) )
  //               );
   float xleft = -1.5f, xright = 1.5f;
   float ytop = 2.051f, ybot = 0.0f;
   float zfront = 3.5f, zback = 0.0f;
   float zmid = ( zfront + zback )/2;
   float light_dim = ( xright - xleft )/8;
   AARect back(
       AARect::PLANE_XY,
       zback,
       AABB( v3{ xleft, ybot ,0.0f }, v3{ xright ,ytop, 0.0f  } ),
       0,
       &mat_pure_diffuse_white
       ); 
   AARect front(
       AARect::PLANE_XY,
       zfront,
       AABB( v3{ xleft, ybot ,0.0f }, v3{ xright ,ytop, 0.0f  } ),
       1,
       &mat_pure_diffuse_blue
       ); 
   AARect top(
       AARect::PLANE_ZX,
       ytop,
       AABB( v3{ xleft, 0.0f ,zback }, v3{ xright ,0.00f, zfront  } ),
       1,
       &mat_weak_white
       ); 
   AARect bottom(
       AARect::PLANE_ZX,
       ybot,
       AABB( v3{ xleft, 0.0f ,zback }, v3{ xright ,0,zfront  } ),
       0,
       &mat_weak_white
       );
   AARect left(
       AARect::PLANE_YZ,
       xleft,
       AABB( v3{ 0.0f, ybot ,zback }, v3{ 0.0f ,ytop, zfront  } ),
       0,
       &mat_pure_diffuse_green
       ); 
   AARect right(
       AARect::PLANE_YZ,
       xright,
       AABB( v3{ 0.0f, ybot ,zback }, v3{ 0.0f ,ytop, zfront  } ),
       1,
       &mat_pure_diffuse_red
       ); 
   AARect light(
       AARect::PLANE_ZX,
       ytop-0.01f,
       AABB( v3{ -light_dim,0.0f,zmid-light_dim },
             v3{ light_dim ,0.0f,zmid+light_dim  } ),
       1,
       &mat_spotlight
       ); 
  //world_add_sphere( world,
  //    Sphere( {0.0f, -1000.0f, 0.0f}, 1000.0f,&mat_pure_diffuse_white));
  world_add_sphere( world,
      Sphere( {-1.0f,0.5f,2.0f}, 0.5f, &mat_pink_marble) );
  world_add_sphere( world,
      Sphere( {0.0f,0.5f,1.0f}, 0.5f, &mat_pure_metallic) );
  world_add_sphere( world,
      Sphere( {1.0f,0.5f,1.5f}, 0.5f, &mat_white_marble) );
   world_add_rect( world, left);
   world_add_rect( world, right);
   world_add_rect( world, light);
   world_add_rect( world, back);
//   world_add_rect( world, front);
   world_add_rect( world, top );
   world_add_rect( world, bottom);
#endif
  std::vector<PrimInfo> ordered_prims;
  Arena bvh_arena = new_arena();
  BVHNode *tree = create_bvh_tree( &bvh_arena, world, ordered_prims );
#if 1
  for ( size_t i = 0; i < ordered_prims.size(); i++ ){
    fprintf(stdout,"Index %d\n", i );
    print_priminfo( &ordered_prims[i] );
    fprintf(stdout,"\n========================================\n");
  }
  bvh_tree_print( tree );
#endif
//  Ray r = camera.get_ray( 0.5f, 0.5f );
//  v3 color = get_ray_color( tree,r, 0, ordered_prims );
#if 1
  uint8 *buff = ( uint8 *)malloc( 3 * nx * ny * sizeof( uint8 ) );
  uint8 *start = buff;
  uint64 pixel_completed = 0;
  uint pixel_five_percent = (uint)( 0.05 * total_pixels ); 
  int count = 0;
  for ( int j = ny - 1; j >= 0; j-- ){
    for ( int i = 0; i < nx; i++ ){
      v3 color = { 0.0f, 0.0f, 0.0f };
      for ( uint64 k = 0; k < samples ; k++ ){
        float s1 = ( i + prng_float() )/(float)nx;
        float s2 = ( j + prng_float() )/(float)ny;
        Ray r = camera.get_ray( s1, s2 );
        color = color + get_ray_color( tree,r, 0, ordered_prims );
      }
      color = color / samples;
      color = { HMM_SquareRootF( color[0] ),
                HMM_SquareRootF( color[1] ),
                HMM_SquareRootF( color[2] ) };

      int ir = (int)(255.99 * CLAMP( color[0], 0.0f, 1.0f ) );
      int ig = (int)(255.99 * CLAMP( color[1], 0.0f, 1.0f ) );
      int ib = (int)(255.99 * CLAMP( color[2], 0.0f, 1.0f ) );
      
      *start++ = ir & 0xff;
      *start++ = ig & 0xff;
      *start++ = ib & 0xff;
      pixel_completed++;
      if ( pixel_completed >= pixel_five_percent ){
        count++;
        pixel_completed = 0;
        printf("Ray tracing %d percent completed\n", count * 5 );
      }
    }
  }
  
  stbi_write_png( "./images/out.png", nx, ny, 3, buff,3 * nx );
#endif
  return 0;
}
