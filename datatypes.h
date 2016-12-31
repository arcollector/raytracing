#ifndef DATATYPES_H
#define DATATYPES_H

#include "rgb.h"

// ----------------------------------------

typedef struct {
  double x,y,z,w;
} Vector;

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
  Vector up,left,view;
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
  RGB color;
  Matrix local,invLocal;
} Sphere;

// ----------------------------------------

typedef struct {
  Vector center;
  Vector normal;
  RGB color;
} Plane;

// ----------------------------------------

typedef struct ObjectStruct {
  void *obj;
  int type;
  struct ObjectStruct *next;
  double (*intersect)(Ray ray, void *obj);
  RGB (*getColor)(void *obj);
  void (*print)(void *obj);
  void (*free)(void *obj);
} Object;

// ----------------------------------------

typedef struct {
  char *fileName;
  long width, height;
  Camera cam;
  Object *objList;
  long objectsTotal;
  RGB bkgColor;
} Scene;

// ----------------------------------------

#endif
