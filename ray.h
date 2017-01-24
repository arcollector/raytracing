#ifndef RAY_H
#define RAY_H

#include "helpers.h"
#include "datatypes.h"
#include "vector.h"
#include "matrix.h"

Ray Ray_New(Vector p1, Vector p2);
Ray Ray_Transform(Ray ray, Matrix m);
Vector Ray_PointAt(Ray ray, double t);
void Ray_Print(Ray ray);

#endif
