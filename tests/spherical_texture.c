#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../rgb.h"
#include "../bmp.h"
#include "../helpers.h"
#include "../datatypes.h"
#include "../vector.h"
#include "../matrix.h"
#include "../windowing.h"

double spherical(Vector p, Matrix m, double minRad, double maxRad);
double spherical2(Vector p, Matrix m, double minRad, double maxRad);

int main(int argc, char *args[]) {

  BMP_Canvas canvas;
  long width = 500;
  long height = 500;
  if(!BMP_NewCanvas(&canvas,width,height)) {
    printf("cannot create canvas\n");
    return 0;
  }

  Matrix m = Windowing_New(
    Vector_New(0,0,0), Vector_New(width,height,0),
    Vector_New(-10,-10,0), Vector_New(10,10,0),
    1
  );

  Vector color = Vector_New(1,0,0); // red
  double minRad = .6;
  double maxRad = .8;


  for(long x = 500; x < width; x++) {
    double t;
      t = spherical(Vector_New(x,500,0),m,minRad,maxRad);
      printf("x: %ld t: %f red: %ld\n\n",x,t, MIN(255,ROUND(1*t*255)));
  }
  //exit(0);

  for(long y = 0; y < height; y++) {
    //if(y != 500) continue;
    for(long x = 0; x < width; x++) {
      //if(x < 500) continue;
      double t;
      #if 1
      t = spherical(Vector_New(x,y,0),m,minRad,maxRad);
      Vector avgColor = Vector_MulScalar(color,t);
      #else
        #if 0
      t = spherical(Vector_New(x-1,y,0),m,minRad,maxRad);
      Vector newColor1 = Vector_MulScalar(color,t);
      t = spherical(Vector_New(x+1,y,0),m,minRad,maxRad);
      Vector newColor2 = Vector_MulScalar(color,t);
      t = spherical(Vector_New(x-1,y+1,0),m,minRad,maxRad);
      Vector newColor3 = Vector_MulScalar(color,t);
      t = spherical(Vector_New(x+1,y+1,0),m,minRad,maxRad);
      Vector newColor4 = Vector_MulScalar(color,t);
      t = spherical(Vector_New(x+.5,y+.5,0),m,minRad,maxRad);
      Vector newColor5 = Vector_MulScalar(color,t);
        #else
      t = spherical2(Vector_New(x-1,y,0),m,minRad,maxRad);
      Vector newColor1 = Vector_MulScalar(color,t);
      t = spherical2(Vector_New(x+1,y,0),m,minRad,maxRad);
      Vector newColor2 = Vector_MulScalar(color,t);
      t = spherical2(Vector_New(x-1,y+1,0),m,minRad,maxRad);
      Vector newColor3 = Vector_MulScalar(color,t);
      t = spherical2(Vector_New(x+1,y+1,0),m,minRad,maxRad);
      Vector newColor4 = Vector_MulScalar(color,t);
      t = spherical2(Vector_New(x+.5,y+.5,0),m,minRad,maxRad);
      Vector newColor5 = Vector_MulScalar(color,t);
        #endif
      Vector avgColor = Vector_DivScalar(
        Vector_AddVector(newColor5,
          Vector_AddVector(newColor4,
            Vector_AddVector(newColor3,
              Vector_AddVector(newColor1,newColor2)
            )
          )
        ),
        5
      );
      #endif

      BMP_PushRGB(&canvas,RGB_New(
        MIN(255,ROUND(avgColor.x*255)),
        MIN(255,ROUND(avgColor.y*255)),
        MIN(255,ROUND(avgColor.z*255))
        )
      );
    }
  }

  BMP_Save(&canvas,"spherical.bmp");
  BMP_Free(&canvas);

  return 0;
}

double spherical(Vector p, Matrix m, double minRad, double maxRad) {

  int debug = 0;
  if(debug) Vector_Print(p);
  p = Vector_MulMatrix(p,m);
  if(debug) Vector_Print(p);

  double distance = Vector_Length(p);

  double radius = minRad + maxRad;
  distance = fmod(distance, radius);
  if(debug) printf("distance (fmod?) is: %f\n",distance);

  if(distance < minRad * .25) {
    if(debug) printf(".5 to 1\n");
    return .5 + (1 - .5)*(distance/(minRad*.25));
  }

  if(distance < minRad * .75) {
    if(debug) printf("1\n");
    return 1;
  }

  if(distance < minRad) {
    if(debug) printf("1 to .5\n");
    return .5 + (1 - .5)*(-(distance - minRad*.75)/(minRad - minRad*.75)+1);
  }

  distance -= minRad;

  if(distance < maxRad * .25) {
    if(debug) printf(".5 to 0\n");
    return 0 + (.5 - 0)*(-(distance)/(maxRad*.25)+1);
  }

  if(distance < maxRad * .75) {
    if(debug) printf("0\n");
    return 0;
  }

  if(debug) printf("0 to .5\n");
  return 0 + (.5 - 0)*((distance - maxRad*.75)/(maxRad - maxRad *.75));

}

double spherical2(Vector p, Matrix m, double minRad, double maxRad) {

  p = Vector_MulMatrix(p,m);

  double distance = Vector_Length(p);
  //distance += minRad * .5;

  double radius = minRad + minRad;
  distance = fmod(distance, radius);

  if(distance < minRad * .25) {
    return .5 + 2*distance/minRad;
  }

  if(distance < minRad * .75) {
    return 1;
  }

  if(distance < minRad) {
    return 2.5 - 2*distance/minRad;
  }

  distance -= minRad;

  if(distance < radius * .25) {
    return .5 - 2*distance/radius;
  }

  if(distance < radius * .75) {
    return 0;
  }

  return 2*distance/radius - 1.5;

}
