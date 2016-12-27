#include "windowing.h"

Matrix Windowing_New(Vector minS, Vector maxS,
                    Vector minT, Vector maxT,
                    int isYAxisSourceInv) {

  // xt = (xs - mins)/(maxs - mins)*(maxt - mint) + mint;
  
  Matrix t1 = Matrix_New();
  t1._30 = -minS.x;
  t1._31 = -minS.y;

  double fx = (maxT.x - minT.x)/(maxS.x - minS.x);
  double fy = (maxT.y - minT.y)/(maxS.y - minS.y);
  Matrix t2 = Matrix_New();
  t2._00 = fx;
  t2._11 = fy;

  Matrix t3 = Matrix_New();
  t3._30 = minT.x;
  t3._31 = minT.y;

  t1 = Matrix_Mul(t1,t2);
  t1 = Matrix_Mul(t1,t3);

  if(isYAxisSourceInv) {
    Matrix t4 = Matrix_New();
    t4._11 = -1;
    t4._31 = maxS.y;
    t1 = Matrix_Mul(t4,t1);
  }
  
  return t1;
}

