#ifndef MATERIALS_H
#define MATERIALS_H
#include "common.h"
#include "HandmadeMath.h"
enum MaterialType {
  MATERIAL_PURE_DIFFUSE,
  MATERIAL_METALLIC,
  MATERIAL_GLASS,
  MATERIAL_PARTAIL_METALLIC,
  MATERIAL_LIGHT
};

struct Material {
  MaterialType type;
  union {
    struct {
      f32 diff;
      f32 spec;
      f32 shine;
    };
    Light l;
  }
};
#endif
