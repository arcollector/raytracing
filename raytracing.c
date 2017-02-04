#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define TT_IMPLEMENTATION
#include "3rdparty/tinytime.h"

#include "camera.h"
#include "bmp.h"
#include "scene.h"
#include "shoot.h"
#include "bbox.h"

int gbDebug = 0;

Scene *gbScene = NULL;
BBOXTree *gbRoot = NULL;
long gbTreeObjectLength = 0;
Object *gbUnboundedObjectList = NULL;
long gbUnboundedObjectListLength = 0;
BMP_Canvas gbCanvas;

typedef struct {
  long width, height;
  long yStart, yEnd;
} ThreadArg;

void *trace(void *arg) {
  printf("i am thread %ld\n",pthread_self());
  ThreadArg *threadArg = (ThreadArg *)arg;
  long yStart = threadArg->yStart;
  long yEnd = threadArg->yEnd;
  long width = threadArg->width;
  //printf("y %ld yEnd %ld x %ld xEnd %ld\n",y,yEnd,xStart,xEnd);
  for(long y = yStart; y < yEnd; y++) {
    for(long x = 0; x < width; x++) {
      //printf("(%ld %ld)\n",x,y);
      RGB pixel = Shoot(
        x,y,
        gbScene,
        gbRoot,gbTreeObjectLength,
        gbUnboundedObjectList,gbUnboundedObjectListLength
      );
      BMP_SetPixel(&gbCanvas,x,y,pixel);
    }
  }
  printf("end thread %ld\n",pthread_self());
  return NULL;
}

int main(int argc, char *argv[]) {

  if(argc < 2) {
    printf("missing scene file\n");
    return 0;
  }

  struct {
    char *sceneFile;
    long threads;
  } options = {
    .threads = 1
  };

  for(long i = 1; i < argc; i++) {
    char *arg = argv[i];
    if(strncmp(arg,"--threads=",8) == 0) {
      options.threads = atoi(strchr(arg,'=')+1);
      if(options.threads == 0) {
        printf("bad --threads= value\n");
        return 0;
      }
    } else {
      options.sceneFile = arg;
    }
  }

  FILE *fp = fopen(options.sceneFile,"rb");
  if(!fp) {
    printf("file %s does not exist\n",options.sceneFile);
    return 0;
  }

  if(!(gbScene = Scene_New())) {
    fclose(fp);
    return 0;
  }

  if(!Scene_Setup(fp, gbScene)) {
    Scene_Free(gbScene);
    fclose(fp);
    return 0;
  }

  fclose(fp);
  Scene_Print(gbScene);

  long width = gbScene->width;
  long height = gbScene->height;

  // resulted image
  if(!BMP_NewCanvas(&gbCanvas,width,height)) {
    Scene_Free(gbScene);
    return 0;
  }

  #if 1
  ttTime();
  gbRoot = BBOXTree_New(
    gbScene->objectList,
    &gbTreeObjectLength,
    &gbUnboundedObjectList,
    &gbUnboundedObjectListLength
  );
  printf("hierarchy bbox tree builded in %f seconds\n",ttTime());
  #else
  // if 0 to raytracing without hierarchy bbox tree (for benchmarking use)
  gbUnboundedObjectList = gbScene->objectList;
  gbUnboundedObjectListLength = gbScene->objectListLength;
  #endif

  Camera_PrepareForShooting(width,height,&gbScene->cam);

  printf("--- RAY TRACING GO! ---\n");
  ttTime();

  long threadsNum = options.threads;
  pthread_t *threads = calloc(threadsNum,sizeof(pthread_t));
  ThreadArg *threadsArg = calloc(threadsNum,sizeof(ThreadArg));
  long yOffset = 0;
  long yPadding = height / threadsNum;
  for(long i = 0; i < threadsNum; i++) {
    ThreadArg *arg = &threadsArg[i];
    arg->width = width;
    arg->height = height;
    arg->yStart = yOffset;
    arg->yEnd = yOffset + yPadding;
    if(i+1 == threadsNum && arg->yEnd < height) arg->yEnd = height;
    printf("thread %ld yStart %ld yEnd %ld\n",i,arg->yStart,arg->yEnd);
    yOffset += yPadding;
    if(pthread_create(&threads[i],NULL,trace,(void *)arg)) {
      printf("fail to create thread\n");
      threadsNum = 0;
      break;
    }
  }

  for(long i = 0; i < threadsNum; i++) {
    pthread_join(threads[i],NULL);
  }

  free(threads);
  free(threadsArg);

  printf("raytracing elaped time was: %f seconds\n",ttTime());

  BMP_Save(&gbCanvas,gbScene->fileName);
  BMP_Free(&gbCanvas);

  BBOXTree_Free(gbRoot);

  Scene_Free(gbScene);

  return 0;
}
