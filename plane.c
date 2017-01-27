#include "plane.h"

Plane *Plane_New(Vector center, Vector normal, Texture *tex, Camera cam) {
  Plane *plane = malloc(sizeof(Plane));
  if(!plane) return NULL;

  plane->center = Vector_MulMatrix(center, cam.local);
  normal = Vector_MulMatrixNotTranslation(normal, cam.local);
  plane->normal = Vector_Normalize(normal);
  plane->tex = tex;

  return plane;
}

double Plane_Intersect(Ray ray, void *_plane) {
  Plane *plane = _plane;

  double deno = Vector_Dot(plane->normal, ray.dir);
  if(fabs(deno) < EPSILON) { // not intersection possible
    return -1;
  }

  double nume = Vector_Dot(plane->normal,
    Vector_SubVector(plane->center, ray.start)
  );

  double t = nume/deno;
  if(fabs(t) < EPSILON) {
    return -1;
  }

  return t;
}

Vector Plane_GetColor(Vector p, Camera cam, void *_plane) {
  Plane *plane = _plane;

  p = Vector_MulMatrix(p,cam.invLocal); // to world

  return Texture_GetColor(p,plane->tex);
}

void Plane_Print(void *_plane) {
  Plane *plane = _plane;
  printf("plane normal is: "); Vector_Print(plane->normal);
  printf("plane center is: \n"); Vector_Print(plane->center);
  Texture_Print(plane->tex);
}

void Plane_Free(void *_plane) {
  Plane *plane = _plane;
  Texture_Free(plane->tex);
  free(plane);
}
