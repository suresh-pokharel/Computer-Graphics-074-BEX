#include <vector>
#include <common.h>
#include <limits.h>
#include <float.h>
typedef hmm_vec3 v3;
union AABB{
  enum {
    LOWER = 0,
    UPPER
  };

  v3 bounds[2];
  v3 l,u;

  AABB () {
    bounds[0] = { FLT_MAX, FLT_MAX, FLT_MAX }; 
    bounds[1] = { FLT_MIN, FLT_MIN, FLT_MIN }; 
  }

  AABB (v3 lower, v3 upper){
    bounds[UPPER] = upper;
    bounds[LOWER]=lower;
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


#if 0

struct PrimitiveInfo{
  Primitive *prim;
  Bounds b;
  v3 centroid;

  PrimitiveInfo( Primitive *prim, Bounds x ):
    prim( prim ),
    b(x),
    centroid( 0.5 * x.bounds[0] + 0.5f * x.bounds[1] )
  {}
};


struct BVHNode{
  v3 bounds[2];
  BVHNode *left, *right; // NULL for leaf primitives
  int split_axis;
  int first_offset, num_prim; // only valid for leaf nodes
  
  void init_leaf( int first, int n, v3 *b ){
    first_offset = first;
    num_prim = n;
    bounds[0] = *b;
    bounds[1] = *(b+1);
  }

  void init_interior( BVHNode *l, BVHNode *r, int axis ){
    num_prim = 0;
    left = l;
    right = r;
    split_axis = axis;
    bounds = Union( l->bounds, r->bounds );
  }

};

struct LinearBVHNodes{
};

// This function returns a BVH Tree for all the primitives from 
// info[ start ] to info[ end-1 ]
// if this range has a single prim, creates a leaf node
// else,
// partitions the array into [ start,mid) and [mid,end)
// and passes that to the next recursive call
BVHNode *recursive_build(
    void *mem, // Memory location to stored the ordered prims?
    PrimitiveInfo *info, // Info of the primitives 
    int start,
    int end,
    int *total_nodes, // The total number of bvh nodes created
    Primitive *ordered_prim )
{

  BVHNode *n = create_BVHNode( mem );
  (*total_nodes)++;

  // Calculate the bounds of all the primitives
  // that have been passed to this call
  for ( int i = start; i < end; i++ ){
    bounds = Union( bounds, info[i].bounds );
  }

  int nprim = end - start;
  if ( nprim == 1 ){
    // Create a leaf BVHNode
    int off = ordered_prim.size();
    // Append all the primitives overlapping the leaf
    for ( int i = start; i < end; i++ ){
      Primitive *p = info[i].prim;
      array_push( ordered_prim, p );
    }
    n = init_leaf( off, nprim, bounds );
    return n;
  } else {
    // Calculate the centroid bounds
    AABB centroid_bound;  // AABB that contains all the centroids
    for ( int i = start; i < end ; i++ ){
      centroid_bounds = Union( centroid_bounds, info[i].centroid ); 
    }
    // get the dimension along which the centroid is the longest
    int dim = get_max_extent_dim( centroid_bounds );
    int mid = ( start + end )/2;
    if ( centroid_bounds.max_b[ dim ] == centroid_bounds.min_b[dim] ){
      // There is only a single primititve with a point as AABB,
      // so create a leaf node
      int off = ordered_prim.size();
      // Append all the primitives overlapping the leaf
      for ( int i = start; i < end; i++ ){
        Primitive *p = info[i].prim;
        array_push( ordered_prim, p );
      }
      n = init_leaf( off, nprim, bounds );
      return n;
    } else {
      // Partition the primitives based on some method
      // 
      // Partition using mid-point of centroid
      float pmid = (centroid_bounds.max_b[dim] + centroid_bounds.min_b[dim])/2;
      PrimInfoExArgs args = { dim,pmid};
      int m = partition(
                (void *)(info + start),
                ( end - start ),
                sizeof( PrimitiveInfo ),
                mid_point_filter,
                (void *)&args );
       
      if ( m == start || m == end ){
        // Bad partition with the mid point
        // Choose a different one
      }
      // Create the left and right sub-trees
      BVHNode *left = recursive_build(
                      mem, // Memory location to stored the ordered prims?
                      info, // Info of the primitives 
                      start,
                      mid,
                      total_nodes, // The total number of bvh nodes created
                      ordered_prim );
      BVHNode *right= recursive_build(
                      mem, // Memory location to stored the ordered prims?
                      info, // Info of the primitives 
                      mid,
                      end,
                      total_nodes, // The total number of bvh nodes created
                      ordered_prim );
      n = init_interior( left, right, dim );

    }
  }
}

struct MortonPrimitive {
  Primitive *prim;
  uint32 code;
};

v3 bounds_get_offset( const AABB &box, v3 p ){
  v3 pmax = box.bound[ AABB::UPPER ];
  v3 pmin = box.bound[ AABB::LOWER ];
  
  v3 off = p - pmin;
  off.X = ( pmax.X > pmin.X )?(off.X/(pmax.X - pmin.X) ):off.X;
  off.Y = ( pmax.Y > pmin.Y )?(off.Y/(pmax.Y - pmin.Y) ):off.Y;
  off.Z = ( pmax.Z > pmin.Z )?(off.Z/(pmax.Z - pmin.Z) ):off.Z;
  return off;
}

BVHNode *HLBVHBuild( void *mem, 
    PrimitiveInfo *info, 
    int *total_nodes,
    Primitive *ordered_prims )
{

  // Calculate bounding box that contains all the primitives
  // Computer Morton indices
  // Sort the morton indices
  // Create LBVH treelets at the bottom of BVH
  // Create and return SAH BVH from LBVH treelets
  

  // Calculate bounding box that contains all the primitives' centroid
  AABB bounds; 
  for ( int i = 0; i < len( info ); i++ ){
    bounds = Union( bounds, info[i].centroid ); 
  }

  int morton_bits = 10;
  int scale = 1 << morton_bits;
  MortonPrimitive morton_prims[100];
  for ( int i = 0 ; i < len( info ); i++ ){
    // Calculate the relative offset of centroid from the bounding box
    // lower bound
    morton_prims[i].prim = info[i].prim;
    v3 off = bounds_get_offset( bounds, info[i].centroid );
    morton_prims[i].code = encode_morton3( off * scale );
  }
}
#endif
struct PrimitiveInfo;

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
  PrimitiveInfo *pi = ( PrimitiveInfo *)x;
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
  BVHNode *node = arena_alloc( arena, sizeof( BVHNode ));
  node->first_offset = first;
  node->num_prim = n;
  node->box = bound;
  return node;
}

BVHNode *bvh_create_interior( Arena *arena, BVHNode *l, BVHNode *r, int axis ){
  BVHNode *node = arena_alloc( arena, sizeof( BVHNode ));
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
    PrimitiveInfo *info,
    int start,
    int end
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
    int l = ordered_prims.size(); 
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
#if 0
  if ( mid == start || mid == end ){
    return bvh_n_build( ):
  }
#endif
  BVHNode *l = bvh_recursive_build( arena, info, start, mid );
  BVHNode *r = bvh_recursive_build( arena, info, mid, end);
  return bvh_create_interior( arena, l, r, dim ); 
}
    

#if 0
BVHNode *create_bvh_tree( const World &w ){
  std::vector<Primitive> prim;
  for ( int i = 0; i < w.sph_count; i++ ){
    prim.push_back(
        PrimitiveInfo( PrimitiveInfo::SPHERE, (void *)( w.spheres+i ) )
    );
  }
#if 0
  for ( int i = 0; i < w.sph_count; i++ ){
    prim.push_back( { Primitive::PLANE, w.planes+ i } );  
  }
#endif

  return bvh_recursive_build( bvh_arena, &prim[0], prim.size() );
}
#endif
