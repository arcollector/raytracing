#include "quadtree.h"

Quadtree *Quadtree_New() {

  Quadtree *n = malloc(sizeof(Quadtree));
  n->isLeaf = 0;

  n->children[QUADTREE_TOPLEFT] = NULL;
  n->children[QUADTREE_TOPRIGHT] = NULL;
  n->children[QUADTREE_BOTTOMLEFT] = NULL;
  n->children[QUADTREE_BOTTOMRIGHT] = NULL;

  return n;
}

void Quadtree_Print(Quadtree *n) {

  printf("== QUADTREE ====\n");
  if(!n) {
    return;
  }

  long totalNodes = 0;

  struct Stack {
    Quadtree *nodes[4*64];
    long length;
    long currentIndex;
  } stack;

  stack.nodes[0] = n;
  stack.length = 1;
  stack.currentIndex = 0;
  int level = 1;
  int levelNodes = 1;
  totalNodes++;

  printf("=== level %d ====\n", level);
  while(stack.currentIndex < stack.length) {

    Quadtree *n = stack.nodes[stack.currentIndex++];

    RGB_Print(n->c1);
    RGB_Print(n->c2);
    RGB_Print(n->c3);
    RGB_Print(n->c4);
    RGB_Print(n->avg);

    levelNodes--;
    if(!levelNodes) {
      level++;
      printf("=== level %d ====\n", level);
      levelNodes = pow(4,level);
    }

    if(n->children[QUADTREE_TOPLEFT]) {
      stack.nodes[stack.length++] = n->children[QUADTREE_TOPLEFT];
      totalNodes++;
    }
    if(n->children[QUADTREE_TOPRIGHT]) {
      stack.nodes[stack.length++] = n->children[QUADTREE_TOPLEFT];
      totalNodes++;
    }
    if(n->children[QUADTREE_BOTTOMLEFT]) {
      stack.nodes[stack.length++] = n->children[QUADTREE_TOPLEFT];
      totalNodes++;
    }
    if(n->children[QUADTREE_BOTTOMRIGHT]) {
      stack.nodes[stack.length++] = n->children[QUADTREE_TOPLEFT];
      totalNodes++;
    }

  }

  printf("total nodes are %ld\n", totalNodes);
}

void Quadtree_GetSum(Quadtree *n, RGBl *sum, long *totalNodes) {

  if(!n) {
    return;
  }

  if(n->isLeaf) {
    (*totalNodes)++;
    sum->red += n->sum.red;
    sum->green += n->sum.green;
    sum->blue += n->sum.blue;
    return;
  }

  if(n->children[QUADTREE_TOPLEFT]) {
    Quadtree_GetSum(n->children[QUADTREE_TOPLEFT],sum,totalNodes);
  }
  if(n->children[QUADTREE_TOPRIGHT]) {
    Quadtree_GetSum(n->children[QUADTREE_TOPRIGHT],sum,totalNodes);
  }
  if(n->children[QUADTREE_BOTTOMLEFT]) {
    Quadtree_GetSum(n->children[QUADTREE_BOTTOMLEFT],sum,totalNodes);
  }
  if(n->children[QUADTREE_BOTTOMRIGHT]) {
    Quadtree_GetSum(n->children[QUADTREE_BOTTOMRIGHT],sum,totalNodes);
  }

}

RGB Quadtree_GetAvg(Quadtree *n) {

  RGBl sum = RGBl_New(0,0,0);
  long totalNodes = 0;
  Quadtree_GetSum(n,&sum,&totalNodes);

  RGB avg = RGB_New(0,0,0);
  avg.red = MIN(255,ROUND(sum.red / (double)totalNodes / 4.));
  avg.green = MIN(255,ROUND(sum.green / (double)totalNodes / 4.));
  avg.blue = MIN(255,ROUND(sum.blue / (double)totalNodes / 4.));

  return avg;
}

void Quadtree_Free(Quadtree *n) {

  if(!n) {
    return;
  }

  if(n->isLeaf) {
    free(n);
    return;
  }

  if(n->children[QUADTREE_TOPLEFT]) {
    Quadtree_Free(n->children[QUADTREE_TOPLEFT]);
  }
  if(n->children[QUADTREE_TOPRIGHT]) {
    Quadtree_Free(n->children[QUADTREE_TOPRIGHT]);
  }
  if(n->children[QUADTREE_BOTTOMLEFT]) {
    Quadtree_Free(n->children[QUADTREE_BOTTOMLEFT]);
  }
  if(n->children[QUADTREE_BOTTOMRIGHT]) {
    Quadtree_Free(n->children[QUADTREE_BOTTOMRIGHT]);
  }

  free(n);
}
