#include "lamp.h"

Lamp *Lamp_New(Vector position, double strength, double falloff) {

  Lamp *lamp = malloc(sizeof(Lamp));
  if(!lamp) return NULL;

  lamp->position = position;
  lamp->strength = strength;
  lamp->falloff = falloff;

  lamp->next = NULL;

  return lamp;
}

void Lamp_Free(Lamp *lamp) {
  for(; lamp; ) {
    Lamp *tmp = lamp->next;
    free(lamp);
    lamp = tmp;
  }
}

void Lamp_Print(Lamp *lamp) {
  printf("==== LAMP ====\n");
  printf("position: "); Vector_Print(lamp->position);
  printf("strength: %f falloff: %f\n", lamp->strength, lamp->falloff);
}
