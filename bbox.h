#ifndef BBOX_H
#define BBOX_H

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "helpers.h"
#include "vector.h"
#include "object.h"
#include "sphere.h"

#define BBOX_AXES_COUNT 3

typedef struct BBOXStruct {
  Object *obj;
  int isUnbounded;
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
BBOX *BBOXList_New(
  Object *objList,
  Object **unboundObjList,
  long *bboxListLength
);
BBOXTree *BBOXTree_New(Object *objList, Object **unboundObjList);
void BBOXTree_ComputeNodeBBOX(BBOXTree *node, BBOX *bboxList);
int BBOXTree_GenerateSplitLists(
  BBOXTree *node,
  BBOX *list,
  BBOX **leftList,
  long *leftListLength,
  BBOX **rightList,
  long *rightListLength
);
BBOXTree *BBOXTree_BuildHierarchy(
  BBOX *list,
  long listLength,
  int forceGrouping
);

void BBOX_Print(BBOX *bbox);
void BBOX_Free(BBOX *bbox);

long BBOX_ToObjectList(BBOX *bboxList, Object **object);
void BBOXTree_Free(BBOXTree *tree);
void BBOXTree_Print(BBOXTree *tree);

int BBOX_Intersect(Ray ray, BBOX *bbox);
int BBOXTree_NodeIntersect(Ray ray, BBOXTree *node);

void BBOXList_Print(BBOX *bboxList);
void BBOXList_Free(BBOX *bboxList);

void BBOX_Sphere(BBOX *bbox);

#endif
