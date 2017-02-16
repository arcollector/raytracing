#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <math.h>

#include "helpers.h"
#include "rgb.h"

typedef struct {
  double x,y,z,w;
} Vector;

#include "matrix.h"

#define VECTOR_ZERO Vector_New(0,0,0)
#define VECTOR_NAN Vector_New(NAN,NAN,NAN);

Vector Vector_New(double x, double y, double z);
Vector Vector2d_New(double x, double y);
Vector Vector4d_New(double x, double y, double z, double w);

Vector Vector_FromP1toP2(Vector p1,Vector p2);

Vector Vector_FromRGB(RGB color);

RGB Vector_ToRGB(Vector color);

Vector Vector_AddVector(Vector v1,Vector v2);

Vector Vector_AddScalar(Vector v,double scalar);

Vector Vector_SubVector(Vector v1, Vector v2);

Vector Vector_Negate(Vector v);

double Vector_Dot(Vector v1, Vector v2);

Vector Vector_Cross(Vector v1, Vector v2);

Vector Vector_MulVector(Vector v1, Vector v2);

Vector Vector_MulScalar(Vector v, double scalar);

Vector Vector_MulMatrix(Vector v, Matrix m);

Vector Vector_MulMatrixNotTranslation(Vector v, Matrix m);

Vector Vector_DivVector(Vector v1, Vector v2);

Vector Vector_DivScalar(Vector v, double scalar);

double Vector_Length(Vector v);

Vector Vector_Normalize(Vector v);

Vector Vector_Reflect(Vector incident, Vector normal);

Vector Vector_Refract(
  Vector incident,
  Vector normal,
  double nI,
  double nR,
  int *isTIR
);

void Vector_Print(Vector v);

#endif
