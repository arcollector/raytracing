#include "shade.h"

RGB Shade(
  Ray ray,
  Camera cam,
  BBOXTree *root,
  Object *unboundedObjectList,
  Texture *sky
) {

  double lastT = POSITIVE_INFINITY;
  Object *intersected = NULL, *tmp;
  RGB lastColor = Texture_GetColorRGB(Vector_New(0,0,0),sky);

  // TODO: make a linked list
  BBOXTree *stack[1000];
  long stackLength = 0;
  stack[stackLength++] = root;
  // root may null if ie scene only contains unbounded objects
  while(root && stackLength) {
    BBOXTree *node = stack[--stackLength];
    if(!BBOXTree_NodeIntersect(ray, node)) {
      continue;
    }
    if(node->left) stack[stackLength++] = node->left;
    if(node->right) stack[stackLength++] = node->right;
    // node is a leaf if node->objectList contains Objects
    if((tmp = Intersect(ray, node->objectList, &lastT))) intersected = tmp;
  }

  if((tmp = Intersect(ray, unboundedObjectList, &lastT))) intersected = tmp;
  
  if(intersected) lastColor = Shade_ComputeColor(intersected, ray, lastT, cam);

  return lastColor;
}

Object *Intersect(Ray ray, Object *objectList, double *lastT) {
  Object *returnObj = NULL;
  for(Object *obj = objectList; obj; obj = obj->next) {
    double t = (*obj->intersect)(ray, obj->primitive);
    if(t > 0 && t < *lastT) {
      *lastT = t;
      returnObj = obj;
    }
  }
  return returnObj;
}

RGB Shade_ComputeColor(Object *obj, Ray ray, double t, Camera cam) {
  return (*obj->getColor)(Ray_PointAt(ray,t),cam,obj->primitive);
}
