#ifndef SHADE_H
#define SHADE_H

#include "rgb.h"
#include "vector.h"
#include "ray.h"
#include "camera.h"
#include "texture.h"
#include "bbox.h"

RGB Shade(
  Ray ray,
  Camera cam,
  BBOXTree *root,
  Object *unboundObjList,
  Texture *sky
);

#endif
