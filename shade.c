#include "shade.h"

RGB Shade(Ray ray, BBOX *bbox, RGB bkgColor) {

  double lastT = 1e10;
  RGB lastColor = bkgColor;

  for(; bbox; bbox = bbox->next) {
    // check if ray intersect bouding box, and if so...
    if(BBOX_Intersect(ray, bbox)) {
      Object *obj = bbox->obj;
      // ... call primitive intersect method, and finally...
      double t = (*obj->intersect)(ray, obj->primitive);
      // ... check if t value is acceptable
      if(t > 0 && t < lastT) {
        lastT = t;
        lastColor = (*obj->getColor)(obj->primitive);
      }
    }
  }

  return lastColor;
}
