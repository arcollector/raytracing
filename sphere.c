#include "sphere.h"

Sphere Sphere_New(Vector center, double radius, RGB color, Camera cam) {

  center = Vector_MulMatrix(center, cam.local);

  Sphere sphere;
  sphere.center = center;
  sphere.radius = radius;
  sphere.rSquared = radius*radius;
  sphere.invRadius = 1/radius;
  sphere.color = color;
  
  Matrix t1 = Matrix_New();
  t1._30 = -center.x;
  t1._31 = -center.y;
  t1._32 = -center.z;
  sphere.local = t1;

  t1._30 = center.x;
  t1._31 = center.y;
  t1._32 = center.z;
  sphere.invLocal = t1;
  
  return sphere;
}

double Sphere_Intersect(Ray ray, void *_sphere) {
  Sphere *sphere = _sphere;

  ray = Ray_Transform(ray, sphere->local);

  double b = Vector_Dot(ray.start,ray.dir);
  double c = Vector_Dot(ray.start,ray.start) - sphere->rSquared;

  double dis = b*b - c;
  // not intersection possible
  if(fabs(dis) < EPSILON) {
    return -1;
  }

  double term = sqrt(dis);
  double t1 = -b - term;
  double t2 = -b + term;

  // point behind start point
  if(t1 < 0 && t2 < 0) {
    return -1;
  }
  // ray start inside sphere
  if(t1 < 0) {
    return t2;
  }
  return t1;
}

Vector Sphere_Normal(Vector point, Sphere sphere) {
  Vector normal = Vector_FromP1toP2(sphere.center, point);
  normal = Vector_MulScalar(normal, sphere.invRadius);
  return normal;
}

RGB Sphere_GetColor(void *_sphere) {
  Sphere *sphere = _sphere;
  return sphere->color;
}

void Sphere_Print(Sphere sphere) {
  printf("sphere center: (%5.5f %5.5f %5.5f)",
                sphere.center.x,sphere.center.y,sphere.center.z);
  printf(" radius: %5.5f", sphere.radius);
  printf(" color: %d %d %d\n", 
                sphere.color.red,sphere.color.green,sphere.color.blue);
}
