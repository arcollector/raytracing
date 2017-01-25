#ifndef WINDOWING_H
#define WINDOWING_H

#include "vector.h"
#include "matrix.h"

Matrix Windowing_New(
  Vector minS, Vector maxS,
  Vector minT, Vector maxT,
  int isYAxisSourceInv
);

#endif
