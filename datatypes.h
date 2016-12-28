#ifndef DATATYPES_H
#define DATATYPES_H

#include "rgb.h"

// ----------------------------------------

struct VectorStruct {
  double x,y,z,w;
};

typedef struct VectorStruct Vector;

// ----------------------------------------

struct MatrixStruct {
  double _00,_01,_02,_03;
  double _10,_11,_12,_13;
  double _20,_21,_22,_23;
  double _30,_31,_32,_33;
};

typedef struct MatrixStruct Matrix;

// ----------------------------------------

struct RayStruct {
  Vector start, end, dir;
};

typedef struct RayStruct Ray;

// ----------------------------------------

struct ObjectStruct {
  void *obj;
  struct ObjectStruct *next;
  double (*intersect)(Ray ray, void *obj);
  RGB (*getColor)(void *obj);
};

typedef struct ObjectStruct Object;

// ----------------------------------------

struct SphereStruct {
  Vector center;
  double radius;
  double rSquared;
  double invRadius; 
  RGB color;
  Matrix local,invLocal;
};

typedef struct SphereStruct Sphere;

// ----------------------------------------

struct PlaneStruct {
  Vector center;
  Vector normal;
  RGB color;
};

typedef struct PlaneStruct Plane;

// ----------------------------------------



#endif

