#ifndef SHADE_H
#define SHADE_H

#include "datatypes.h"
#include "rgb.h"
#include "bbox.h"

RGB Shade(Ray ray, BBOX *bbox, RGB bkgColor);

#endif
