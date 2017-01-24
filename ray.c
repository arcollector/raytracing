#include "ray.h"

Ray Ray_New(Vector p1, Vector p2) {
  Ray ray;
  ray.start = p1;
  ray.dir = Vector_Normalize(Vector_FromP1toP2(ray.start,p2));
  return ray;
}

Ray Ray_Transform(Ray ray, Matrix m) {
  ray.start = Vector_MulMatrix(ray.start, m);
  ray.dir = Vector_MulMatrixNotTranslation(ray.dir, m);
  return ray;
}

Vector Ray_PointAt(Ray ray, double t) {
  return Vector_AddVector(ray.start, Vector_MulScalar(ray.dir, t));
}

void Ray_Print(Ray ray) {
  printf("ray start: (%5.5f %5.5f %5.5f)",
                    ray.start.x,ray.start.y,ray.start.z);
  printf(" dir: (%5.5f %5.5f %5.5f)\n",
                    ray.dir.x,ray.dir.y,ray.dir.z);
}
