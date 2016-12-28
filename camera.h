#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>

#include "datatypes.h"
#include "vector.h"
#include "matrix.h"

Camera Camera_New(
  Vector pos,
  Vector upPoint,
  Vector viewPoint
);

void Camera_Print(Camera cam);

#endif
