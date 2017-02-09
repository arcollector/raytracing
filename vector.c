#include "vector.h"

Vector Vector_New(double x, double y, double z) {
  Vector res;
  res.x = x;
  res.y = y;
  res.z = z;
  res.w = 1;
  return res;
}

Vector Vector2d_New(double x, double y) {
  Vector res;
  res.x = x;
  res.y = y;
  res.z = 0;
  res.w = 1;
  return res;
}

Vector Vector4d_New(double x, double y, double z, double w) {
  Vector res;
  res.x = x;
  res.y = y;
  res.z = z;
  res.w = w;
  return res;
}

Vector Vector_FromP1toP2(Vector p1,Vector p2) {
  Vector res;
  res.x = p2.x - p1.x;
  res.y = p2.y - p1.y;
  res.z = p2.z - p1.z;
  res.w = 1;
  return res;
}

Vector Vector_FromRGB(RGB color) {
  Vector res;
  res.x = color.red / 255.;
  res.y = color.green / 255.;
  res.z = color.blue / 255.;
  return res;
}

RGB Vector_ToRGB(Vector color) {
  RGB res;
  res.red = MAX(0,MIN(255,ROUND(color.x * 255)));
  res.green = MAX(0,MIN(255,ROUND(color.y * 255)));
  res.blue = MAX(0,MIN(255,ROUND(color.z * 255)));
  return res;
}

Vector Vector_AddVector(Vector v1,Vector v2) {
  Vector res;
  res.x = v1.x + v2.x;
  res.y = v1.y + v2.y;
  res.z = v1.z + v2.z;
  res.w = 1;
  return res;
}

Vector Vector_AddScalar(Vector v,double scalar) {
  Vector res;
  res.x = v.x + scalar;
  res.y = v.y + scalar;
  res.z = v.z + scalar;
  res.w = 1;
  return res;
}

Vector Vector_SubVector(Vector v1, Vector v2) {
  Vector res;
  res.x = v1.x - v2.x;
  res.y = v1.y - v2.y;
  res.z = v1.z - v2.z;
  res.w = 1;
  return res;
}

Vector Vector_Negate(Vector v) {
  Vector res;
  res.x = -v.x;
  res.y = -v.y;
  res.z = -v.z;
  res.w = 1;
  return res; 
}

double Vector_Dot(Vector v1, Vector v2) {
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector Vector_Cross(Vector v1, Vector v2) {
  Vector res;
  res.x = v1.y*v2.z - v1.z*v2.y;
  res.y = v1.z*v2.x - v1.x*v2.z;
  res.z = v1.x*v2.y - v1.y*v2.x;
  res.w = 1;
  return res;
}

Vector Vector_MulVector(Vector v1, Vector v2) {
  Vector res;
  res.x = v1.x*v2.x;
  res.y = v1.y*v2.y;
  res.z = v1.z*v2.z;
  res.w = 1;
  return res;
}

Vector Vector_MulScalar(Vector v, double scalar) {
  Vector res;
  res.x = v.x * scalar;
  res.y = v.y * scalar;
  res.z = v.z * scalar;
  res.w = 1;
  return res;
}

Vector Vector_MulMatrix(Vector v, Matrix m) {
  Vector res;
  res.x = v.x*m._00 + v.y*m._10 + v.z*m._20 + v.w*m._30;
  res.y = v.x*m._01 + v.y*m._11 + v.z*m._21 + v.w*m._31;
  res.z = v.x*m._02 + v.y*m._12 + v.z*m._22 + v.w*m._32;
  res.w = v.x*m._03 + v.y*m._13 + v.z*m._23 + v.w*m._33;
  
  if(fabs(res.w) < EPSILON) {
    printf("WARNING: w ordinate is zero!!!\n");
    res.w = EPSILON;
  }

  res.x /= res.w;
  res.y /= res.w;
  res.z /= res.w;
  res.w = 1;

  return res;
}

Vector Vector_MulMatrixNotTranslation(Vector v, Matrix m) {
  m._30 = 0;
  m._31 = 0;
  m._32 = 0;
  return Vector_MulMatrix(v, m);
}

Vector Vector_DivVector(Vector v1, Vector v2) {
  Vector res;
  res.x = v1.x / v2.x;
  res.y = v1.y / v2.y;
  res.z = v1.z / v2.z;
  res.w = 1;
  return res;
}

Vector Vector_DivScalar(Vector v, double scalar) {
  Vector res;
  res.x = v.x / scalar;
  res.y = v.y / scalar;
  res.z = v.z / scalar;
  res.w = 1;
  return res;
}

double Vector_Length(Vector v) {
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vector Vector_Normalize(Vector v) {
  double l = Vector_Length(v);
  Vector res = Vector_DivScalar(v,l);
  return res;
}

void Vector_Print(Vector v) {
  printf("(%5.5f %5.5f %5.5f)\n",v.x,v.y,v.z);
}

