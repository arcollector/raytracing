#include "intersect.h"

static Object *Intersect_RayObject(
  Ray ray,
  Object *objectList,
  double *tValue,
  double dist
) {

  Object *returnObj = NULL;
  int isShadowCasting = dist > 0;
  for(Object *obj = objectList; obj; obj = obj->next) {
    double t = (*obj->intersect)(ray,obj->primitive);
    if(t > 0 && t < *tValue) {
      // dist is used to test if t has length less than dist
      // used for shadow testing
      if(isShadowCasting) {
        if(t < dist) return obj;
      } else {
        *tValue = t;
        returnObj = obj;
      }
    }
  }
  return returnObj;
}

static Object *_Intersect(
  Ray ray,
  double *tValue,
  double dist,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
) {

  *tValue = POSITIVE_INFINITY;
  Object *tmp, *intersected = NULL;
  int isShadowCasting = dist > 0;
  // root is null if the scene contains only unbounded objects
  if(root) {
    BBOXTree **stack = calloc(treeObjectLength,sizeof(BBOXTree *));
    long stackLength = 0;
    int isAbort = 0;
    stack[stackLength++] = root;
    while(stackLength && !isAbort) {
      BBOXTree *node = stack[--stackLength];
      if(!BBOXTree_NodeIntersect(ray,node)) {
        continue;
      }
      // when node is leaf then left and right are null
      if(node->left) stack[stackLength++] = node->left;
      if(node->right) stack[stackLength++] = node->right;
      // node is leaf if node->objectList contains objects
      if(node->objectListLength &&
        (tmp = Intersect_RayObject(ray,node->objectList,tValue,dist))) {
        intersected = tmp;
        if(isShadowCasting) isAbort = 1;
      }
    }
    free(stack);
    // for shadows only first intersection is sufficient
    if(isShadowCasting && intersected) return intersected;
  }

  if(unboundedObjectListLength &&
    (tmp = Intersect_RayObject(ray,unboundedObjectList,tValue,dist))) {
    intersected = tmp;
    if(isShadowCasting) return intersected;
  }

  return intersected;
}

Object *Intersect(
  Ray ray,
  double *tValue,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
) {

  return _Intersect(
    ray,
    tValue,
    0,
    root,
    treeObjectLength,
    unboundedObjectList,
    unboundedObjectListLength
  );
}

int Intersect_Shadow(
  Ray ray,
  double dist,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
) {

  double tValue;
  return _Intersect(
    ray,
    &tValue,
    dist,
    root,
    treeObjectLength,
    unboundedObjectList,
    unboundedObjectListLength
  ) != NULL;
}
