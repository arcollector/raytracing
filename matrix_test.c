#include <stdio.h>
#include <math.h>

#include "matrix.h"

#ifdef DEG2RAD
#undef DEG2RAD
#endif
#define DEG2RAD(deg) deg/180.*M_PI

int main(int argc,char *argv) {

  Matrix m1 = Matrix_New();
  Matrix m2 = Matrix_New();

  m1._00 = cos(DEG2RAD(45));
  m1._01 = sin(DEG2RAD(45));
  m1._10 = -m1._01;
  m1._11 = m1._00;

  printf("m1\n");
  Matrix_Print(m1);

  m2._00 = cos(DEG2RAD(45));
  m2._01 = -sin(DEG2RAD(45));
  m2._10 = -m2._01;
  m2._11 = m2._00;

  printf("m2\n");
  Matrix_Print(m2);

  printf("m1 * m2\n");
  Matrix res = Matrix_Mul(m1,m2);
  Matrix_Print(res);

  m1 = Matrix_New();
  m1._11 = cos(DEG2RAD(60));
  m1._12 = sin(DEG2RAD(60));
  m1._21 = -m1._12;
  m1._22 = m1._11;
  printf("m1\n");
  Matrix_Print(m1);

  m2 = Matrix_Transpose(m1);
  printf("m2\n");
  Matrix_Print(m2);

  printf("m1 * m2\n");
  res = Matrix_Mul(m1,m2);
  Matrix_Print(res);

  m1 = Matrix_New();
  m1._00 = 5;
  m1._11 = 10;
  m1._22 = 20;
  printf("m1\n");
  Matrix_Print(m1);

  m2 = Matrix_New();
  m2._00 = 2;
  m2._11 = 2;
  m2._22 = 2;
  printf("m2\n");
  Matrix_Print(m2);

  printf("m1 * m2\n");
  res = Matrix_Mul(m1,m2);
  Matrix_Print(res);
  
  m1 = res;

  m2 = Matrix_New();
  m2._00 = 1/10.;
  m2._11 = 1/20.;
  m2._22 = 1/40.;
  printf("m2\n");
  Matrix_Print(m2);

  printf("m1 * m2\n");
  res = Matrix_Mul(m1,m2);
  Matrix_Print(res);
  
  return 0;
}

