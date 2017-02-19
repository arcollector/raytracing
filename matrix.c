#include "matrix.h"

Matrix Matrix_New() {
  return (Matrix){
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };
}

Matrix Matrix_Mul(Matrix left, Matrix right) {
  Matrix m;

  m._00 = left._00*right._00 + left._01*right._10 +
          left._02*right._20 + left._03*right._30;

  m._01 = left._00*right._01 + left._01*right._11 +
          left._02*right._21 + left._03*right._31;

  m._02 = left._00*right._02 + left._01*right._12 +
          left._02*right._22 + left._03*right._32;

  m._03 = left._00*right._03 + left._01*right._13 +
          left._02*right._23 + left._03*right._33;
  // ----------------------------------
  m._10 = left._10*right._00 + left._11*right._10 +
          left._12*right._20 + left._13*right._30;

  m._11 = left._10*right._01 + left._11*right._11 +
          left._12*right._21 + left._13*right._31;

  m._12 = left._10*right._02 + left._11*right._12 +
          left._12*right._22 + left._13*right._32;

  m._13 = left._10*right._03 + left._11*right._13 +
          left._12*right._23 + left._13*right._33;
  // ----------------------------------
  m._20 = left._20*right._00 + left._21*right._10 +
          left._22*right._20 + left._23*right._30;

  m._21 = left._20*right._01 + left._21*right._11 +
          left._22*right._21 + left._23*right._31;

  m._22 = left._20*right._02 + left._21*right._12 +
          left._22*right._22 + left._23*right._32;

  m._23 = left._20*right._03 + left._21*right._13 +
          left._22*right._23 + left._23*right._33;
  // ----------------------------------
  m._30 = left._30*right._00 + left._31*right._10 +
          left._32*right._20 + left._33*right._30;

  m._31 = left._30*right._01 + left._31*right._11 +
          left._32*right._21 + left._33*right._31;

  m._32 = left._30*right._02 + left._31*right._12 +
          left._32*right._22 + left._33*right._32;

  m._33 = left._30*right._03 + left._31*right._13 +
          left._32*right._23 + left._33*right._33;

  return m;
}

Matrix Matrix_FromVectorsAsRows(
  Vector row1, Vector row2, Vector row3, Vector row4
) {
  return (Matrix){
    row1.x, row1.y, row1.z, row1.w,
    row2.x, row2.y, row2.z, row2.w,
    row3.x, row3.y, row3.z, row3.w,
    row4.x, row4.y, row4.z, row4.w
  };
}

Matrix Matrix_Transpose(Matrix m) {
  return (Matrix){
    m._00, m._10, m._20, m._30,
    m._01, m._11, m._21, m._31,
    m._02, m._12, m._22, m._32,
    m._03, m._13, m._23, m._33
  };
}

void Matrix_Print(Matrix m) {
  printf(
    "[%5.5f %5.5f %5.5f %5.5f]\n"
    "[%5.5f %5.5f %5.5f %5.5f]\n"
    "[%5.5f %5.5f %5.5f %5.5f]\n"
    "[%5.5f %5.5f %5.5f %5.5f]\n",
    m._00, m._01, m._02, m._03,
    m._10, m._11, m._12, m._13,
    m._20, m._21, m._22, m._23,
    m._30, m._31, m._32, m._33
  );
}
