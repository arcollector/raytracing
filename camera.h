#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>

#include "datatypes.h"
#include "vector.h"
#include "matrix.h"
#include "windowing.h"

Camera Camera_New(
  Vector pos,
  Vector upPoint,
  Vector viewPoint,
  double viewerDistance,
  Vector min,
  Vector max
);

void Camera_PrepareForShooting(
  long width,
  long height,
  Camera *cam
);

void Camera_Print(Camera cam);

#endif
