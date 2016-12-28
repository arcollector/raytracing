#ifndef PLANE_H
#define PLANE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"
#include "datatypes.h"
#include "rgb.h"
#include "vector.h"
#include "ray.h"
#include "camera.h"

Plane *Plane_New(Vector center, Vector normal, RGB color, Camera cam);
double Plane_Intersect(Ray ray, void *plane);
RGB Plane_GetColor(void *plane);
void Plane_Print(void *plane);
void Plane_Free(void *plane);

#endif
