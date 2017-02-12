#ifndef INTERSECT_H
#define INTERSECT_H

#include "rgb.h"
#include "vector.h"
#include "ray.h"
#include "bbox.h"

extern int gbDebug;

Object *Intersect(
  Ray ray,
  double *tValue,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
);

int Intersect_Shadow(
  Ray ray,
  double dist,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
);

#endif
