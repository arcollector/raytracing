#ifndef INTERSECT_H
#define INTERSECT_H

#include "helpers.h"
#include "vector.h"
#include "ray.h"
#include "bbox.h"
#include "object.h"
#include "hit.h"
#include "texture.h"
#include "clip.h"

extern int gbDebug;

Hit *Intersect(
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
  double *attenuate,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
);

#endif
