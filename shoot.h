#ifndef SHOOT_H
#define SHOOT_H

#include <stdio.h>

#include "helpers.h"
#include "rgb.h"
#include "vector.h"
#include "ray.h"
#include "camera.h"
#include "object.h"
#include "shade.h"
#include "quadtree.h"
#include "scene.h"
#include "bbox.h"
#include "texture.h"

#define JITTER(min,max) (rand()/(double)RAND_MAX*((min)+(max))+(min)-.125)

RGB Shoot(
  long x, long y,
  Scene *scene,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength
);

Ray Shoot_BuildRay(double x, double y, Camera cam);

RGB Shoot_Single(
  double x, double y,
  Camera cam,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength,
  Texture *sky
);

void Shoot_Multi(
  double x, double y, double length,
  Quadtree *n, int level,
  int isStochastic,
  Camera cam,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength,
  Texture *sky
);

#endif
