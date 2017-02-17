#include "vector.h"

Vector Vector_New(double x, double y, double z) {
  return (Vector){
    x, y, z, 1
  };
}

Vector Vector2d_New(double x, double y) {
  return (Vector){
    x, y, 0, 1
  };
}

Vector Vector4d_New(double x, double y, double z, double w) {
  return (Vector){
    x, y, z, w
  };
}

Vector Vector_FromP1toP2(Vector p1,Vector p2) {
  return (Vector){
    p2.x - p1.x,
    p2.y - p1.y,
    p2.z - p1.z,
    1
  };
}

Vector Vector_FromRGB(RGB color) {
  return (Vector){
    color.red / 255.,
    color.green / 255.,
    color.blue / 255.,
    1
  };
}

RGB Vector_ToRGB(Vector color) {
  RGB res;
  res.red = MAX(0,MIN(255,ROUND(color.x * 255)));
  res.green = MAX(0,MIN(255,ROUND(color.y * 255)));
  res.blue = MAX(0,MIN(255,ROUND(color.z * 255)));
  return res;
}

Vector Vector_AddVector(Vector v1,Vector v2) {
  return (Vector){
    v1.x + v2.x,
    v1.y + v2.y,
    v1.z + v2.z,
    1
  };
}

Vector Vector_AddScalar(Vector v,double scalar) {
  return (Vector){
    v.x + scalar,
    v.y + scalar,
    v.z + scalar,
    1
  };
}

Vector Vector_SubVector(Vector v1, Vector v2) {
  return (Vector){
    v1.x - v2.x,
    v1.y - v2.y,
    v1.z - v2.z,
    1
  };
}

Vector Vector_Negate(Vector v) {
  return (Vector){
    -v.x,
    -v.y,
    -v.z,
    1
  };
}

double Vector_Dot(Vector v1, Vector v2) {
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector Vector_Cross(Vector v1, Vector v2) {
  return (Vector){
    v1.y*v2.z - v1.z*v2.y,
    v1.z*v2.x - v1.x*v2.z,
    v1.x*v2.y - v1.y*v2.x,
    1
  };
}

Vector Vector_MulVector(Vector v1, Vector v2) {
  return (Vector){
    v1.x * v2.x,
    v1.y * v2.y,
    v1.z * v2.z,
    1
  };
}

Vector Vector_MulScalar(Vector v, double scalar) {
  return (Vector){
    v.x * scalar,
    v.y * scalar,
    v.z * scalar,
    1
  };
}

Vector Vector_MulMatrix(Vector v, Matrix m) {
  double x, y, z, w;
  x = v.x*m._00 + v.y*m._10 + v.z*m._20 + v.w*m._30;
  y = v.x*m._01 + v.y*m._11 + v.z*m._21 + v.w*m._31;
  z = v.x*m._02 + v.y*m._12 + v.z*m._22 + v.w*m._32;
  w = v.x*m._03 + v.y*m._13 + v.z*m._23 + v.w*m._33;

  if(fabs(w) < EPSILON) {
    printf("WARNING: w ordinate is zero!!!\n");
    return VECTOR_NAN;
  }

  return (Vector){
    x / w,
    y / w,
    z / w,
    1
  };
}

Vector Vector_MulMatrixNotTranslation(Vector v, Matrix m) {
  m._30 = 0;
  m._31 = 0;
  m._32 = 0;
  return Vector_MulMatrix(v, m);
}

Vector Vector_DivVector(Vector v1, Vector v2) {
  return (Vector){
    v1.x / v2.x,
    v1.y / v2.y,
    v1.z / v2.z,
    1
  };
}

Vector Vector_DivScalar(Vector v, double scalar) {
  if(fabs(scalar) <= EPSILON) return VECTOR_NAN;
  return (Vector){
    v.x / scalar,
    v.y / scalar,
    v.z / scalar,
    1
  };
}

double Vector_Length(Vector v) {
  double sum = v.x*v.x + v.y*v.y + v.z*v.z;
  return fabs(sum - 1) < EPSILON ? 1 : sqrt(sum);
}

Vector Vector_Normalize(Vector v) {
  return Vector_DivScalar(v,Vector_Length(v));
}

Vector Vector_Reflect(Vector incident, Vector normal) {
  incident = Vector_Normalize(incident);
  normal = Vector_Normalize(normal);
  double cosI = Vector_Dot(incident,normal);
  if(cosI < 0) {
    cosI = -cosI;
    incident = Vector_Negate(incident);
  }
  return Vector_SubVector(
    Vector_MulScalar(
      normal,
      2 * cosI
    ),
    incident
  );
}

Vector Vector_Refract(
  Vector incident,
  Vector normal,
  double nI,
  double nR,
  int *isTIR
) {

  normal = Vector_Normalize(normal);
  incident = Vector_Normalize(incident);
  double cosI = Vector_Dot(normal,incident);
  // incident is not pointing to the interface
  if(cosI < 0) {
    cosI = -cosI;
    incident = Vector_Negate(incident);
  }
  double sinI = sqrt(1 - cosI*cosI);
  // sinR > 1 means that it is not defined
  double sinR = sinI * nI / nR;
  double cosR = sinR > 1 ? NAN : sqrt(1 - sinR*sinR);

  // from greater dense medium to a lesser dense medium
  // total internal reflection can ocurr
  if(nI > nR) {
    // calc critical angle
    double criticalAngle = asin(nR/nI);
    double incidentAngle = acos(cosI);
    if(incidentAngle > criticalAngle) {
      *isTIR = 1;
      Vector reflected = Vector_Reflect(incident, normal);
      return reflected;
    }
  }
  *isTIR = 0;

  Vector Iw = Vector_MulScalar(normal,cosI);
  Vector Iu = Vector_SubVector(incident,Iw);

  Iu = Vector_Normalize(Vector_Negate(Iu));

  Vector Rw = Vector_MulScalar(Vector_Negate(normal),cosR);
  Vector Ru = Vector_MulScalar(Iu,sinR);

  Vector refraction = Vector_AddVector(Rw,Ru);

  return refraction;
}

void Vector_Print(Vector v) {
  printf("(%5.5f %5.5f %5.5f)\n",v.x,v.y,v.z);
}
