#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"
#include "rgb.h"
#include "vector.h"
#include "matrix.h"

extern int gbDebug;

#define TEXTURE_MAX_LENGTH 10
#define TEXTURE_DEFAULT 0
#define TEXTURE_SKY 1
#define TEXTURE_SPHERICAL 2
#define TEXTURE_CHECKER 3

#define IOR_VACUUM 1.
#define IOR_AIR 1.00029
#define IOR_ICE 1.31
#define IOR_WATER 1.33
#define IOR_CROWN_GLASS 1.52
#define IOR_FLINT_GLASS 1.65
#define IOR_SAPPHIRE 1.77
#define IOR_DIAMOND 2.417

typedef struct {
  Vector color[16];
  double limit[16];
  long length;
  long type;
  Vector translate, scale;
  Matrix local, invLocal;
  double rfl, rfr, ior;
  double kA;
  double kS; long expS; int isMetallic;
  double minRadius, maxRadius;
} Texture;

Texture *Texture_New();
Texture *Texture_FromRGB(RGB color);
void Texture_Setup(Texture *tex);

void Texture_SetAmbient(double ambient, Texture *tex);
void Texture_SetPhong(
  double phong,
  long phongExp,
  int isMetallic,
  Texture *tex
);
void Texture_SetReflect(double rfl, Texture *tex);
void Texture_SetRefraction(double rfr, Texture *tex);
void Texture_SetIOR(double ior, Texture *tex);
void Texture_SetRadii(double minRadius, double maxRadius, Texture *tex);
void Texture_SetScale(Vector v, Texture *tex);

void Texture_AddColor(double t, Vector color, Texture *tex);
Vector Texture_GetColor(
  Vector p,
  Vector normal,
  Texture *tex
);
RGB Texture_GetColorRGB(
  Vector p,
  Vector normal,
  Texture *tex
);

void Texture_Free(Texture *tex);
void Texture_Print(Texture *tex);

#endif
