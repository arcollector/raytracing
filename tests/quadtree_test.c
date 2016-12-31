#include <stdio.h>
#include <stdlib.h>

#include "../quadtree.h"

long gbTotalNodes = 0;
long gbIteractions = 0;

void trace(
  double x, double y, double length,
  Quadtree *n,
  int level
) {

  gbIteractions++;

  double half = length / 2.;
  double half2 = half / 2.;
  double half32 = half * 3. / 2.;

  // compute topleft, topright, bottomleft and bottomright colors
  //printf("shoot %5.5f %5.5f\n", x+half2, y+half2);
  n->c1 = (RGB_New(255,255,255));
  //printf("shoot %5.5f %5.5f\n", x+half32, y+half2);
  n->c2 = (RGB_New(240,250,250));
  //printf("shoot %5.5f %5.5f\n", x+half2, y+half32);
  n->c3 = (RGB_New(200,220,255));
  //printf("shoot %5.5f %5.5f\n", x+half32, y+half32);
  n->c4 = (RGB_New(200,142,255));
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
  //RGB_Print(n->avg);
  //exit(0);

  // not much difference between avg and corners colors
  int c1 = RGB_Equals(n->avg,n->c1,QUADTREE_THRESHOLD);
  int c2 = RGB_Equals(n->avg,n->c2,QUADTREE_THRESHOLD);
  int c3 = RGB_Equals(n->avg,n->c3,QUADTREE_THRESHOLD);
  int c4 = RGB_Equals(n->avg,n->c4,QUADTREE_THRESHOLD);
  //printf("%d %d %d %d\n",c1,c2,c3,c4);
  if((c1 && c2 && c3 && c4) || level == QUADTREE_MAXLEVEL) {
    // enough
    gbTotalNodes++;
    n->isLeaf = 1;
    return;
  }

  Quadtree *aux;
  // need more topleft refinement
  if(!c1) {
    aux = n->children[QUADTREE_TOPLEFT] = Quadtree_New();
    trace(x,y,half,aux,level+1);
  }
  // topright
  if(!c2) {
    aux = n->children[QUADTREE_TOPRIGHT] = Quadtree_New();
    trace(x+half,y,half,aux,level+1);
  }
  // bottomleft
  if(!c3) {
    aux = n->children[QUADTREE_BOTTOMLEFT] = Quadtree_New();
    trace(x,y+half,half,aux,level+1);
  }
  // bottomright
  if(!c4) {
    aux = n->children[QUADTREE_BOTTOMRIGHT] = Quadtree_New();
    trace(x+half,y+half,half,aux,level+1);
  }

}

int main(int argc, char *argv[]) {

  Quadtree *n = Quadtree_New();
  trace(0,0,1,n,1);
  printf("total nodes are %ld\n", gbTotalNodes);
  printf("total iteractionss %ld\n", gbIteractions);
  //Quadtree_Print(n);

  RGB avg = Quadtree_GetAvg(n);
  printf("avg: "); RGB_Print(avg);

  Quadtree_Free(n);

  return 0;
}
