#ifndef SHOOT_H
#define SHOOT_H

#include <stdio.h>

#include "datatypes.h"
#include "rgb.h"
#include "ray.h"
#include "shade.h"

RGB Shoot(long x, long y, Scene *scene);
Ray Shoot_BuildRay(double x, double y, Camera cam);

#endif
