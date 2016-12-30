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
#include "shoot.h"
#include "shade.h"

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

  // resulted image
  BMP_Canvas canvas;
  if(!BMP_NewCanvas(&canvas,width,height)) {
    Scene_Free(scene);
    fclose(fp);
    return 0;
  }

  Camera_PrepareForShooting(width,height,&scene->cam);

  for(long y = 0; y < height; y++) {
    for(long x = 0; x < width; x++) {

      RGB pixel = Shoot(x,y,scene);

      BMP_PushRGB(&canvas,pixel);

    }
  }

  BMP_Save(&canvas,scene->fileName);
  BMP_Free(&canvas);

  Scene_Free(scene);
  fclose(fp);

  return 0;
}
