#include "matrix.h"

Matrix Matrix_New() {
  Matrix m;
  m._00 = 1; m._01 = 0; m._02 = 0; m._03 = 0;
  m._10 = 0; m._11 = 1; m._12 = 0; m._13 = 0;
  m._20 = 0; m._21 = 0; m._22 = 1; m._23 = 0;
  m._30 = 0; m._31 = 0; m._32 = 0; m._33 = 1;
  return m;
}

Matrix Matrix_Mul(Matrix left, Matrix right) {
  Matrix m = Matrix_New();

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
  Matrix t;
  
  t._00 = row1.x; t._01 = row1.y; t._02 = row1.z; t._03 = row1.w;
  t._10 = row2.x; t._11 = row2.y; t._12 = row2.z; t._13 = row2.w;
  t._20 = row3.x; t._21 = row3.y; t._22 = row3.z; t._23 = row3.w;
  t._30 = row4.x; t._31 = row4.y; t._32 = row4.z; t._33 = row4.w;

  return t;
}

Matrix Matrix_Transpose(Matrix m) {
  Matrix t;
  
  t._00 = m._00; t._01 = m._10; t._02 = m._20; t._03 = m._30;
  t._10 = m._01; t._11 = m._11; t._12 = m._21; t._13 = m._31;
  t._20 = m._02; t._21 = m._12; t._22 = m._22; t._23 = m._32;
  t._30 = m._03; t._31 = m._13; t._32 = m._23; t._33 = m._33;

  return t;
}

void Matrix_Print(Matrix m) {
  printf("[%5.5f %5.5f %5.5f %5.5f]\n",
          m._00, m._01, m._02, m._03);
  printf("[%5.5f %5.5f %5.5f %5.5f]\n",
          m._10, m._11, m._12, m._13);
  printf("[%5.5f %5.5f %5.5f %5.5f]\n",
          m._20, m._21, m._22, m._23);
  printf("[%5.5f %5.5f %5.5f %5.5f]\n",
          m._30, m._31, m._32, m._33);
}

