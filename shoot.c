#include "shoot.h"

long gbTotalNodes = 0;

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
  if(1) {
    // todo
    Quadtree *n = Quadtree_New();
    gbTotalNodes = 0;
    Shoot_Stochastic(x,y,1,n,1,cam,objList,bkgColor);
    pixel = Quadtree_GetAvg(n);
    Quadtree_Free(n);

  // else if single ray shooting
  } else {
    // build ray
    Ray ray = Shoot_BuildRay(x+.5,y+.5,cam);
    pixel = Shade(ray, objList, bkgColor);
  }

  return pixel;
}

void Shoot_Stochastic(
  double x, double y, double length,
  Quadtree *n, int level,
  Camera cam,
  Object *objList,
  RGB bkgColor
) {

  double half = length / 2.;
  double half2 = half / 2.;
  double half32 = half * 3. / 2.;
  Ray ray;

  // compute topleft, topright, bottomleft and bottomright colors
  //printf("shoot %5.5f %5.5f\n", x+half2, y+half2);
  ray = Shoot_BuildRay(x+half2,y+half2,cam);
  n->c1 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half32, y+half2);
  ray = Shoot_BuildRay(x+half32,y+half2,cam);
  n->c2 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half2, y+half32);
  ray = Shoot_BuildRay(x+half2,y+half32,cam);
  n->c3 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half32, y+half32);
  ray = Shoot_BuildRay(x+half32,y+half32,cam);
  n->c4 = Shade(ray, objList, bkgColor);
  //printf("\n");

  // compute avg color
  long red = n->c1.red + n->c2.red + n->c3.red + n->c4.red;
  long green = n->c1.green + n->c2.green + n->c3.green + n->c4.green;
  long blue = n->c1.blue + n->c2.blue + n->c3.blue + n->c4.blue;
  n->sum = RGBl_New(red, green, blue);
  n->avg = RGB_New(
    MIN(255,ROUND(n->sum.red/4.)),
    MIN(255,ROUND(n->sum.green/4.)),
    MIN(255,ROUND(n->sum.blue/4.))
  );

  // not much difference between avg and corners colors
  int c1 = RGB_Equals(n->avg,n->c1,QUADTREE_THRESHOLD);
  int c2 = RGB_Equals(n->avg,n->c2,QUADTREE_THRESHOLD);
  int c3 = RGB_Equals(n->avg,n->c3,QUADTREE_THRESHOLD);
  int c4 = RGB_Equals(n->avg,n->c4,QUADTREE_THRESHOLD);

  if((c1 && c2 && c3 && c4) || level == QUADTREE_MAXLEVEL) {
    // enough
    n->isLeaf = 1;
    return;
  }

  Quadtree *aux;
  // need more topleft refinement
  if(!c1) {
    aux = n->children[QUADTREE_TOPLEFT] = Quadtree_New();
    gbTotalNodes++;
    Shoot_Stochastic(
      x,y,half,aux,level+1,
      cam,objList,bkgColor
    );
  }
  // topright
  if(!c2) {
    aux = n->children[QUADTREE_TOPRIGHT] = Quadtree_New();
    gbTotalNodes++;
    Shoot_Stochastic(
      x+half,y,half,aux,level+1,
      cam,objList,bkgColor
    );
  }
  // bottomleft
  if(!c3) {
    aux = n->children[QUADTREE_BOTTOMLEFT] = Quadtree_New();
    gbTotalNodes++;
    Shoot_Stochastic(
      x,y+half,half,aux,level+1,
      cam,objList,bkgColor
    );
  }
  // bottomright
  if(!c4) {
    aux = n->children[QUADTREE_BOTTOMRIGHT] = Quadtree_New();
    gbTotalNodes++;
    Shoot_Stochastic(
      x+half,y+half,half,aux,level+1,
      cam,objList,bkgColor
    );
  }

}
