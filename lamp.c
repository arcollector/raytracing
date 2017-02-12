#include "lamp.h"

Lamp *Lamp_New(Vector position) {
  
  Lamp *lamp = malloc(sizeof(Lamp));
  if(!lamp) return NULL;

  lamp->position = position;

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
}
