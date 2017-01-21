#include "quadtree.h"

Quadtree *Quadtree_New() {

  Quadtree *n = malloc(sizeof(Quadtree));
  n->isLeaf = 0;

  n->topLeft = NULL;
  n->topRight = NULL;
  n->bottomLeft = NULL;
  n->bottomRight = NULL;

  return n;
}

void Quadtree_NodeToLeaf(Quadtree *node, Quadtree *parent) {
  parent->isLeaf = 1;
  if(node == parent) return;
  parent->c1 = node->c1;
  parent->c2 = node->c2;
  parent->c3 = node->c3;
  parent->c4 = node->c4;
  parent->sum = node->sum;
  parent->avg = node->avg;
}

int Quadtree_CmpRGB(RGB c1, RGB c2, int threshold) {
  RGB cmp = RGB_New(
    abs(c1.red - c2.red),
    abs(c1.green - c2.green),
    abs(c1.blue - c2.blue)
  );
  return cmp.red <= threshold &&
        cmp.blue <= threshold &&
        cmp.green <= threshold;
}

void Quadtree_NodeSumRGB(Quadtree *n) {
  long red = n->c1.red + n->c2.red + n->c3.red + n->c4.red;
  long green = n->c1.green + n->c2.green + n->c3.green + n->c4.green;
  long blue = n->c1.blue + n->c2.blue + n->c3.blue + n->c4.blue;
  n->sum = RGBl_New(red, green, blue);
}

void Quadtree_NodeAvgRGB(Quadtree *n) {
  Quadtree_NodeSumRGB(n);
  n->avg = RGB_New(
    MIN(255,ROUND(n->sum.red/4.)),
    MIN(255,ROUND(n->sum.green/4.)),
    MIN(255,ROUND(n->sum.blue/4.))
  );
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
    RGBl_Print(n->sum);
    RGB_Print(n->avg);

    levelNodes--;
    if(!levelNodes) {
      level++;
      printf("=== level %d ====\n", level);
      levelNodes = pow(4,level);
    }

    if(n->topLeft) {
      stack.nodes[stack.length++] = n->topLeft;
      totalNodes++;
    }
    if(n->topRight) {
      stack.nodes[stack.length++] = n->topRight;
      totalNodes++;
    }
    if(n->bottomLeft) {
      stack.nodes[stack.length++] = n->bottomLeft;
      totalNodes++;
    }
    if(n->bottomRight) {
      stack.nodes[stack.length++] = n->bottomRight;
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

  if(n->topLeft) {
    Quadtree_GetSum(n->topLeft,sum,totalNodes);
  }
  if(n->topRight) {
    Quadtree_GetSum(n->topRight,sum,totalNodes);
  }
  if(n->bottomLeft) {
    Quadtree_GetSum(n->bottomLeft,sum,totalNodes);
  }
  if(n->bottomRight) {
    Quadtree_GetSum(n->bottomRight,sum,totalNodes);
  }

}

RGB Quadtree_GetAvg(Quadtree *n) {

  RGBl sum = RGBl_New(0,0,0);
  long totalNodes = 0;
  Quadtree_GetSum(n,&sum,&totalNodes);

  RGB avg = RGB_New(0,0,0);
  double deno = totalNodes * 4.;
  // avg = color / totalNodes / 4
  avg.red = MIN(255,ROUND(sum.red / deno));
  avg.green = MIN(255,ROUND(sum.green / deno));
  avg.blue = MIN(255,ROUND(sum.blue / deno));

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

  if(n->topLeft) {
    Quadtree_Free(n->topLeft);
  }
  if(n->topRight) {
    Quadtree_Free(n->topRight);
  }
  if(n->bottomLeft) {
    Quadtree_Free(n->bottomLeft);
  }
  if(n->bottomRight) {
    Quadtree_Free(n->bottomRight);
  }

  free(n);
}
