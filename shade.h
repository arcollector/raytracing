#ifndef SHADE_H
#define SHADE_H

#include "datatypes.h"
#include "ray.h"
#include "vector.h"
#include "camera.h"
#include "rgb.h"
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
