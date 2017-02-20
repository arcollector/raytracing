#include "hit.h"

Hit *Hit_New(int length, ...) {

  Hit *hit = malloc(sizeof(Hit));
  if(!hit) return NULL;
  hit->t = calloc(length, sizeof(double));
  if(!hit->t) {
    free(hit);
    return NULL;
  }

  hit->next = -1;
  hit->tLength = length;
  hit->object = NULL;

  va_list argp;
  va_start(argp, length);
  for(int i = 0; i < length; i++) {
    hit->t[i] = va_arg(argp, double);
  }
  va_end(argp);

  return hit;
}

void Hit_Begin(Hit *hit) {
  hit->next = 0;
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
  printf(" next %d tLength %d\n",hit->next,hit->tLength);
  printf("object %p\n",hit->object);
  for(int i = 0; i < hit->tLength; i++) {
    printf("%d) t: %f ",i,hit->t[i]);
  }
  printf("\n");
}
