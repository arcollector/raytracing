#include <stdio.h>

#include "../rgb.h"
#include "../sphere.h"
#include "../ray.h"
#include "../camera.h"
#include "../bbox.h"
#include "../texture.h"

int main(int argc, char *argv[]) {

  Camera *cam = Camera_New(
    Vector_New(0,0,-1),
    Vector_New(0,1,-1),
    Vector_New(0,0,0),
    10,
    Vector_New(-10,-10,0),
    Vector_New(10,10,0)
  );
  Camera_Print(cam);

  Sphere *sphere = Sphere_New(Vector_New(0,0,0),5,Texture_FromRGB(RGB_New(255,0,0)));
  Sphere_Print(sphere);

  Ray ray;

  Object obj;
  obj.primitive = sphere;
  obj.intersect = Sphere_Intersect;
  obj.type = OBJ_SPHERE;
  obj.print = Sphere_Print;
  obj.free = Sphere_Free;
  obj.next = NULL;

  BBOX *bbox = BBOX_New(&obj);
  BBOX_Print(bbox);

  ray = Ray_FromP1toP2(Vector_New(0,0,-10),Vector_New(0,0,0));
  Ray_Print(ray);
  printf("intersection? %d\n", BBOX_Intersect(ray, bbox));
  printf("t value: %5.5f\n", (*obj.intersect)(ray, obj.primitive));

  ray = Ray_FromP1toP2(Vector_New(1,0,-10),Vector_New(1,0,0));
  Ray_Print(ray);
  printf("intersection? %d\n", BBOX_Intersect(ray, bbox));
  printf("t value: %5.5f\n", (*obj.intersect)(ray, obj.primitive));

  ray = Ray_FromP1toP2(Vector_New(0,10,-10),Vector_New(0,10,0));
  Ray_Print(ray);
  printf("intersection? %d\n", BBOX_Intersect(ray, bbox));
  printf("t value: %5.5f\n", (*obj.intersect)(ray, obj.primitive));

  return 0;
}
