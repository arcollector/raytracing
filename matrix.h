#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>

#include "helpers.h"

typedef struct {
  double _00,_01,_02,_03;
  double _10,_11,_12,_13;
  double _20,_21,_22,_23;
  double _30,_31,_32,_33;
} Matrix;

#include "vector.h"

Matrix Matrix_New();
Matrix Matrix_FromVectorsAsRows(
  Vector row1, Vector row2, Vector row3, Vector row4
);
Matrix Matrix_Mul(Matrix left, Matrix right);
Matrix Matrix_Transpose(Matrix m);
void Matrix_Print(Matrix m);

#endif
