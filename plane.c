#include "plane.h"

Plane Plane_New(Vector center, Vector normal, RGB color, Camera cam) {

  Plane plane;
  plane.center = Vector_MulMatrix(center,cam.m);
  plane.normal = Vector_Normalize(normal);
  plane.color = color;

  return plane;
}

