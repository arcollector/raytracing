#include "shoot.h"

Ray Shoot_BuildRay(double x, double y, Camera cam) {

  // windowing
  Vector ps = Vector_New(x,y,0); // from screen space
  Vector pvp = Vector_MulMatrix(ps,cam.win); // to view plane space
  //Vector_Print(ps); Vector_Print(pvp); printf("\n");

  // build ray
  Ray ray = Ray_New(cam.viewerPos,pvp);
  //Ray_Print(ray); exit(0);

  return ray;
}

RGB Shoot(long x, long y, Scene *scene) {

  Camera cam = scene->cam;
  RGB pixel;
  Object *objList = scene->objList;
  RGB bkgColor = scene->bkgColor;


  // if stochastic
    // todo

  // else if single ray shooting
    // build ray
    Ray ray = Shoot_BuildRay(x+.5,y+.5,cam);
    pixel = Shade(ray, objList, bkgColor);

  return pixel;
}
