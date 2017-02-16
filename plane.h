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
#include "texture.h"
#include "hit.h"

typedef struct {
  Vector center;
  Vector normal;
  Texture *tex;
} Plane;

Plane *Plane_New(Vector center, Vector normal, Texture *tex);
Hit *Plane_Intersect(Ray ray, void *plane);
Vector Plane_Normal(Vector point, void *plane);
void Plane_Print(void *plane);
void Plane_Free(void *plane);

#endif
