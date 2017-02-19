#include "clip.h"

ClipList *ClipList_NewNode(
  void *clipper,
  Clip_Funct clipFunct,
  Clip_Print clipPrint
) {
  ClipList *clipList = malloc(sizeof(ClipList));
  if(!clipList) return NULL;
  clipList->next = NULL;
  clipList->clipper = clipper;
  clipList->clip = clipFunct;
  clipList->print = clipPrint;
  return clipList;
}

void ClipList_InsertNode(ClipList *node, ClipList **clipList) {
  if(!*clipList) {
    *clipList = node;
  } else {
    node->next = *clipList;
    *clipList = node;
  }
}

void ClipList_Free(ClipList *clipList) {
  for(; clipList;) {
    ClipList *tmp = clipList->next;
    Clipper_Free(clipList->clipper);
    free(clipList);
    clipList = tmp;
  }
}

void Clipper_Free(void *clipper) {
  if(clipper) free(clipper);
}

int Clip(Vector point, ClipList *clipList) {
  for(; clipList; clipList = clipList->next) {
    if(!(*clipList->clip)(point, clipList->clipper)) return 0;
  }
  return 1;
}

/*
 * Save all parts that in the same side where
 * plane normal is pointing
 */
int ClipPlane_Clip(Vector point, void *_clipper) {
  ClipPlane *clipper = _clipper;
  return Vector_Dot(
    clipper->normal,
    Vector_FromP1toP2(clipper->center, point)
  ) >= -EPSILON;
}

ClipPlane *ClipPlane_New(Vector center, Vector normal) {
  ClipPlane *clipper = malloc(sizeof(ClipPlane));
  if(!clipper) return NULL;
  clipper->normal = normal;
  clipper->center = center;
  return clipper;
}

void ClipPlane_Print(void *_clipper) {
  ClipPlane *clipper = _clipper;
  printf("==== CLIP PLANE ====\n");
  printf("center :"); Vector_Print(clipper->center);
  printf("normal :"); Vector_Print(clipper->normal);
}

/*
 *  Save all parts that are inside cone
 */
static int _ClipCone_Clip(Vector point, ClipCone *clipper) {
  Vector v = Vector_FromP1toP2(clipper->apex, point);
  double dist = Vector_Dot(v, clipper->axis);
  if(dist < 0 || dist > clipper->axisLength) return 0;
  Vector vProj = Vector_MulScalar(clipper->axis, dist);
  Vector u = Vector_FromP1toP2(vProj, v);
  double uLength = Vector_Length(u);
  return uLength <= clipper->minRadius ||
         atan2(uLength - clipper->minRadius, dist) <= clipper->angle;
}

int ClipCone_Clip(Vector point, void *_clipper) {
  ClipCone *clipper = _clipper;
  int res = _ClipCone_Clip(point,clipper);
  return clipper->IN && res ? 1 : clipper->OUT && !res ? 1 : 0;
}

ClipCone *ClipCone_New(
  Vector apex,
  double minRadius,
  Vector base,
  double maxRadius,
  int clipType
) {
  ClipCone *clipper = malloc(sizeof(ClipCone));
  if(!clipper) return NULL;
  clipper->apex = apex;
  clipper->minRadius = minRadius;
  clipper->base = base;
  clipper->maxRadius = maxRadius;
  Vector axis = Vector_FromP1toP2(apex,base);
  clipper->axisLength = Vector_Length(axis);
  clipper->axis = Vector_DivScalar(axis,clipper->axisLength);
  clipper->angle = atan2(maxRadius - minRadius, clipper->axisLength);
  clipper->IN = clipType == CLIP_IN;
  clipper->OUT = clipType == CLIP_OUT;
  return clipper;
}

void ClipCone_Print(void *_clipper) {
  ClipCone *clipper = _clipper;
  printf("==== CLIP CONE ====\n");
  printf("base: "); Vector_Print(clipper->base);
  printf("apex: "); Vector_Print(clipper->apex);
  printf("axisLength: %f ",clipper->axisLength);
  printf("axis: "); Vector_Print(clipper->axis);
  printf("minRadius: %f maxRadius: %f\n",clipper->minRadius,clipper->maxRadius);
  printf("angle: %f\n",clipper->angle);
  printf("IN %d OUT %d\n",clipper->IN,clipper->OUT);
}

/*
 * Keep all parts that are inside the box
 */
static int _ClipBox_Clip(Vector point, ClipBox *clipper) {
  return point.x >= clipper->base.x &&
         point.x <= clipper->apex.x &&
         point.y >= clipper->base.y &&
         point.y <= clipper->apex.y &&
         point.z >= clipper->base.z &&
         point.z <= clipper->apex.z;
}

int ClipBox_Clip(Vector point, void *_clipper) {
  ClipBox *clipper = _clipper;
  int res = _ClipBox_Clip(point,clipper);
  return clipper->IN && res ? 1 : clipper->OUT && !res ? 1 : 0;
}

ClipBox *ClipBox_New(
  Vector apex,
  Vector base,
  int clipType
) {
  ClipBox *clipper = malloc(sizeof(ClipBox));
  if(!clipper) return NULL;
  clipper->apex = apex;
  clipper->base = base;
  clipper->IN = clipType == CLIP_IN;
  clipper->OUT = clipType == CLIP_OUT;
  return clipper;
}

void ClipBox_Print(void *_clipper) {
  ClipBox *clipper = _clipper;
  printf("==== CLIP BOX ====\n");
  printf("base: "); Vector_Print(clipper->base);
  printf("apex: "); Vector_Print(clipper->apex);
  printf("IN %d OUT %d\n",clipper->IN,clipper->OUT);
}

/*
 * Keep all parts that are inside the sphere
 */
static int _ClipSphere_Clip(Vector point, ClipSphere *clipper) {
  return Vector_Length(
    Vector_FromP1toP2(clipper->center, point)
  ) <= clipper->radius;
}

int ClipSphere_Clip(Vector point, void *_clipper) {
  ClipSphere *clipper = _clipper;
  int res = _ClipSphere_Clip(point,clipper);
  return clipper->IN && res ? 1 : clipper->OUT && !res ? 1 : 0;
}

ClipSphere *ClipSphere_New(
  Vector center,
  double radius,
  int clipType
) {
  ClipSphere *clipper = malloc(sizeof(ClipSphere));
  if(!clipper) return NULL;
  clipper->center = center;
  clipper->radius = radius;
  clipper->IN = clipType == CLIP_IN;
  clipper->OUT = clipType == CLIP_OUT;
  return clipper;
}

void ClipSphere_Print(void *_clipper) {
  ClipSphere *clipper = _clipper;
  printf("==== CLIP SPHERE ====\n");
  printf("center: "); Vector_Print(clipper->center);
  printf("radus: %f\n", clipper->radius);
  printf("IN %d OUT %d\n",clipper->IN,clipper->OUT);
}
