#include "intersect.h"

static Object *Intersect_RayObject(
  Ray ray,
  Object *objectList,
  double *tValue,
  double dist
);

Object *Intersect(
  Ray ray,
  double *tValue,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
) {

  *tValue = POSITIVE_INFINITY;
  Object *intersected = NULL, *tmp;
  // root is null if the scene contains only unbounded objects
  if(root) {
    BBOXTree **stack = calloc(treeObjectLength,sizeof(BBOXTree *));
    long stackLength = 0;
    stack[stackLength++] = root;
    while(stackLength) {
      BBOXTree *node = stack[--stackLength];
      if(!BBOXTree_NodeIntersect(ray,node)) {
        continue;
      }
      // when node is leaf then left and right are null
      if(node->left) stack[stackLength++] = node->left;
      if(node->right) stack[stackLength++] = node->right;
      // node is leaf if node->objectList contains objects
      if(node->objectListLength &&
        (tmp = Intersect_RayObject(ray,node->objectList,tValue,0))) {
        intersected = tmp;
      }
    }
    free(stack);
  }

  if(unboundedObjectListLength &&
    (tmp = Intersect_RayObject(ray,unboundedObjectList,tValue,0))) {
    intersected = tmp;
  }

  return intersected;
}

int Intersect_Shadow(
  Ray ray,
  double dist,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
) {

  double tValue = POSITIVE_INFINITY;
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
        Intersect_RayObject(ray,node->objectList,&tValue,dist)) {
        isAbort = 1;
      }
    }
    free(stack);
    if(isAbort) return 1;
  }

  if(unboundedObjectListLength &&
    Intersect_RayObject(ray,unboundedObjectList,&tValue,dist)) {
    return 1;
  }

  return 0;
}

Object *Intersect_RayObject(
  Ray ray,
  Object *objectList,
  double *tValue,
  double dist
) {

  Object *returnObj = NULL;
  for(Object *obj = objectList; obj; obj = obj->next) {
    double t = (*obj->intersect)(ray,obj->primitive);
    if(t > 0 && t < *tValue) {
      // dist is used to test if t has length less than dist
      // used for shadow testing
      if(gbDebug) printf("!!! %f %f\n", t, dist);
      if(dist > 0) {
        if(t < dist) return obj;
      } else {
        *tValue = t;
        returnObj = obj;
      }
    }
  }
  return returnObj;
}
