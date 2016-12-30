#ifndef SHADE_H
#define SHADE_H

#include "datatypes.h"
#include "rgb.h"

RGB Shade(Ray ray, Object *objList, RGB bkgColor);

#endif
