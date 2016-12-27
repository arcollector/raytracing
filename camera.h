#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>

#include "vector.h"
#include "matrix.h"

struct CameraStruct {
  Vector pos;
  Vector up,left,view;
  Matrix local, invLocal;
};

typedef struct CameraStruct Camera;

Camera Camera_New(
  Vector pos,
  Vector upPoint,
  Vector viewPoint
);

void Camera_Print(Camera cam);

#endif
