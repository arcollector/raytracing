#ifndef SHOOT_H
#define SHOOT_H

#include <stdio.h>

#include "helpers.h"
#include "datatypes.h"
#include "rgb.h"
#include "ray.h"
#include "shade.h"
#include "quadtree.h"
#include "scene.h"

#define SRANDOM(min,max) (rand()/(double)RAND_MAX*((min)+(max))+(min)-.125)

RGB Shoot(long x, long y, Scene *scene);
Ray Shoot_BuildRay(double x, double y, Camera cam);

RGB Shoot_Single(
  double x, double y,
  Camera cam,
  Object *objList,
  RGB bkgColor
);

void Shoot_Multi(
  double x, double y, double length,
  Quadtree *n, int level,
  int isStochastic,
  Camera cam,
  Object *objList,
  RGB bkgColor
);

#endif
