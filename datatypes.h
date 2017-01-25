#ifndef DATATYPES_H
#define DATATYPES_H

#include "rgb.h"

// ----------------------------------------

typedef struct {
  double x,y,z,w;
} Vector;

// ----------------------------------------

typedef struct {
  Vector color[16];
  double limit[16];
  long length;
  double minRadius, maxRadius; // used in spherical texturing
} Texture;

// ----------------------------------------

typedef struct {
  double _00,_01,_02,_03;
  double _10,_11,_12,_13;
  double _20,_21,_22,_23;
  double _30,_31,_32,_33;
} Matrix;

// ----------------------------------------

typedef struct {
  Vector pos;
  Vector up,left,viewDir;
  Matrix local, invLocal;
  Vector viewerPos;
  Vector min, max;
  Matrix win;
} Camera;

// ----------------------------------------

typedef struct {
  Vector start, dir;
} Ray;

// ----------------------------------------

typedef struct {
  Vector center;
  double radius;
  double rSquared;
  double invRadius;
  Texture *tex;
  Matrix local,invLocal;
} Sphere;

// ----------------------------------------

typedef struct {
  Vector center;
  Vector normal;
  Texture *tex;
} Plane;

// ----------------------------------------

#define OBJ_SPHERE 1
#define OBJ_PLANE 2

typedef struct ObjectStruct {
  void *primitive;
  int type;
  struct ObjectStruct *next;
  double (*intersect)(Ray ray, void *primitive);
  RGB (*getColor)(Vector p, Camera cam, void *primitive);
  void (*print)(void *primitive);
  void (*free)(void *primitive);
} Object;

// ----------------------------------------

#define BBOX_AXES_COUNT 3

typedef struct BBOXStruct {
  Object *obj;
  int isUnbounded;
  struct BBOXStruct *next;
  Vector min[BBOX_AXES_COUNT], max[BBOX_AXES_COUNT], centroid[BBOX_AXES_COUNT];
} BBOX;

typedef struct BBOXTreeStruct {
  struct BBOXTreeStruct *left, *right;
  BBOX bbox;
  Object *objectList;
  long objectListLength;
} BBOXTree;

// ----------------------------------------

typedef struct {
  char *fileName;
  long width, height;
  Camera cam;
  Object *objList;
  long objectsTotal;
  Texture *sky;
  int aa;
} Scene;

// ----------------------------------------

#endif
