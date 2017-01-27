#ifndef PLANE_H
#define PLANE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"
#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "camera.h"
#include "texture.h"

typedef struct {
  Vector center;
  Vector normal;
  Texture *tex;
} Plane;

Plane *Plane_New(Vector center, Vector normal, Texture *tex, Camera cam);
double Plane_Intersect(Ray ray, void *plane);
Vector Plane_GetColor(Vector p, Camera cam, void *plane);
void Plane_Print(void *plane);
void Plane_Free(void *plane);

#endif
