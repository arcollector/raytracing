#ifndef SHOOT_H
#define SHOOT_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

RGB Shoot(
  long x, long y,
  Scene *scene,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength
);

#endif
