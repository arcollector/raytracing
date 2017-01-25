#ifndef QUADTREE_H
#define QUADTREE_H

#define QUADTREE_THRESHOLD 1
#define QUADTREE_TOPLEFT 0
#define QUADTREE_TOPRIGHT 1
#define QUADTREE_BOTTOMLEFT 2
#define QUADTREE_BOTTOMRIGHT 3
#define QUADTREE_MAXLEVEL 4

#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
#include "rgb.h"

typedef struct QuadtreeStruct {
  RGBl sum;
  RGB avg;
  RGB c1,c2,c3,c4;
  struct QuadtreeStruct *topLeft, *topRight, *bottomLeft, *bottomRight;
  int isLeaf;
} Quadtree;

Quadtree *Quadtree_New();
void Quadtree_NodeSumRGB(Quadtree *n);
void Quadtree_NodeAvgRGB(Quadtree *n);
int Quadtree_CmpRGB(RGB c1, RGB c2, int threshold);
void Quadtree_NodeToLeaf(Quadtree *node, Quadtree *parent);
void Quadtree_Print(Quadtree *n);
void Quadtree_GetSum(Quadtree *n, RGBl *sum, long *totalNodes);
RGB Quadtree_GetAvg(Quadtree *n);
void Quadtree_Free(Quadtree *n);

#endif
