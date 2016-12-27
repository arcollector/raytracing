#ifndef PLANE_H
#define PLANE_H

#include <stdio.h>

#include "rbg.h"
#include "vector.h"
#include "ray.h"
#include "camera.h"

struct PlaneStruct {
  Vector center;
  Vector normal;
  RGB color;
};

typedef struct PlaneStruct Plane;

Plane Plane_New(Vector center, Vector normal, RGB color, Camera cam);
double Plane_Intersect(Ray ray, Plane plane);
void Plane_Print(Plane plane);

#endif
