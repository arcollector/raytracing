#ifndef OBJECT_H
#define OBJECT_H

#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "texture.h"
#include "hit.h"
#include "bbox.h"
#include "clip.h"

enum {
  OBJ_SPHERE = 1,
  OBJ_PLANE,
  OBJ_POLYGON
} ObjectTypes;

typedef struct BBOXStruct BBOX;

typedef struct ObjectStruct {
  void *primitive;
  int type;
  struct ObjectStruct *next;
  Hit *(*intersect)(Ray ray, void *primitive);
  Vector (*normal)(Vector point, void *primitive);
  Texture *texture;
  void (*print)(void *primitive);
  void (*free)(void *primitive);
  void (*bbox)(BBOX *bbox);
  ClipList *clipList;
  long clipListLength;
} Object;

#endif
