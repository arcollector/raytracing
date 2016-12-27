#ifndef TGA_H
#define TGA_H

#include <stdio.h>
#include <string.h>

#include "rgb.h"
#include "bmp.h"

#if !defined(TGA_DEBUG)
  #define TGA_DEBUG 1
#endif

int TGA_IsTGA(FILE *fp);
int TGA_Read(FILE *fp, BMP_Canvas *canvas);

#endif
