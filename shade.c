#include "shade.h"

RGB Shade(
  Ray ray,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
) {

  double lastT = POSITIVE_INFINITY;
  Object *intersected = NULL, *tmp;
  Vector lastColor;

  // root is null if the scene contains only unbounded objects
  if(root) {
    BBOXTree **stack = BBOXTree_GetStack();
    long stackLength = 0;
    stack[stackLength++] = root;
    while(stackLength) {
      BBOXTree *node = stack[--stackLength];
      if(!BBOXTree_NodeIntersect(ray,node)) {
        continue;
      }
      if(node->left) stack[stackLength++] = node->left;
      if(node->right) stack[stackLength++] = node->right;
      // node is leaf if node->objectList contains objects
      if(node->objectListLength &&
          (tmp = Intersect(ray,node->objectList,&lastT))) intersected = tmp;
    }
  }

  if(unboundedObjectListLength &&
      (tmp = Intersect(ray,unboundedObjectList,&lastT))) intersected = tmp;

  lastColor = intersected ?
    Shade_ComputeColor(intersected,ray,lastT,scene->cam)
    :
    Texture_GetColor(ray,VECTOR_NULL,VECTOR_NULL,scene->sky);

  return Vector_ToRGB(lastColor);
}

Object *Intersect(Ray ray, Object *objectList, double *lastT) {
  Object *returnObj = NULL;
  for(Object *obj = objectList; obj; obj = obj->next) {
    double t = (*obj->intersect)(ray,obj->primitive);
    if(t > 0 && t < *lastT) {
      *lastT = t;
      returnObj = obj;
    }
  }
  return returnObj;
}

Vector Shade_ComputeColor(Object *obj, Ray ray, double t, Camera cam) {
  return (*obj->getColor)(
    ray,
    Ray_PointAt(ray,t),
    VECTOR_NULL,
    cam,
    obj->primitive
  );
}
