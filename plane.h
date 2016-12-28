#ifndef PLANE_H
#define PLANE_H

#include <stdio.h>
#include <math.h>

#include "helpers.h"
#include "datatypes.h"
#include "rgb.h"
#include "vector.h"
#include "ray.h"
#include "camera.h"

Plane Plane_New(Vector center, Vector normal, RGB color, Camera cam);
double Plane_Intersect(Ray ray, Plane plane);
void Plane_Print(Plane plane);

#endif
