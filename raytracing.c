#include <stdio.h>

#define TT_IMPLEMENTATION
#include "3rdparty/tinytime.h"

#include "camera.h"
#include "bmp.h"
#include "scene.h"
#include "shoot.h"
#include "bbox.h"

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

  fclose(fp);
  Scene_Print(scene);

  long width = scene->width;
  long height = scene->height;

  // resulted image
  BMP_Canvas canvas;
  if(!BMP_NewCanvas(&canvas,width,height)) {
    Scene_Free(scene);
    return 0;
  }

  Object *unboundedObjectList = NULL;
  BBOXTree *root = NULL;
  long treeObjectLength, unboundedObjectListLength;
  #if 1
  ttTime();
  root = BBOXTree_New(
    scene->objectList,
    &treeObjectLength,
    &unboundedObjectList,
    &unboundedObjectListLength
  );
  printf("hierarchy bbox tree builded in %f seconds\n",ttTime());
  BBOXTree_InitStack(treeObjectLength);
  #else
  // if 0 to raytracing without hierarchy bbox tree (for benchmarking use)
  unboundedObjectList = scene->objectList;
  #endif

  Camera_PrepareForShooting(width,height,&scene->cam);

  printf("--- RAY TRACING GO! ---\n");
  ttTime();
  for(long y = 0; y < height; y++) {
    for(long x = 0; x < width; x++) {
      //printf("(%ld %ld)\n",x,y);
      RGB pixel = Shoot(
        x,y,
        scene,
        root,treeObjectLength,
        unboundedObjectList,unboundedObjectListLength
      );
      BMP_PushRGB(&canvas,pixel);
    }
  }
  printf("raytracing elaped time was: %f seconds\n",ttTime());

  BMP_Save(&canvas,scene->fileName);
  BMP_Free(&canvas);

  BBOXTree_Free(root);
  BBOXTree_FreeStack();

  Scene_Free(scene);

  return 0;
}
