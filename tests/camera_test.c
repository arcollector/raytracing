#include "../camera.h"

int main(int argc, char *argv) {

  Camera cam = Camera_New(
    Vector_New(3,0,2), // pos
    Vector_New(3,1,2), // up reference point
    Vector_New(0,0,0), // view reference point
    10, // eye viewplane distance
    Vector_New(-10,-10,0),Vector_New(10,10,0) // viewplane dim
  );

  Vector p = Vector_New(0,0,0);
  p = Vector_MulMatrix(p,cam.local);
  Vector_Print(p);

  Vector normal = Vector_New(1,0,0);
  Matrix m = cam.local;
  m._30 = 0; m._31 = 0; m._32 = 0;
  printf("matrix is:\n"); Matrix_Print(m);
  normal = Vector_MulMatrix(normal,m);
  printf("normal is: "); Vector_Print(normal);

  normal = Vector_New(1,0,0);
  m = Matrix_New();
  m._00 = cam.left.x; m._01 = cam.up.x; m._02 = cam.viewDir.x;
  m._10 = cam.left.y; m._11 = cam.up.y; m._12 = cam.viewDir.y;
  m._20 = cam.left.z; m._21 = cam.up.z; m._22 = cam.viewDir.z;
  printf("matrix is:\n"); Matrix_Print(m);
  normal = Vector_MulMatrix(normal,m);
  printf("normal is: "); Vector_Print(normal);


  return 0;
}
