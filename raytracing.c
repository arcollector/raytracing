#include <stdio.h>
#include <math.h>

#include "helpers.h"
#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "sphere.h"
#include "windowing.h"
#include "camera.h"
#include "bmp.h"
#include "tga.h"

int main(int argc, char *argv) {

  long height = 200;
  long width = 200;

  Matrix win = Windowing_New(
      Vector_New(0,0,0),Vector_New(width,height,0),
      Vector_New(-10,-10,0),Vector_New(10,10,0),
      1
  );

  Camera cam = Camera_New(
    Vector_New(20,20,20), // pos
    Vector_New(20,21,20), // up reference point
    Vector_New(0,0,0) // view reference point
  );
  Camera_Print(cam);

  Vector rayStart = Vector_New(0,0,-10);

  Sphere spheres[10];
  long spheresLength = 0;
  spheres[0] = Sphere_New(Vector_New(0,0,0),5,RGB_New(255,0,0),cam);
  Sphere_Print(spheres[0]);
  spheresLength++;

  // this is a test!!

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
      for(long i = 0; i < spheresLength; i++) {
        Sphere sphere = spheres[i];
        double t = Sphere_Intersect(ray,sphere);
        if(t > 0 && t < lastT) {
          lastT = t;
          lastColor = sphere.color;
        }
      }
    
      // store pixel
      BMP_PushRGB(&canvas,lastColor);     

    }
  }

  BMP_Save(&canvas,"scene.bmp");
  BMP_Free(&canvas);

  return 0;
}

