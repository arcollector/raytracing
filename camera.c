#include "camera.h"
#define CAMERA_DEBUG 0

Camera Camera_New(
  Vector pos,
  Vector upPoint,
  Vector viewPoint,
  double viewerDistance,
  Vector min,
  Vector max
) {

  Vector viewDir = Vector_Normalize(Vector_FromP1toP2(pos,viewPoint));
  Vector upDir = Vector_Normalize(Vector_FromP1toP2(pos,upPoint));
  #if CAMERA_DEBUG == 1
  Vector_Print(viewDir); Vector_Print(upDir); printf("==========\n");
  #endif
  // create orthnormal basis

  // up
  Vector w = Vector_MulScalar(viewDir,Vector_Dot(viewDir,upDir));
  Vector up = Vector_SubVector(upDir,w);
  up = Vector_Normalize(up);
  #if CAMERA_DEBUG == 1
  printf("Up: "); Vector_Print(up);
  printf("up length: %5.5f\n",Vector_Length(up));
  #endif

  // left
  Vector left = Vector_Cross(up,viewDir);
  left = Vector_Normalize(left);
  #if CAMERA_DEBUG == 1
  printf("left: "); Vector_Print(left);
  printf("left length: %5.5f\n",Vector_Length(left));

  printf("view: "); Vector_Print(viewDir);
  printf("are normal? %5.5f %5.5f %5.5f\n",Vector_Dot(up,left),
             Vector_Dot(up,viewDir),Vector_Dot(viewDir,left));
  #endif

  Camera cam;
  cam.pos = pos;
  // save basis vectors
  cam.up = up;
  cam.left = left;
  cam.view = viewDir;

  // create orthnormal matrix
  Matrix t1 = Matrix_New();
  t1._30 = -pos.x;
  t1._31 = -pos.y;
  t1._32 = -pos.z;

  Matrix t2 = Matrix_New();
  t2._00 = left.x; t2._01 = up.x; t2._02 = viewDir.x;
  t2._10 = left.y; t2._11 = up.y; t2._12 = viewDir.y;
  t2._20 = left.z; t2._21 = up.z; t2._22 = viewDir.z;

  cam.local = Matrix_Mul(t1,t2);
  #if CAMERA_DEBUG == 1
  Matrix_Print(cam.local); printf("==============\n");
  #endif

  // create inverse orthonormal matrix
  t1._30 = pos.x;
  t1._31 = pos.y;
  t1._32 = pos.z;

  t2 = Matrix_Transpose(t2);

  cam.invLocal = Matrix_Mul(t2,t1);
  #if CAMERA_DEBUG == 1
  Matrix_Print(cam.invLocal); printf("============\n");

  Matrix_Print(Matrix_Mul(cam.local,cam.invLocal)); printf("==========\n");
  #endif

  cam.vpd = viewerDistance;
  cam.min = min;
  cam.max = max;

  return cam;
}

Matrix Camera_GetScreenToViewingPlaneMatrix(
  long width,
  long height,
  Camera cam
) {

  Matrix win = Windowing_New(
      Vector_New(0,0,0),Vector_New(width,height,0),
      cam.min,cam.max,
      1 // invert y screen axis
  );

  return win;
}

Vector Camera_GetViewerPosition(Camera cam) {
  return Vector_New(0,0,-cam.vpd);
}

void Camera_Print(Camera cam) {
  printf("cam.up: %5.5f %5.5f %5.5f\n",
          cam.up.x,cam.up.y,cam.up.z);
  printf("cam.left: %5.5f %5.5f %5.5f\n",
          cam.left.x,cam.left.y,cam.left.z);
  printf("cam.view: %5.5f %5.5f %5.5f\n",
        cam.view.x,cam.view.y,cam.view.z);
  printf("cam.vpd: %5.5f\n", cam.vpd);
  printf("cam.min and cam.max:\n");
  Vector_Print(cam.min);
  Vector_Print(cam.max);
}
