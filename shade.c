#include "shade.h"

RGB Shade(
  Ray ray,
  Camera cam,
  BBOXTree *root,
  Object *unboundObjList,
  Texture *sky
) {

  double lastT = POSITIVE_INFINITY;
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
    for(Object *obj = node->objectList; obj; obj = obj->next) {
      double t = (*obj->intersect)(ray, obj->primitive);
      if(t > 0 && t < lastT) {
        lastT = t;
        lastColor = (*obj->getColor)(Ray_PointAt(ray,t),cam,obj->primitive);
      }
    }
  }

  for(Object *obj = unboundObjList; obj; obj = obj->next) {
    double t = (*obj->intersect)(ray, obj->primitive);
    if(t > 0 && t < lastT) {
      lastT = t;
      lastColor = (*obj->getColor)(Ray_PointAt(ray,t),cam,obj->primitive);
    }
  }

  return lastColor;
}
