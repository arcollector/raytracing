#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../helpers.h"
#include "../datatypes.h"
#include "../vector.h"
#include "../bbox.h"
#include "../camera.h"
#include "../sphere.h"

int main(int argc, char *argv[]) {

  Camera cam = Camera_New(
    Vector_New(5,0,-7), Vector_New(5,1,-7), Vector_New(5,0,0),
    10, Vector_New(-10,-10,0), Vector_New(10,10,0)
  );

  Object *first, *objList;
  first = objList = malloc(sizeof(Object));
  objList->primitive = Sphere_New(Vector_New(1,0,0),1,RGB_New(255,0,0),cam);
  objList->type = OBJ_SPHERE;
  objList->print = Sphere_Print;

  objList = objList->next = malloc(sizeof(Object));
  objList->primitive = Sphere_New(Vector_New(3,0,0),1,RGB_New(0,255,0),cam);
  objList->type = OBJ_SPHERE;
  objList->print = Sphere_Print;

  objList = objList->next = malloc(sizeof(Object));
  objList->primitive = Sphere_New(Vector_New(7,0,0),1,RGB_New(0,0,255),cam);
  objList->type = OBJ_SPHERE;
  objList->print = Sphere_Print;

  objList = objList->next = malloc(sizeof(Object));
  objList->primitive = Sphere_New(Vector_New(1,0,4),1,RGB_New(255,255,0),cam);
  objList->type = OBJ_SPHERE;
  objList->print = Sphere_Print;

  objList = objList->next = malloc(sizeof(Object));
  objList->primitive = Sphere_New(Vector_New(3,0,4),1,RGB_New(255,255,255),cam);
  objList->type = OBJ_SPHERE;
  objList->print = Sphere_Print;

  objList = objList->next = malloc(sizeof(Object));
  objList->primitive = Sphere_New(Vector_New(1,0,-3),1,RGB_New(255,255,0),cam);
  objList->type = OBJ_SPHERE;
  objList->print = Sphere_Print;

  objList = objList->next = malloc(sizeof(Object));
  objList->primitive = Sphere_New(Vector_New(3,0,-3),1,RGB_New(255,255,255),cam);
  objList->type = OBJ_SPHERE;
  objList->print = Sphere_Print;

  objList = objList->next = malloc(sizeof(Object));
  objList->primitive = Sphere_New(Vector_New(7,0,-3),1,RGB_New(0,0,255),cam);
  objList->type = OBJ_SPHERE;
  objList->print = Sphere_Print;

  objList->next = NULL;

  Object *unboundObjList = NULL;

  BBOXTree *root = BBOXTree_New(first, &unboundObjList);
  if(unboundObjList) {
    printf("there are unbounded objects!\n");
  }
  BBOXTree_Print(root);
  BBOXTree_Free(root);

  for(Object *n = first, *t; n;) {
    Sphere_Free(n->primitive);
    t = n->next;
    free(n);
    n = t;
  }

  return 0;
}
