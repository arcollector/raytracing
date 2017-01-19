#ifndef BBOX_H
#define BBOX_H

#include <stdio.h>

#include "helpers.h"
#include "datatypes.h"
#include "vector.h"

BBOX *BBOX_New(Object *objList);
void BBOX_Sphere(BBOX *bbox);
int BBOX_Intersect(Ray ray, BBOX *bbox);
void BBOX_Print(BBOX *bbox);
void BBOX_Free(BBOX *bbox);

#endif
