#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"
#include "datatypes.h"
#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "sphere.h"
#include "windowing.h"
#include "camera.h"
#include "bmp.h"
#include "tga.h"
#include "plane.h"

int main(int argc, char *argv) {

  long height = 200;
  long width = 200;

  Matrix win = Windowing_New(
      Vector_New(0,0,0),Vector_New(width,height,0),
      Vector_New(-10,-10,0),Vector_New(10,10,0),
      1
  );

  Camera cam = Camera_New(
    Vector_New(0,0,-10), // pos
    Vector_New(0,1,-10), // up reference point
    Vector_New(0,0,0) // view reference point
  );
  Camera_Print(cam);

  Vector rayStart = Vector_New(0,0,-10);

  Sphere spheres[10];
  long spheresLength = 0;
  spheres[0] = Sphere_New(Vector_New(-6,0,0),5,RGB_New(255,0,0),cam);
  Sphere_Print(spheres[0]);
  spheresLength++;
  spheres[1] = Sphere_New(Vector_New(6,0,0),5,RGB_New(0,0,255),cam);
  Sphere_Print(spheres[1]);
  spheresLength++;
  Plane planes[10];
  long planesLength = 0;
  planes[0] = Plane_New(Vector_New(0,-10,0),Vector_New(0,1,0),RGB_New(255,255,0),cam);
  Plane_Print(planes[0]);
  planesLength++;

  Object *objList, *nextObj;
  objList = malloc(sizeof(Object)); 
  nextObj = objList;
  nextObj->obj = &spheres[0];
  nextObj->intersect = &Sphere_Intersect;
  nextObj->getColor = &Sphere_GetColor;
  nextObj->next = NULL;
  nextObj = nextObj->next = malloc(sizeof(Object));
  nextObj->obj = &spheres[1];
  nextObj->intersect = &Sphere_Intersect;
  nextObj->getColor = &Sphere_GetColor;
  nextObj->next = NULL;
  nextObj = nextObj->next = malloc(sizeof(Object));
  nextObj->obj = &planes[0];
  nextObj->intersect = &Plane_Intersect;
  nextObj->getColor = &Plane_GetColor;
  nextObj->next = NULL;

  double lastT;
  RGB lastColor; 

  // resulted image
  BMP_Canvas canvas;
  if(!BMP_NewCanvas(&canvas,width,height)) {
    return 0;
  }

  RGB bkgColor = RGB_New(54,241,54);

  for(long y = 0; y < height; y++) {
    for(long x = 0; x < width; x++) {
      
      // windowing
      Vector ps = Vector_New(x+.5,y+.5,0); // from screen space
      Vector pvp = Vector_MulMatrix(ps,win); // to view plane space
      //Vector_Print(ps); Vector_Print(pvp); printf("\n");

      // build ray
      Ray ray = Ray_New(rayStart,pvp);
    
      // intersect
      lastT = 1e10;
      lastColor = bkgColor;
      for(Object *node = objList; node; node = node->next) {
        double t = (*node->intersect)(ray,node->obj);
        if(t > 0 && t < lastT) {
          lastT = t;
          lastColor = (*node->getColor)(node->obj);
        }
      }
      /*
      for(long i = 0; i < spheresLength; i++) {
        Sphere sphere = spheres[i];
        double t = Sphere_Intersect(ray,sphere);
        if(t > 0 && t < lastT) {
          lastT = t;
          lastColor = sphere.color;
        }
      }
      for(long i = 0; i < planesLength; i++) {
        Plane plane = planes[i];
        double t = Plane_Intersect(ray,plane);
        if(t > 0 && t < lastT) {
          lastT = t;
          lastColor = plane.color;
        }
      }
      */
      // store pixel
      BMP_PushRGB(&canvas,lastColor);     

    }
  }

  for(Object *node = objList, *tmp; node; node = tmp) {
    tmp = node->next;
    free(node);
  }

  BMP_Save(&canvas,"scene.bmp");
  BMP_Free(&canvas);

  return 0;
}

