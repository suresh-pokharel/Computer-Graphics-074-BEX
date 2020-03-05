#include "ui_primitives.h"

struct ObjectBVHInfo{
  Object object;
  AABB box;
  v3 centroid;

  ObjectBVHInfo ( PrimType t, void *p, AABB b ){
    type = t;
    box = b;
    centroid = 0.5f * (box.l + box.u);
    data = p;
  }
};

struct ObjectBVHInfoExArgs{
  int dim;
  float pmid;
};

struct BVHNode{
  AABB box;
  BVHNode *left, *right; // NULL for leaf primitives
  int split_axis;
  int first_offset, num_prim; // only valid for leaf nodes
};
