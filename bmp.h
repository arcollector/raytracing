#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rgb.h"

#if !defined(BMP_DEBUG)
  #define BMP_DEBUG 1 
#endif

#define BMP_HEADER_SIZE 40
#define BMP_24BITS_ADDRESS 54
#define BMP_IMAGE_PLANES 1
#define BMP_COLOR_DEPTH_24BITS 24
#define BMP_NO_COMPRESSION 0

struct BMP_HeaderStruct {
  uint8_t B,M;
  uint32_t fileSize;
  uint16_t reserved0;
  uint16_t reserved1;
  uint32_t canvasAddress;
  uint32_t headerSize;
  uint32_t width;
  uint32_t height;
  uint16_t imagePlanes;
  uint16_t colorDepth;
  uint32_t compressionType;
  uint32_t canvasSize; 
  uint32_t ignore1; 
  uint32_t ignore2; 
  uint32_t ignore3; 
  uint32_t ignore4; 
};

typedef struct BMP_HeaderStruct BMP_Header;

struct BMP_CanvasStruct {
  long width, height;
  long length, expectedLength;
  RGB *data;
};

typedef struct BMP_CanvasStruct BMP_Canvas;

int BMP_NewCanvas( BMP_Canvas *canvas, long width, long height );
int BMP_IsBMP( FILE *fp );
int BMP_Read( FILE *fp, BMP_Header *header, BMP_Canvas *canvas );
int BMP_PushRGB( BMP_Canvas *canvas, RGB color );
int BMP_Save( BMP_Canvas *canvas, char *filename );
void BMP_Free( BMP_Canvas *canvas );

#endif

