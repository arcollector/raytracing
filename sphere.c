#include "sphere.h"

Sphere *Sphere_New(Vector center, double radius, Texture *tex) {
  Sphere *sphere = malloc(sizeof(Sphere));
  if(!sphere) return NULL;

  sphere->center = center;
  sphere->radius = radius;
  sphere->rSquared = radius*radius;
  sphere->invRadius = 1/radius;
  sphere->tex = tex;

  Matrix t1 = Matrix_New();
  t1._30 = -center.x;
  t1._31 = -center.y;
  t1._32 = -center.z;
  sphere->local = t1;

  t1._30 = center.x;
  t1._31 = center.y;
  t1._32 = center.z;
  sphere->invLocal = t1;

  return sphere;
}

Hit *Sphere_Intersect(Ray ray, void *_sphere) {
  Sphere *sphere = _sphere;

  // ray in world space to sphere space
  ray = Ray_Transform(ray, sphere->local);

  double b = Vector_Dot(ray.start, ray.dir);
  double c = Vector_Dot(ray.start, ray.start) - sphere->rSquared;

  double disc = b*b - c;
  // not intersection possible
  if(disc < 0) return NULL;

  double term = sqrt(disc);
  double t1 = -b - term;
  double t2 = -b + term;

  return Hit_New(2,t1,t2);
}

Vector Sphere_Normal(Vector point, void *_sphere) {
  Sphere *sphere = _sphere;
  Vector normal = Vector_FromP1toP2(sphere->center, point);
  normal = Vector_MulScalar(normal, sphere->invRadius);
  return normal;
}

void Sphere_Print(void *_sphere) {
  Sphere *sphere = _sphere;
  printf("==== SPHERE ====\n");
  printf("center is "); Vector_Print(sphere->center);
  printf("radius is %f\n", sphere->radius);
  Texture_Print(sphere->tex);
}

void Sphere_Free(void *_sphere) {
  Sphere *sphere = _sphere;
  Texture_Free(sphere->tex);
  free(sphere);
}

void Sphere_BBOX(BBOX *bbox) {
  Sphere *sphere = bbox->obj->primitive;
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    Vector axis = BBOX_GetAxis(i);
    double length = Vector_Dot(axis, sphere->center);
    Vector min = Vector_MulScalar(axis, length - sphere->radius);
    Vector max = Vector_MulScalar(axis, length + sphere->radius);
    bbox->min[i] = min;
    bbox->max[i] = max;
  }
}
