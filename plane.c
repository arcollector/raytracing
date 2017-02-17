#include "plane.h"

Plane *Plane_New(Vector center, Vector normal, Texture *tex) {
  Plane *plane = malloc(sizeof(Plane));
  if(!plane) return NULL;

  plane->center = center;
  plane->normal = Vector_Normalize(normal);

  plane->tex = tex;

  return plane;
}

Hit *Plane_Intersect(Ray ray, void *_plane) {
  Plane *plane = _plane;

  double deno = Vector_Dot(plane->normal, ray.dir);
  // not intersection possible
  if(fabs(deno) < EPSILON) return NULL;

  double nume = Vector_Dot(
    plane->normal,
    Vector_SubVector(plane->center, ray.start)
  );

  double t = nume/deno;
  return Hit_New(1,t);
}

Vector Plane_Normal(Vector point, void *_plane) {
  Plane *plane = _plane;
  return plane->normal;
}

void Plane_Print(void *_plane) {
  Plane *plane = _plane;
  printf("===== PLANE =====\n");
  printf("normal is: "); Vector_Print(plane->normal);
  printf("center is: "); Vector_Print(plane->center);
  Texture_Print(plane->tex);
}

void Plane_Free(void *_plane) {
  Plane *plane = _plane;
  Texture_Free(plane->tex);
  free(plane);
}
