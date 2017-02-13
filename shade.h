#ifndef SHADE_H
#define SHADE_H

#include <math.h>

#include "rgb.h"
#include "vector.h"
#include "ray.h"
#include "texture.h"
#include "lamp.h"
#include "bbox.h"
#include "scene.h"
#include "intersect.h"

extern int gbDebug;

Vector Shade(
  Ray ray,
  int level,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
);

#endif
