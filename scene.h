#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "texture.h"
#include "hit.h"
#include "lamp.h"
#include "object.h"
#include "sphere.h"
#include "plane.h"

#define AA_NONE 1
#define AA_MULTI 2
#define AA_STOCHASTIC 3

typedef struct {
  char *fileName;
  long width, height;
  Camera *cam;
  Object *objectList;
  long objectListLength;
  Texture *sky;
  int aa;
  Lamp *lampList;
  long lampListLength;
} Scene;

int Scene_Setup(FILE *fp, Scene **scene);
void Scene_Free(Scene *scene);
void Scene_Print(Scene *scene);

#endif
