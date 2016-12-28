#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>

#include "helpers.h"
#include "datatypes.h"

Matrix Matrix_New();
Matrix Matrix_Mul(Matrix left, Matrix right);
Matrix Matrix_Transpose(Matrix m);
void Matrix_Print(Matrix m);

#endif
