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

int mid_point_filter( void *x, void *ex_args ){
  ObjectBVHInfo *pi = ( ObjectBVHInfo *)x;
  ObjectBVHInfoExArgs *args = (ObjectBVHInfoExArgs *)ex_args;
  return pi->centroid[ args->dim ] < args->pmid;
}

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
    ObjectBVHInfo *info,
    int start,
    int end,
    ObjectBVHInfo *ordered_prims )
{

  AABB total_bound;
  for ( int i = start; i < end; i++ ){
    total_bound = AABB_union( total_bound, info[i].box );
  }
  int len = end - start;
  if ( len == 1 ){
    // only a single primitive
    BVHNode *n = bvh_create_leaf( arena,
                                  array_length( ordered_prims ),
                                  1,total_bound );
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
    BVHNode *n = bvh_create_leaf( arena,
                                  array_length( ordered_prims ),
                                  len,total_bound );
    for ( int i = start; i < end; i++ ){
      ordered_prims.push_back( info[i] );
    }
    return n;
  }

  float pmid = 0.5f * ( bounds.l[dim] + bounds.u[dim] );
  ObjectBVHInfoExArgs args = { dim, pmid };

  int mid = start + partition(
              (void *)(info + start),
              len,
              sizeof(*info),
              mid_point_filter,
              &args );
  if ( mid == start || mid == end ){
    // We were not able to find a good partition
    BVHNode *n = bvh_create_leaf( arena,
                                  array_length( ordered_prims ),
                                  len,total_bound );
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
    ObjectBVHInfo *world_objects, // input 
    ObjectBVHInfo *ordered_prims) // output
{
  return bvh_recursive_build( arena,
      &world_objects[0], 0,
      array_length( world_objects ),
      ordered_prims );
}

bool bvh_leaf_hit( 
    BVHNode *node,
    const Ray &r,
    float tmin,
    float tmax,
    HitRecord &rec,
    ObjectBVHInfo *ordered_prims)
{
  bool hit_anything = false;
  HitRecord temp;
  for ( int i = 0;
        i < node->num_prim;
        i++ )
  {
    ObjectBVHInfo &p = ordered_prims[ node->first_offset+ i];
    switch ( p.type ){
      case ObjectType::OBJECT_GRID:
        if( hit_sphere( *( (Sphere*)p.data), r, tmin, tmax, temp) ){
          hit_anything = true;
          tmax = temp.t;
          rec = temp;
        }
        break;
      case ObjectType::OBJECT_CUBE:
        if( hit_plane( *( (Plane*)p.data), r, tmin, tmax, temp) ){
          hit_anything = true;
          tmax = temp.t;
          rec = temp;
        }
        break;
      case ObjectBVHInfo::RECTANGLE:
        if( hit_AARect( *( (AARect*)p.data), r, tmin, tmax, temp) ){
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
    ObjectBVHInfo *ordered_prims)
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
