#ifndef OBJECT_H
#define OBJECT_H

#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "texture.h"
#include "hit.h"

#define OBJ_SPHERE 1
#define OBJ_PLANE 2

typedef struct ObjectStruct {
  void *primitive;
  int type;
  struct ObjectStruct *next;
  Hit *(*intersect)(Ray ray, void *primitive);
  Vector (*normal)(Vector point, void *primitive);
  Texture *texture;
  void (*print)(void *primitive);
  void (*free)(void *primitive);
} Object;

#endif
