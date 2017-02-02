#ifndef SPHERE_H
#define SPHERE_H

#include <stdlib.h>

#include "helpers.h"
#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "camera.h"
#include "texture.h"

extern int gbDebug;

typedef struct {
  Vector center;
  double radius;
  double rSquared;
  double invRadius;
  Texture *tex;
  Matrix local,invLocal;
} Sphere;

Sphere *Sphere_New(Vector center, double radius, Texture *tex, Camera cam);
double Sphere_Intersect(Ray ray, void *sphere);
Vector Sphere_GetColor(
  Ray ray,
  Vector p,
  Vector normal,
  Camera cam,
  void *sphere
);
Vector Sphere_Normal(Vector point, Sphere sphere);
void Sphere_Print(void *sphere);
void Sphere_Free(void *sphere);

#endif
