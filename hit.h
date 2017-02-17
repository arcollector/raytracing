#ifndef HIT_H
#define HIT_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

#include "helpers.h"

typedef struct ObjectStruct Object;

typedef struct {
  double *t;
  int tLength;
  int start, next;
  int allPositive;
  Object *object;
} Hit;

#include "object.h"

Hit *Hit_New(int length, ...);
void Hit_Begin(Hit *hit);
double Hit_Next(Hit *hit);
int Hit_IsEnd(Hit *hit);
void Hit_SetObject(Object *object, Hit *hit);
void Hit_Free(Hit *hit);
void Hit_Print(Hit *hit);

#endif
