#ifndef SHOOT_H
#define SHOOT_H

#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
#include "rgb.h"
#include "vector.h"
#include "ray.h"
#include "camera.h"
#include "object.h"
#include "shade.h"
#include "scene.h"
#include "bbox.h"

extern int gbDebug;

typedef struct WindowStruct Window;

RGB Shoot(
  long x, long y,
  Scene *scene,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength
);

static Ray Shoot_BuildRay(double x, double y, Camera cam);

static RGB Shoot_Single(
  double x, double y,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength,
  Scene *scene
);

static RGB Shoot_Multi(
  double x, double y, int i, int j,
  Window *window, int upto,
  int isStochastic,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength,
  Scene *scene
);

#endif
