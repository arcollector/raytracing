#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"
#include "rgb.h"
#include "vector.h"

typedef struct {
  Vector color[16];
  double limit[16];
  long length;
  double minRadius, maxRadius; // used in spherical texturing
} Texture;

Texture *Texture_New();
Texture *Texture_FromRGB(RGB color);

void Texture_AddColor(double t, Vector color, Texture *tex);
Vector Texture_GetColorWithinLimit(double t, Texture *tex);
Vector Texture_GetColor(Vector p, Texture *tex);
RGB Texture_GetColorRGB(Vector p, Texture *tex);

void Texture_SetRadii(double minRadius, double maxRadius, Texture *tex);

void Texture_Free(Texture *tex);
void Texture_Print(Texture *tex);

double Texture_Spherical(Vector p, Texture *tex);

#endif
