#ifndef OBJECT_H
#define OBJECT_H

#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "camera.h"

#define OBJ_SPHERE 1
#define OBJ_PLANE 2

typedef struct ObjectStruct {
  void *primitive;
  int type;
  struct ObjectStruct *next;
  double (*intersect)(Ray ray, void *primitive);
  Vector (*getColor)(Vector p, Camera cam, void *primitive);
  void (*print)(void *primitive);
  void (*free)(void *primitive);
} Object;

#endif
