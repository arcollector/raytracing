#include "hit.h"

Hit *Hit_New(int length, ...) {

  Hit *hit = malloc(sizeof(Hit));
  if(!hit) return NULL;
  hit->t = calloc(length, sizeof(double));
  if(!hit->t) return NULL;

  hit->start = 0;
  hit->next = -1;
  hit->tLength = length;
  hit->object = NULL;
  hit->allPositive = 1;

  va_list argp;
  va_start(argp, length);
  int isStart = 0;
  for(int i = 0; i < length; i++) {
    double t = hit->t[i] = va_arg(argp, double);
    if(t < -EPSILON) hit->allPositive = 0;
    if(t >= -EPSILON && !isStart) {
      isStart = 1;
      hit->start = i;
    }
  }
  va_end(argp);

  return hit;
}

void Hit_Begin(Hit *hit) {
  hit->next = hit->start;
}

double Hit_Next(Hit *hit) {
  return (hit->next < 0 || Hit_IsEnd(hit)) ? -1 : hit->t[hit->next++];
}

int Hit_IsEnd(Hit *hit) {
  return hit->next == hit->tLength;
}

void Hit_SetObject(Object *object, Hit *hit) {
  hit->object = object;
}

void Hit_Free(Hit *hit) {
  free(hit->t);
  free(hit);
}

void Hit_Print(Hit *hit) {
  printf("=== HIT ===\n");
  printf("start %d next %d tLength %d\n",hit->start,hit->next,hit->tLength);
  printf("are t all positive %d\n",hit->allPositive);
  printf("object %p\n",hit->object);
  for(int i = 0; i < hit->tLength; i++) {
    printf("%d) t: %f ",i,hit->t[i]);
  }
  printf("\n");
}
