#include "camera.h"

Camera *Camera_New(
  Vector pos,
  Vector upVector,
  Vector viewPoint,
  Vector min,
  Vector max
) {

  Vector viewDir = Vector_Normalize(Vector_FromP1toP2(pos,viewPoint));
  Vector upPoint = Vector_AddVector(pos,upVector);
  Vector upDir = Vector_Normalize(Vector_FromP1toP2(pos,upPoint));
  //Vector_Print(viewDir); Vector_Print(upDir); printf("==========\n");

  // create an orthonormal basis

  // up
  Vector up = Vector_Normalize(
    Vector_SubVector(
      upDir,
      Vector_MulScalar(
        viewDir,
        Vector_Dot(viewDir, upDir)
      )
    )
  );
  //printf("up: "); Vector_Print(up);

  // left
  Vector left = Vector_Normalize(Vector_Cross(up, viewDir));
  //printf("left: "); Vector_Print(left);

  //printf("viewDir: "); Vector_Print(viewDir);
  //printf("are normal? %5.5f %5.5f %5.5f\n",Vector_Dot(up,left),Vector_Dot(up,viewDir),Vector_Dot(viewDir,left));

  Camera *cam = malloc(sizeof(Camera));
  if(!cam) return NULL;

  cam->pos = pos;
  // save basis vectors
  cam->up = up;
  cam->left = left;
  cam->viewDir = viewDir;

  // create orthnormal matrix
  Matrix t1 = Matrix_New();
  t1._30 = -viewPoint.x;
  t1._31 = -viewPoint.y;
  t1._32 = -viewPoint.z;

  Matrix t2 = Matrix_New();
  t2._00 = left.x; t2._01 = up.x; t2._02 = viewDir.x;
  t2._10 = left.y; t2._11 = up.y; t2._12 = viewDir.y;
  t2._20 = left.z; t2._21 = up.z; t2._22 = viewDir.z;

  cam->local = Matrix_Mul(t1,t2);
  //Matrix_Print(cam->local); printf("==============\n");

  // create inverse orthonormal matrix
  t1._30 = viewPoint.x;
  t1._31 = viewPoint.y;
  t1._32 = viewPoint.z;

  t2 = Matrix_Transpose(t2);

  cam->invLocal = Matrix_Mul(t2,t1);
  //Matrix_Print(cam->invLocal); printf("============\n");

  //Matrix_Print(Matrix_Mul(cam->local,cam->invLocal)); printf("==========\n");

  cam->min = min;
  cam->max = max;

  return cam;
}

void Camera_Setup(
  long width,
  long height,
  Camera *cam
) {

  Matrix win = Windowing_New(
      Vector2d_New(0,0),Vector2d_New(width,height),
      cam->min,cam->max,
      1 // invert y screen axis
  );

  cam->win = win;
}

void Camera_Free(Camera *cam) {
  free(cam);
}

void Camera_Print(Camera *cam) {
  printf("==== CAMERA CONFIG ===\n");
  printf("left: %5.5f %5.5f %5.5f\n",
          cam->left.x,cam->left.y,cam->left.z);
  printf("up: %5.5f %5.5f %5.5f\n",
          cam->up.x,cam->up.y,cam->up.z);
  printf("viewDir: %5.5f %5.5f %5.5f\n",
        cam->viewDir.x,cam->viewDir.y,cam->viewDir.z);
  printf("min and max:\n");
  Vector_Print(cam->min);
  Vector_Print(cam->max);
  printf("win\n");
  Matrix_Print(cam->win);
}
