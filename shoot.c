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


  // if stochastic or multisamplig
  if(scene->aa == AA_STOCHASTIC || scene->aa == AA_MULTI) {
    Quadtree *n = Quadtree_New();
    gbTotalNodes = 0;
    Shoot_Multi(
      x,y,1,n,1,
      scene->aa == AA_STOCHASTIC,
      cam,objList,bkgColor
    );
    pixel = Quadtree_GetAvg(n);
    Quadtree_Free(n);

  // else if single ray shooting
  } else {
    pixel = Shoot_Single(
      x,y,
      cam,objList,bkgColor
    );
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

void Shoot_Multi(
  double x, double y, double length,
  Quadtree *n, int level,
  int isStochastic,
  Camera cam,
  Object *objList,
  RGB bkgColor
) {

  double half = length / 2.;
  double half2 = half / 2.;
  double half32 = half * 3. / 2.;
  Ray ray;

  double r1,r2,r3,r4,r5,r6,r7,r8;
  if(isStochastic) {
    r1 = SRANDOM(-half2,half2);
    r2 = SRANDOM(-half2,half2);
    r3 = SRANDOM(-half2,half2);
    r4 = SRANDOM(-half2,half2);
    r5 = SRANDOM(-half2,half2);
    r6 = SRANDOM(-half2,half2);
    r7 = SRANDOM(-half2,half2);
    r8 = SRANDOM(-half2,half2);
  } else {
    r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = 0;
  }

  // compute topleft, topright, bottomleft and bottomright colors
  //printf("shoot %5.5f %5.5f\n", x+half2+ r1, y+half2+ r2);
  ray = Shoot_BuildRay(
    x+half2 + r1,
    y+half2 + r2,
    cam
  );
  n->c1 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half32+ r3, y+half2+ r4);
  ray = Shoot_BuildRay(
    x+half32 + r3,
    y+half2 + r4,
    cam
  );
  n->c2 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half2+ r5, y+half32+ r6);
  ray = Shoot_BuildRay(
    x+half2 + r5,
    y+half32 + r6,
    cam
  );
  n->c3 = Shade(ray, objList, bkgColor);
  //printf("shoot %5.5f %5.5f\n", x+half32+ r7, y+half32+ r8);
  ray = Shoot_BuildRay(
    x+half32 + r7,
    y+half32 + r8,
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
    Quadtree_NodeToLeaf(n,n);
    return;
  }

  Quadtree *aux;
  aux = n->children[QUADTREE_TOPLEFT] = Quadtree_New();
  // need more topleft refinement
  if(!c1) {
    Shoot_Multi(
      x,y,half,aux,level+1,isStochastic,
      cam,objList,bkgColor
    );
  } else {
    Quadtree_NodeToLeaf(n,aux);
  }
  // topright
  aux = n->children[QUADTREE_TOPRIGHT] = Quadtree_New();
  if(!c2) {
    Shoot_Multi(
      x+half,y,half,aux,level+1,isStochastic,
      cam,objList,bkgColor
    );
  } else {
    Quadtree_NodeToLeaf(n,aux);
  }
  // bottomleft
  aux = n->children[QUADTREE_BOTTOMLEFT] = Quadtree_New();
  if(!c3) {
    Shoot_Multi(
      x,y+half,half,aux,level+1,isStochastic,
      cam,objList,bkgColor
    );
  } else {
    Quadtree_NodeToLeaf(n,aux);
  }
  // bottomright
  aux = n->children[QUADTREE_BOTTOMRIGHT] = Quadtree_New();
  if(!c4) {
    Shoot_Multi(
      x+half,y+half,half,aux,level+1,isStochastic,
      cam,objList,bkgColor
    );
  } else {
    Quadtree_NodeToLeaf(n,aux);
  }

  gbTotalNodes+=4;
}