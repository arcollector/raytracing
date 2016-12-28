#include "plane.h"

Plane Plane_New(Vector center, Vector normal, RGB color, Camera cam) {

  Plane plane;
  plane.center = Vector_MulMatrix(center, cam.local);
  normal = Vector_MulMatrixNotTranslation(normal, cam.local);
  plane.normal = Vector_Normalize(normal);
  plane.color = color;

  return plane;
}

double Plane_Intersect(Ray ray, void *_plane) {
  Plane *plane = _plane;
  
  double deno = Vector_Dot(plane->normal, ray.dir);
  if(fabs(deno) < EPSILON) { // not intersection possible
    return -1;
  }

  double nume = Vector_Dot(plane->normal,
                     Vector_SubVector(plane->center, ray.start));
  
  double t = nume/deno;
  if(fabs(t) < EPSILON) {
    return -1;
  }

  return t;
}

RGB Plane_GetColor(void *_plane) {
  Plane *plane = _plane;
  return plane->color;
}

void Plane_Print(Plane plane) {
  printf("plane normal is: "); Vector_Print(plane.normal);
  printf("plane center is: "); Vector_Print(plane.center);
  printf("plane color is: %d %d %d\n",
                     plane.color.red,plane.color.green,plane.color.blue);  
}

