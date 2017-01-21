#ifndef SHADE_H
#define SHADE_H

#include "datatypes.h"
#include "rgb.h"
#include "bbox.h"

RGB Shade(Ray ray, BBOXTree *root, Object *unboundObjList, RGB bkgColor);

#endif
