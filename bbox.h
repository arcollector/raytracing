#ifndef BBOX_H
#define BBOX_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"
#include "vector.h"
#include "object.h"

#define BBOX_AXES_COUNT 3

extern int gbDebug;

typedef struct BBOXStruct {
  Object *obj;
  struct BBOXStruct *next;
  Vector min[BBOX_AXES_COUNT], max[BBOX_AXES_COUNT], centroid[BBOX_AXES_COUNT];
} BBOX;

typedef struct BBOXTreeStruct {
  struct BBOXTreeStruct *left, *right;
  BBOX bbox;
  Object *objectList;
  long objectListLength;
} BBOXTree;

Vector BBOX_GetAxis(int index);

BBOX *BBOX_New(Object *obj);
void BBOX_Print(BBOX *bbox);
void BBOX_Free(BBOX *bbox);

BBOX *BBOXList_New(
  Object *objectList,
  long *bboxListLength,
  Object **unboundedObjectList,
  long *unboundedObjectListLength
);
void BBOXList_Print(BBOX *bboxList);
void BBOXList_Free(BBOX *bboxList);

BBOXTree *BBOXTree_New(
  Object *objectList,
  long *treeObjectLength,
  Object **unboundedObjectList,
  long *unboundedObjectListLength
);
void BBOXTree_Free(BBOXTree *tree);
void BBOXTree_Print(BBOXTree *tree);

int BBOX_Intersect(Ray ray, BBOX *bbox);
int BBOXTree_NodeIntersect(Ray ray, BBOXTree *node);

#endif
