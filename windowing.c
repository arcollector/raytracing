#include "windowing.h"

Matrix Windowing_New(Vector minS, Vector maxS,
                    Vector minT, Vector maxT,
                    int isYAxisSourceInv) {

  // xt = (xs - mins)/(maxs - mins)*(maxt - mint) + mint;

  Matrix t1 = Matrix_New();
  t1._30 = -minS.x;
  t1._31 = -minS.y;

  double fx = 1/(maxS.x - minS.x);
  double fy = 1/(maxS.y - minS.y);
  Matrix t2 = Matrix_New();
  t2._00 = fx;
  t2._11 = fy;

  t1 = Matrix_Mul(t1,t2);

  if(isYAxisSourceInv) {
    Matrix tmp = Matrix_New();
    tmp._11 = -1;
    tmp._31 = 1;
    t1 = Matrix_Mul(t1,tmp);
  }

  Matrix t3 = Matrix_New();
  t3._00 = maxT.x - minT.x;
  t3._11 = maxT.y - minT.y;
  t3._30 = minT.x;
  t3._31 = minT.y;

  t1 = Matrix_Mul(t1,t3);

  return t1;
}
