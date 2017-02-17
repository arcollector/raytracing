#ifndef SPHERE_H
#define SPHERE_H

#include <stdlib.h>

#include "helpers.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "texture.h"
#include "hit.h"
#include "bbox.h"

extern int gbDebug;

typedef struct {
  Vector center;
  double radius;
  double rSquared;
  double invRadius;
  Texture *tex;
  Matrix local, invLocal;
} Sphere;

Sphere *Sphere_New(Vector center, double radius, Texture *tex);

Hit *Sphere_Intersect(Ray ray, void *sphere);
Vector Sphere_Normal(Vector point, void *sphere);
void Sphere_Print(void *sphere);
void Sphere_Free(void *sphere);

void Sphere_BBOX(BBOX *bbox);

#endif
