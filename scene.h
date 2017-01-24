#ifndef SCENE_H
#define SCENE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "datatypes.h"
#include "rgb.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"
#include "sphere.h"
#include "plane.h"
#include "texture.h"

#define AA_NONE 1
#define AA_MULTI 2
#define AA_STOCHASTIC 3

// helpers
int Scene_GetString(FILE *fp);
Vector Scene_ParseVector(FILE *fp);
double Scene_ParseFloat(FILE *fp);
int Scene_GetTexture(FILE *fp, Texture *tex);
Object *Scene_AddBlankObject(Scene *scene);

Scene *Scene_New();
void Scene_Free(Scene *scene);
int Scene_Setup(FILE *fp, Scene *scene);
void Scene_Print(Scene *scene);

int Scene_GetAntiAliasing(FILE *fp, Scene *scene);
int Scene_GetCamera(FILE *fp, Scene *scene);
int Scene_GetSky(FILE *fp, Scene *scene);
int Scene_GetSphere(FILE *fp, Scene *scene);
int Scene_GetPlane(FILE *fp, Scene *scene);

#endif
