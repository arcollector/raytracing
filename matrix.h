#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>

#include "helpers.h"

struct MatrixStruct {
  double _00,_01,_02,_03;
  double _10,_11,_12,_13;
  double _20,_21,_22,_23;
  double _30,_31,_32,_33;
};

typedef struct MatrixStruct Matrix;

Matrix Matrix_New();
Matrix Matrix_Mul(Matrix left, Matrix right);
Matrix Matrix_Transpose(Matrix m);
void Matrix_Print(Matrix m);

#endif
