#include <stdio.h>
#include <stdlib.h>

#include "../helpers.h"
#include "../vector.h"
#include "../matrix.h"
#include "../windowing.h"
#include "../rgb.h"
#include "../bmp.h"

int main(int argc, char **args) {

  long width = 500;
  long height = 500;
  
  BMP_Canvas canvas;
  BMP_NewCanvas(&canvas,width,height);

  Matrix win = Windowing_New(
    Vector2d_New(0,0), Vector2_New(width,height),
    Vector2d_New(-10,-10), Vector2_New(10,10),
    1
  );
  Matrix scale = Matrix_FromVectorsAsRows(
    Vector4d_New(.30 , 0   , 0   , 0),
    Vector4d_New(0   , .30 , 0   , 0),
    Vector4d_New(0   , 0   , .30 , 0),
    Vector4d_New(0   , 0   , 0   , 1)
  );
  Matrix translation = Matrix_FromVectorsAsRows(
    Vector4d_New(1   , 0   , 0   , 0),
    Vector4d_New(0   , 1   , 0   , 0),
    Vector4d_New(0   , 0   , 1   , 0),
    Vector4d_New(1000, 1000, 1000, 1)
  );
  Matrix m = Matrix_Mul(
    win,
    Matrix_Mul(
      scale,
      translation
    )
  );

  Vector white = Vector_New(1,0,0);
  Vector black = Vector_New(0,1,0);

  int debug = 0;
  #define CHECKER(p) ((int)(p.x) + (int)(p.y) + (int)(p.z)) % 2
 
  for(long y = 0; y < height; y++) {
    for(long x = 0; x < width; x++) {
        
      Vector p = Vector2d_New(x,y);
      Vector pw = Vector_MulMatrix(p,m);

      Vector diff = Vector_New(
        pw.x - (long)pw.x, pw.y - (long)pw.y, pw.z - (long)pw.z
      );
      if(debug) Vector_Print(pw);
      diff.x = diff.x > .5 ? 1. - diff.x : diff.x;
      diff.y = diff.y > .5 ? 1. - diff.y : diff.y;
      diff.z = diff.z > .5 ? 1. - diff.z : diff.z;
      if(debug) Vector_Print(diff);
      // dont use z
      double t = MIN(diff.x,diff.y);
      if(debug) printf("%f\n",t);
 
      t = 2.0*t;
      t = 1; 
      Vector color; 
      if(CHECKER(pw)) {
        color = Vector_MulScalar(white,t);
      } else {
        color = Vector_MulScalar(black,t);
      }  
      
      BMP_PushRGB(&canvas,Vector_ToRGB(color));
    }
  }

  BMP_Save(&canvas,"checker.bmp");
  BMP_Free(&canvas);

  return 0;
}
