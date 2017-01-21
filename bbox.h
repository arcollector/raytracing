#ifndef BBOX_H
#define BBOX_H

#include <stdio.h>
#include <math.h>

#include "helpers.h"
#include "datatypes.h"
#include "vector.h"

Vector BBOX_GetAxis(int index);
BBOX *BBOX_New(Object *obj);
BBOXList *BBOXList_New(Object *objList, Object **unboundObjList);
BBOXTree *BBOXTree_New(Object *objList, Object **unboundObjList);
void BBOXTree_ComputeNodeBBOX(BBOXTree *node, BBOXList *bboxList);
void BBOXTree_GenerateSplittedList(
  BBOXTree *node,
  BBOXList *list,
  BBOXList *leftList,
  BBOXList *rightList
);
BBOXTree *BBOXTree_BuildHierarchy(
  BBOXList *list,
  int forceGrouping
);
long BBOXList_ToObjectList(BBOXList *bboxList, Object **object);
void BBOXTree_Free(BBOXTree *tree);
void BBOX_Print(BBOX *bbox);
void BBOXTree_Print(BBOXTree *tree);

int BBOX_Intersect(Ray ray, BBOX *bbox);
int BBOXTree_NodeIntersect(Ray ray, BBOXTree *node);

void BBOXList_Print(BBOXList *bboxList);
void BBOXList_Free(BBOXList *bboxList);

void BBOX_Sphere(BBOX *bbox);

#endif
