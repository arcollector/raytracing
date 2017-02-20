#ifndef CLIPPING_H
#define CLIPPING_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"
#include "vector.h"

extern int gbDebug;

typedef int (*Clip_Funct)(Vector, void *);
typedef void (*Clip_Print)(void *clipper);

#define CLIP_IN 1
#define CLIP_OUT 2

typedef struct {
  Vector center;
  Vector normal;
} ClipPlane;

typedef struct {
  Vector apex;
  double minRadius;
  Vector base;
  double maxRadius;
  Vector axis;
  double axisLength;
  double angle;
  int IN, OUT;
} ClipCone;

typedef struct {
  Vector apex;
  Vector base;
  int IN, OUT;
} ClipBox;

typedef struct {
  Vector center;
  double radius;
  int IN, OUT;
} ClipSphere;

typedef struct ClipListStruct {
  void *clipper;
  struct ClipListStruct *next;
  Clip_Funct clip;
  Clip_Print print;
} ClipList;

ClipList *ClipList_NewNode(
  void *clipper,
  Clip_Funct clipFunct,
  Clip_Print clipPrint
);
void ClipList_InsertNode(ClipList *node, ClipList **clipList);
void ClipList_Free(ClipList *clipList);
void Clipper_Free(void *clipper);

int Clip(Vector point, ClipList *clipList);


ClipPlane *ClipPlane_New(
  Vector center,
  Vector normal
);
int ClipPlane_Clip(Vector point, void *clipper);
void ClipPlane_Print(void *clipper);


int ClipCone_Clip(Vector point, void *clipper);
ClipCone *ClipCone_New(
  Vector apex,
  double minRadius,
  Vector base,
  double maxRadius,
  int clipType
);
void ClipCone_Print(void *clipper);


int ClipBox_Clip(Vector point, void *clipper);
ClipBox *ClipBox_New(
  Vector apex,
  Vector base,
  int clipType
);
void ClipBox_Print(void *clipper);


int ClipSphere_Clip(Vector point, void *clipper);
ClipSphere *ClipSphere_New(
  Vector center,
  double radius,
  int clipType
);
void ClipSphere_Print(void *clipper);

#endif
