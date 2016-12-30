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
#include "scene.h"

int main(int argc, char *argv[]) {

  if(argc != 2) {
    printf("missing scene file\n");
    return 0;
  }

  FILE *fp = fopen(argv[1],"rb");
  if(!fp) {
    printf("file %s does not exist\n",argv[1]);
    return 0;
  }

  Scene *scene = Scene_New();
  if(!scene) {
    fclose(fp);
    return 0;
  }

  if(!Scene_Setup(fp, scene)) {
    Scene_Free(scene);
    fclose(fp);
    return 0;
  }

  Scene_Print(scene);

  long width = scene->width;
  long height = scene->height;

  Object *objList = scene->objList;

  Camera cam = scene->cam;

  Matrix win = Camera_GetScreenToViewingPlaneMatrix(
                  width, height, cam
              );

  Vector rayStart = Camera_GetViewerPosition(cam);

  double lastT;
  RGB lastColor;

  // resulted image
  BMP_Canvas canvas;
  if(!BMP_NewCanvas(&canvas,width,height)) {
    Scene_Free(scene);
    fclose(fp);
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

      // store pixel
      BMP_PushRGB(&canvas,lastColor);

    }
  }

  BMP_Save(&canvas,scene->fileName);
  BMP_Free(&canvas);

  Scene_Free(scene);
  fclose(fp);

  return 0;
}
