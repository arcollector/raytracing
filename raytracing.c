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

static void *trace(void *arg);

#define DEBUG_PIXEL(x,y) (BMP_SetPixel(&gbCanvas,x,y,RGB_New(255,255,255)))

// **** GLOBALS ****

int gbDebug = 0;
Scene *gbScene = NULL;
BBOXTree *gbRoot = NULL;
long gbTreeObjectLength = 0;
Object *gbUnboundedObjectList = NULL;
long gbUnboundedObjectListLength = 0;
BMP_Canvas gbCanvas;

// **** END GLOBALS ****

// **** THREAD STUFF ****

typedef struct {
  long width, height;
  long yStart, yEnd;
} ThreadData;

static int Thread_Create(
  long width,
  long height,
  long threadsCount,
  pthread_t **threads,
  ThreadData **data
) {
  *threads = calloc(threadsCount,sizeof(pthread_t));
  *data = calloc(threadsCount,sizeof(ThreadData));
  long yOffset = 0;
  long yPadding = height / threadsCount;
  for(long i = 0; i < threadsCount; i++) {
    ThreadData *tmp = &(*data)[i];
    tmp->width = width;
    tmp->height = height;
    tmp->yStart = yOffset;
    tmp->yEnd = yOffset + yPadding;
    if(i+1 == threadsCount && tmp->yEnd < height) tmp->yEnd = height;
    printf("thread %ld yStart %ld yEnd %ld\n",i,tmp->yStart,tmp->yEnd);
    yOffset += yPadding;
    if(pthread_create(&(*threads)[i],NULL,trace,(void *)tmp)) {
      return 0;
    }
  }
  return 1;
}

static int Thread_Join(long threadsCount, pthread_t *threads) {
  for(long i = 0; i < threadsCount; i++) {
    if(pthread_join(threads[i],NULL)) {
      return 0;
    }
  }
  return 1;
}

static void Thread_Free(pthread_t *threads, ThreadData *data) {
  if(data) free(data);
  if(threads) free(threads);
}

// **** END THREAD STUFF ****

// **** THREAD CALLBACK FUNCTION ****

void *trace(void *arg) {
  printf("i am thread %ld\n",(long)pthread_self());
  ThreadData *data = (ThreadData *)arg;
  long yStart = data->yStart;
  long yEnd = data->yEnd;
  long width = data->width;
  //printf("y %ld yEnd %ld x %ld xEnd %ld\n",y,yEnd,xStart,xEnd);
  for(long y = yStart; y < yEnd; y++) {
    for(long x = 0; x < width; x++) {
      //printf("(%ld %ld)\n",x,y);
      //gbDebug = x == 758 && y == 855 ? 1 : 0;
      RGB pixel = Shoot(
        x,y,
        gbScene,
        gbRoot,gbTreeObjectLength,
        gbUnboundedObjectList,gbUnboundedObjectListLength
      );
      BMP_SetPixel(&gbCanvas,x,y,pixel);
    }
  }
  printf("end thread %ld\n",(long)pthread_self());
  return NULL;
}

// **** END THREAD CALLBACK FUNCTION ****

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

  if(!Scene_Setup(fp, &gbScene)) {
    Scene_Free(gbScene);
    fclose(fp);
    return 0;
  }

  fclose(fp);
  Scene_Print(gbScene);
  //exit(0);

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

  Camera_Setup(width,height,gbScene->cam);

  printf("--- RAY TRACING GO! ---\n");
  ttTime();

  pthread_t *threads = NULL;
  ThreadData *data = NULL;
  if(Thread_Create(width,height,options.threads,&threads,&data)) {
    Thread_Join(options.threads,threads);
    Thread_Free(threads,data);
  }

  printf("raytracing elaped time was: %f seconds\n",ttTime());

  BMP_Save(&gbCanvas,gbScene->fileName);
  BMP_Free(&gbCanvas);

  BBOXTree_Free(gbRoot);

  Scene_Free(gbScene);

  return 0;
}
