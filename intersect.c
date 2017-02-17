#include "intersect.h"

static Hit *Intersect_RayObject(
  Ray ray,
  Object *objectList,
  double *tValue,
  double dist,
  double *attenuate
) {

  Hit *returnHit = NULL, *tmp = NULL;
  int isShadowCasting = dist > 0;
  for(Object *obj = objectList; obj; obj = obj->next) {
    if(!(tmp = (*obj->intersect)(ray,obj->primitive))) continue;
    Hit_Begin(tmp);
    double t = Hit_Next(tmp);
    // Hit_Next return always the first positive t value (if any)
    if(t >= 0 && t < *tValue) {
      Hit_SetObject(obj, tmp);
      if(isShadowCasting) {
        // is object farthest than light source
        if(t > dist) continue;
        // rfr is the refraction coeff
        double rfr = obj->texture->rfr;
        // object is opaque
        if(rfr <= 0) return tmp;
        // attenuate shadow
        if(attenuate) *attenuate *= rfr;
      } else {
        *tValue = t;
        returnHit = tmp;
      }
    } else {
      Hit_Free(tmp);
    }
  }
  return returnHit;
}

static Hit *_Intersect(
  Ray ray,
  double *tValue,
  double dist,
  double *attenuate,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength
) {

  *tValue = POSITIVE_INFINITY;
  Hit *tmp = NULL, *intersected = NULL;
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
        (tmp = Intersect_RayObject(
          ray,
          node->objectList,
          tValue,
          dist,
          attenuate)
         )
      ) {
        intersected = tmp;
        if(isShadowCasting) isAbort = 1;
      }
    }
    free(stack);
    // for shadows only first intersection is sufficient
    if(isShadowCasting && intersected) return intersected;
  }

  if(unboundedObjectListLength &&
    (tmp = Intersect_RayObject(
      ray,
      unboundedObjectList,
      tValue,
      dist,
      attenuate
      )
    )
  ) {
    intersected = tmp;
    if(isShadowCasting) return intersected;
  }

  return intersected;
}

Hit *Intersect(
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
    NULL,
    root,
    treeObjectLength,
    unboundedObjectList,
    unboundedObjectListLength
  );
}

int Intersect_Shadow(
  Ray ray,
  double dist,
  double *attenuate,
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
    attenuate,
    root,
    treeObjectLength,
    unboundedObjectList,
    unboundedObjectListLength
  ) != NULL;
}
