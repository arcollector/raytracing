#ifndef LAMP_H
#define LAMP_H

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "matrix.h"

#define LAMP_STRENGTH_DEFAULT 5
#define LAMP_FALLOFF_DEFAULT .5

typedef struct LampStruct {
  Vector position;
  double strength, falloff;
  struct LampStruct *next;
} Lamp;

Lamp *Lamp_New(Vector position, double strength, double falloff);
void Lamp_Free(Lamp *lamp);
void Lamp_Print(Lamp *lamp);

#endif
