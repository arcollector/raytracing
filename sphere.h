#ifndef SPHERE_H
#define SPHERE_H

#include <stdlib.h>

#include "helpers.h"
#include "datatypes.h"
#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "camera.h"
#include "texture.h"

Sphere *Sphere_New(Vector center, double radius, Texture *tex, Camera cam);
double Sphere_Intersect(Ray ray, void *sphere);
RGB Sphere_GetColor(Vector p, Camera cam, void *sphere);
Vector Sphere_Normal(Vector point, Sphere sphere);
void Sphere_Print(void *sphere);
void Sphere_Free(void *sphere);

#endif
