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
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Texture *sky
);

Object *Intersect(
  Ray ray, 
  Object *objectList,
  double *lastT
);

RGB Shade_ComputeColor(
  Object *obj,
  Ray ray,
  double t, 
  Camera cam
);

#endif
