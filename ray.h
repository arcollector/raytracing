#ifndef RAY_H
#define RAY_H

#include "helpers.h"
#include "vector.h"
#include "matrix.h"

extern int gbDebug;

typedef struct {
  Vector start, dir;
} Ray;

Ray Ray_New(Vector start, Vector dir);
Ray Ray_FromP1toP2(Vector p1, Vector p2);
Ray Ray_Normalize(Ray ray);
double Ray_Length(Ray ray);
Ray Ray_Transform(Ray ray, Matrix m);
Vector Ray_PointAt(Ray ray, double t);
void Ray_Print(Ray ray);

#endif
