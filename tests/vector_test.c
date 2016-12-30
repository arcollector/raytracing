#include <stdio.h>

#include "../vector.h"

int main(int argc, char args) {

  Vector v1 = Vector_New(1,2,3);
  Vector_Print(v1);
  v1 = Vector_MulScalar(v1,10);
  Vector_Print(v1);
  v1 = Vector_AddScalar(v1,5);
  Vector_Print(v1);
  v1 = Vector_Negate(v1);
  Vector_Print(v1);
  double l = Vector_Length(v1);
  printf("v1 length: %f\n",l);
  v1 = Vector_DivScalar(v1,l);
  Vector_Print(v1);
  l = Vector_Length(v1);
  printf("v1 length: %f\n",l);

  v1 = Vector_New(1,2,3);
  Vector v2 = Vector_New(4,5,6);
  Vector v3 = Vector_Cross(v1,v2);
  double dot1 = Vector_Dot(v1,v2);
  double dot2 = Vector_Dot(v1,v3);
  double dot3 = Vector_Dot(v2,v3);
  printf("%f %f %f\n",dot1,dot2,dot3);

  v3 = Vector_MulVector(v1,v2);
  Vector_Print(v3);
  v3 = Vector_AddVector(v1,v2);
  Vector_Print(v3);
  v3 = Vector_DivVector(v1,v2);
  Vector_Print(v3);
  v3 = Vector_SubVector(v1,v2);
  Vector_Print(v3);

  v1 = Vector_New(1,0,0);
  Vector_Print(v1);
  v2 = Vector_New(1,1,0);
  Vector_Print(v2);
  v1 = Vector_Normalize(v1);
  Vector_Print(v1);
  v2 = Vector_Normalize(v2);
  Vector_Print(v2);
  dot1 = Vector_Dot(v1,v2);
  printf("%f %f\n",dot1,acos(dot1)/M_PI*180.);

  Matrix m = Matrix_New();
  m._00 = 10;
  m._11 = 10;
  m._22 = 10;
  Matrix_Print(m);
  v1 = Vector_New(5,10,20);
  Vector_Print(v1);
  printf("v * m\n");
  v1 = Vector_MulMatrix(v1,m);
  Vector_Print(v1);

  m = Matrix_New();
  m._00 = cos(45/180.*M_PI);
  m._01 = sin(45/180.*M_PI);
  m._10 = -m._01;
  m._11 = m._00;
  Matrix_Print(m);
  v1 = Vector_New(1,0,0);
  Vector_Print(v1);
  printf("v * m\n");
  v1 = Vector_MulMatrix(v1,m);
  Vector_Print(v1);
  printf("v * m\n");
  v1 = Vector_MulMatrix(v1,m);
  Vector_Print(v1);
  return 0;
}
