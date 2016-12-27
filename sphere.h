#ifndef SPHERE_H
#define SPHERE_H

#include "helpers.h"

#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "camera.h"

struct SphereStruct {
  Vector center;
  double radius;
  double rSquared;
  double invRadius; 
  RGB color;
  Matrix local,invLocal;
};

typedef struct SphereStruct Sphere;

Sphere Sphere_New(Vector center, double radius, RGB color, Camera cam);
double Sphere_Intersect(Ray ray, Sphere sphere);
Vector Sphere_Normal(Vector point, Sphere sphere);
void Sphere_Print(Sphere sphere);

#endif
