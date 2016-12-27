#ifndef RAY_H
#define RAY_H

#include "helpers.h"

#include "vector.h"
#include "matrix.h"

struct RayStruct {
  Vector start, end, dir;
};

typedef struct RayStruct Ray;

Ray Ray_New(Vector p1, Vector p2);
Vector Ray_CalcDir(Ray ray);
Ray Ray_Transform(Ray ray, Matrix m);
void Ray_Print(Ray ray);

#endif
