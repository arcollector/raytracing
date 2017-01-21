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
  //Vector_Print(viewDir); Vector_Print(upDir); printf("==========\n");

  // create orthonormal basis

  // up
  Vector w = Vector_MulScalar(viewDir,Vector_Dot(viewDir,upDir));
  Vector up = Vector_SubVector(upDir,w);
  up = Vector_Normalize(up);
  //printf("Up: "); Vector_Print(up);
  //printf("up length: %5.5f\n",Vector_Length(up));

  // left
  Vector left = Vector_Cross(up,viewDir);
  left = Vector_Normalize(left);
  //printf("left: "); Vector_Print(left);
  //printf("left length: %5.5f\n",Vector_Length(left));
  //printf("viewDir: "); Vector_Print(viewDir);
  //printf("are normal? %5.5f %5.5f %5.5f\n",Vector_Dot(up,left),Vector_Dot(up,viewDir),Vector_Dot(viewDir,left));

  Camera cam;
  cam.pos = pos;
  // save basis vectors
  cam.up = up;
  cam.left = left;
  cam.viewDir = viewDir;

  // create orthnormal matrix
  cam.local = Matrix_New();
  cam.local._00 = left.x; cam.local._01 = up.x;   cam.local._02 = viewDir.x;
  cam.local._10 = left.y; cam.local._11 = up.y;   cam.local._12 = viewDir.y;
  cam.local._20 = left.z; cam.local._21 = up.z;   cam.local._22 = viewDir.z;
  cam.local._30 = -pos.x; cam.local._31 = -pos.y; cam.local._32 = -pos.z;
  //Matrix_Print(cam.local); printf("==============\n");

  // create inverse orthonormal matrix
  cam.invLocal = Matrix_New();
  cam.invLocal._00 = left.x;    cam.invLocal._01 = left.y;    cam.invLocal._02 = left.z;
  cam.invLocal._10 = up.x;      cam.invLocal._11 = up.y;      cam.invLocal._12 = up.z;
  cam.invLocal._20 = viewDir.x; cam.invLocal._21 = viewDir.y; cam.invLocal._22 = viewDir.z;
  cam.invLocal._30 = pos.x;     cam.invLocal._31 = pos.y;     cam.invLocal._32 = pos.z;
  //Matrix_Print(cam.invLocal); printf("============\n");
  //Matrix_Print(Matrix_Mul(cam.local,cam.invLocal)); printf("==========\n");

  cam.viewerPos = Vector_New(0,0,-viewerDistance);
  cam.min = min;
  cam.max = max;

  return cam;
}

void Camera_PrepareForShooting(
  long width,
  long height,
  Camera *cam
) {

  Matrix win = Windowing_New(
      Vector_New(0,0,0),Vector_New(width,height,0),
      cam->min,cam->max,
      1 // invert y screen axis
  );

  cam->win = win;
}

void Camera_Print(Camera cam) {
  printf("==== CAMERA CONFIG ===\n");
  printf("left: %5.5f %5.5f %5.5f\n",
          cam.left.x,cam.left.y,cam.left.z);
  printf("up: %5.5f %5.5f %5.5f\n",
          cam.up.x,cam.up.y,cam.up.z);
  printf("viewDir: %5.5f %5.5f %5.5f\n",
        cam.viewDir.x,cam.viewDir.y,cam.viewDir.z);
  printf("viewerPos: "); Vector_Print(cam.viewerPos);
  printf("min and max:\n");
  Vector_Print(cam.min);
  Vector_Print(cam.max);
  printf("win\n");
  Matrix_Print(cam.win);
}
