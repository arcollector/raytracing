#include "shade.h"

RGB Shade(Ray ray, Object *objList, RGB bkgColor) {

  // intersect
  double lastT = 1e10;
  RGB lastColor = bkgColor;
  for(Object *node = objList; node; node = node->next) {
    double t = (*node->intersect)(ray,node->obj);
    if(t > 0 && t < lastT) {
      lastT = t;
      lastColor = (*node->getColor)(node->obj);
    }
  }

  return lastColor;
}
