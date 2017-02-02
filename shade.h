#ifndef SHADE_H
#define SHADE_H

#include "rgb.h"
#include "vector.h"
#include "ray.h"
#include "texture.h"
#include "bbox.h"
#include "scene.h"

extern int gbDebug;

RGB Shade(
  Ray ray,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
);

Object *Intersect(
  Ray ray, 
  Object *objectList,
  double *lastT
);

Vector Shade_ComputeColor(
  Object *obj,
  Ray ray,
  double t, 
  Camera cam
);

#endif
