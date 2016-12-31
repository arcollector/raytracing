#ifndef RGB_H
#define RGB_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "helpers.h"

typedef struct {
  unsigned int red, green, blue;
} RGB;

typedef struct {
  long red, green, blue;
} RGBl;

typedef struct {
  double red, green, blue;
} RGBf;

RGB RGB_New(unsigned int red, unsigned int green, unsigned int blue);
RGBl RGBl_New(long red, long green, long blue);
RGBf RGBf_New(double red, double green, double blue);
RGBf RGB_toRGBf(RGB rgb);
RGB RGBf_toRGB(RGBf rgb);
int RGB_Equals(RGB c1, RGB c2, int threshold);
void RGB_Print(RGB c);
void RGBl_Print(RGBl c);
void RGBf_Print(RGBf c);

#endif
