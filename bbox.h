#ifndef BBOX_H
#define BBOX_H

#include <stdio.h>
#include <math.h>

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
  Object *objectList,
  Object **unboundedObjectList,
  long *bboxListLength,
  long *unboundedObjectListLength
);
long BBOXList_ToObjectList(BBOX *bboxList, Object **object);
BBOXTree *BBOXTree_New(
  Object *objectList,
  long *treeObjectLength,
  Object **unboundedObjectList,
  long *unboundedObjectListLength
);
void BBOXTree_ComputeNodeBBOX(BBOXTree *node, BBOX *bboxList);
void BBOXTree_GenerateSplitLists(
  BBOXTree *node,
  BBOX *list,
  long listLength,
  BBOX **leftList,
  long *leftListLength,
  BBOX **rightList,
  long *rightListLength
);
void BBOXArr_ToList(
  BBOX **dest,
  BBOX **soure,
  long sourceLength
);
BBOXTree *BBOXTree_BuildHierarchy(
  BBOX *list,
  long listLength,
  long *treeObjectLength,
  int forceGrouping
);
void BBOXTree_InitStack(long treeObjectLength);
BBOXTree **BBOXTree_GetStack();
void BBOXTree_FreeStack();

void BBOX_Print(BBOX *bbox);
void BBOX_Free(BBOX *bbox);

void BBOXTree_Free(BBOXTree *tree);
void BBOXTree_Print(BBOXTree *tree);

int BBOX_Intersect(Ray ray, BBOX *bbox);
int BBOXTree_NodeIntersect(Ray ray, BBOXTree *node);

void BBOXList_Print(BBOX *bboxList);
void BBOXList_Free(BBOX *bboxList);

void BBOX_Sphere(BBOX *bbox);

#endif
