#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"
#include "datatypes.h"
#include "rgb.h"
#include "vector.h"

Texture *Texture_New();

Vector Texture_GetColorWithinLimit(double t, Texture *tex);
Vector Texture_GetColor(Vector p, Texture *tex);
RGB Texture_GetColorRGB(Vector p, Texture *tex);

void Texture_Free(Texture *tex);
void Texture_Print(Texture *tex);

double Texture_Spherical(Vector p, Texture *tex);

#endif
