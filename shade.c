#include "shade.h"

RGB Shade(Ray ray, BBOXTree *root, Object *unboundObjList, RGB bkgColor) {

  double lastT = 1e10;
  RGB lastColor = bkgColor;

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
    if(node->objectListLength) {
      for(Object *obj = node->objectList; obj; obj = obj->next) {
        double t = (*obj->intersect)(ray, obj->primitive);
        if(t > 0 && t < lastT) {
          lastT = t;
          lastColor = (*obj->getColor)(obj->primitive);
        }
      }
    }
  }

  for(Object *obj = unboundObjList; obj; obj = obj->next) {
    double t = (*obj->intersect)(ray, obj->primitive);
    if(t > 0 && t < lastT) {
      lastT = t;
      lastColor = (*obj->getColor)(obj->primitive);
    }
  }

  return lastColor;
}
