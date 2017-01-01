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
  if(scene->shootType == SCENE_STOCHASTIC) {
    // todo
    Quadtree *n = Quadtree_New();
    gbTotalNodes = 0;
    Shoot_Stochastic(x,y,1,n,1,cam,objList,bkgColor);
    pixel = Quadtree_GetAvg(n);
    Quadtree_Free(n);

  // else if single ray shooting
  } else {
    pixel = Shoot_Single(x,y,cam,objList,bkgColor);
  }

  return pixel;
}

RGB Shoot_Single(
  double x, double y,
  Camera cam,
  Object *objList,
  RGB bkgColor
) {
  // build ray
  Ray ray = Shoot_BuildRay(x+.5,y+.5,cam);
  RGB pixel = Shade(ray, objList, bkgColor);
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

  #undef RANDOM
  #define RANDOM(x,y) 0
  // compute topleft, topright, bottomleft and bottomright colors
  //printf("shoot %5.5f %5.5f\n", x+half2+ RANDOM(-half2,half2), y+half2+ RANDOM(-half2,half2));
  ray = Shoot_BuildRay(
    x+half2 + RANDOM(-half2,half2),
    y+half2 + RANDOM(-half2,half2),
    cam
  );
  n->c1 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half32+ RANDOM(-half2,half2), y+half2+ RANDOM(-half2,half2));
  ray = Shoot_BuildRay(
    x+half32 + RANDOM(-half2,half2),
    y+half2 + RANDOM(-half2,half2),
    cam
  );
  n->c2 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half2+ RANDOM(-half2,half2), y+half32+ RANDOM(-half2,half2));
  ray = Shoot_BuildRay(
    x+half2 + RANDOM(-half2,half2),
    y+half32 + RANDOM(-half2,half2),
    cam
  );
  n->c3 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half32+ RANDOM(-half2,half2), y+half32+ RANDOM(-half2,half2));
  ray = Shoot_BuildRay(
    x+half32 + RANDOM(-half2,half2),
    y+half32 + RANDOM(-half2,half2),
    cam
  );
  n->c4 = Shade(ray, objList, bkgColor);
  //printf("\n");
  //exit(0);

  // compute avg color
  Quadtree_NodeAvgRGB(n);

  // not much difference between avg and corners colors
  int c1 = Quadtree_CmpRGB(n->avg,n->c1,QUADTREE_THRESHOLD);
  int c2 = Quadtree_CmpRGB(n->avg,n->c2,QUADTREE_THRESHOLD);
  int c3 = Quadtree_CmpRGB(n->avg,n->c3,QUADTREE_THRESHOLD);
  int c4 = Quadtree_CmpRGB(n->avg,n->c4,QUADTREE_THRESHOLD);

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
