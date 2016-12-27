#ifndef PCX_H
#define PCX_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "rgb.h"
#include "bmp.h"

struct PCX_HeaderStruct {
  uint8_t sig;  
  uint8_t version;
  uint8_t compressionType;
  uint8_t bitsPerPixel;
  uint16_t rectTop;
  uint16_t rectLeft;
  uint16_t rectBottom;
  uint16_t rectRight;
  uint16_t wResPrint;
  uint16_t hResPrint;
  uint8_t palette16[48];
  uint8_t reserved1;
  uint8_t numPlanes;
  uint16_t bytesPerScanline;
  uint8_t paletteType;
  uint8_t reserved2;
  uint16_t width;
  uint16_t height;
  uint8_t notUsed[54];
  RGB palette256[256];
};

typedef struct PCX_HeaderStruct PCX_Header;

struct PCX_RawImageStruct {
  long length, expectedLength;
  char *data;
};

typedef struct PCX_RawImageStruct PCX_RawImage;

int PCX_IsPCX(FILE *fp);
int PCX_Read(FILE *fp, PCX_Header *header, BMP_Canvas *canvas);

#endif
