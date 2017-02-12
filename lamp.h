#ifndef LAMP_H
#define LAMP_H

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "matrix.h"

typedef struct LampStruct {
  Vector position;
  struct LampStruct *next;
} Lamp;

Lamp *Lamp_New(Vector position);
void Lamp_Free(Lamp *lamp);
void Lamp_Print(Lamp *lamp);

#endif
