#ifndef BBOX_H
#define BBOX_H

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "helpers.h"
#include "datatypes.h"
#include "vector.h"

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
