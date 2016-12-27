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

int PCX_IsPCX(FILE *fp) {

  fseek(fp, 0, SEEK_SET);

  char ch;
  ch = fgetc(fp); // at 0
  if(ch != 10) {
    printf("not a pcx file\n");
    return 0;
  }
  
  return 1;
}

int PCX_Read(FILE *fp, PCX_Header *header, BMP_Canvas *canvas) {

  char ch;
  long number;

  header->sig = 10;
  
  fseek(fp,1,SEEK_SET);
  ch = fgetc(fp); // at 1
  if(ch != 5) {
    printf("unsupported pcx version: %d\n",ch);
    return 0; 
  }
  header->version = 5;

  ch = fgetc(fp); // at 2
  if(ch != 1) {
    printf("Unsupported pcx compression method: %d\n",ch);
    return 0;
  }
  header->compressionType = 1;

  ch = fgetc(fp); // at 3
  header->bitsPerPixel = ch;
  
  number = fgetc(fp) | (fgetc(fp) << 8); // at 4
  header->rectLeft= number;
  number = fgetc(fp) | (fgetc(fp) << 8); // at 6
  header->rectTop = number;
  number = fgetc(fp) | (fgetc(fp) << 8); // at 8
  header->rectRight = number;
  number = fgetc(fp) | (fgetc(fp) << 8); // at 10
  header->rectBottom = number;

  number = fgetc(fp) | (fgetc(fp) << 8); // at 12
  header->wResPrint = number;
  number = fgetc(fp) | (fgetc(fp) << 8); // at 14
  header->hResPrint = number;

  for(int i = 0; i < 48;) { // at 16
    header->palette16[i++] = fgetc(fp);
    header->palette16[i++] = fgetc(fp);
    header->palette16[i++] = fgetc(fp);
  }

  ch = fgetc(fp); // at 64
  if(ch != 0) {
    printf("reserved1 (%d) must be zero\n",ch);
    return 0;
  }
  header->reserved1 = 0;

  ch = fgetc(fp);
  header->numPlanes = ch;
    
  number = fgetc(fp) | (fgetc(fp) << 8);
  header->bytesPerScanline = number;

  ch = fgetc(fp);
  header->paletteType = ch;

  ch = fgetc(fp); 
  if(ch != 0) {
    printf("reserved2 (%d) must be zero\n",ch);
    return 0;
  }
  header->reserved2 = 0;
  
  number = fgetc(fp) | (fgetc(fp) << 8);
  header->width = number;
  number = fgetc(fp) | (fgetc(fp) << 8);
  header->height = number;
  if(!header->width) {
    header->width = header->rectRight - header->rectLeft + 1;
  }
  if(!header->height) {
    header->height = header->rectBottom - header->rectTop + 1;
  }

  // 256 palette
  if(header->bitsPerPixel == 8 && header->numPlanes == 1) {
    fseek(fp,-769,SEEK_END);
    printf("seek: %ld\n",ftell(fp));
    ch = fgetc(fp);
    if(ch != 12) {
      printf("not palette information (%d)\n",ch);
      return 0;
    }
    for(int i = 0; i < 256; i++) {
      int r = fgetc(fp);
      int g = fgetc(fp);
      int b = fgetc(fp);
      RGB color = {r,g,b};
      header->palette256[i] = color;
    }
  }

  printf("======== PCX HEADER =======\n");
  printf("sig: %d\n", header->sig);
  printf("version: %d\n", header->version);
  printf("compressionType: %d\n", header->compressionType);
  printf("bitsPerPixel: %d\n", header->bitsPerPixel);
  printf("rect = (%d,%d),(%d,%d)\n", header->rectLeft,header->rectTop,header->rectRight,header->rectBottom);
  printf("printer = (%d,%d)\n", header->wResPrint,header->hResPrint);
  printf("16 palette map (if was necessary)\n");
  for(int i = 0; i < 48;) {
    char r = header->palette16[i++];
    char g = header->palette16[i++];
    char b = header->palette16[i++];
    printf("(%u,%u,%u),",r,g,b);
  }
  printf("\n");
  printf("reserved1: %d\n", header->reserved1);
  printf("numPlanes: %d\n", header->numPlanes);
  printf("bytesPerScanline: %d\n", header->bytesPerScanline);
  printf("paletteType: %d\n", header->paletteType);
  printf("width: %d\n", header->width);
  printf("height: %d\n", header->height);
  printf("256 palette map (if was necessary)\n");
  if(header->bitsPerPixel == 8 && header->numPlanes == 1) {
    for(int i = 0; i < 256; i++) {
      RGB color = header->palette256[i];
      printf("(%u,%u,%u),",color.red,color.green,color.blue);
    }
    printf("\n");
  }
  printf("======= END PCX HEADER ======\n");

  //printf("bytesPerScanline %d, width: %d\n",header->bytesPerScanline,header->width);
  int isDummyByte = header->bytesPerScanline != header->width;
  PCX_RawImage rawImage;
  rawImage.expectedLength = header->height *
                            header->width *
                            header->numPlanes;
  rawImage.data = malloc(rawImage.expectedLength);
  //printf("rawImage.expectedLength: %ld\n",rawImage.expectedLength);
  rawImage.length = 0;

  fseek(fp,128,SEEK_SET);
  int isAbort = 0;
  clock_t time1, time2;
  time1 = clock();
  for(long y = 0; !isAbort && y < header->height*header->numPlanes; y++) {
    for(long x = 0; !isAbort && x < header->bytesPerScanline; ) {
      char ch = fgetc(fp);
      long count = 1;
      if((ch & 0xc0) == 0xc0) {
        count = ch & 0x3f;
        ch = fgetc(fp);
      }
      for(long l = 0; l < count; l++) {
        if(isDummyByte && 
            (x+1) == header->bytesPerScanline && 
            (l+1) == count) {
          x++;
          break;
        }
        if((x+1) == header->bytesPerScanline &&
            (l+1) < count) {
          x = 0;
          continue;
        }
        rawImage.data[rawImage.length++] = ch; 
        if(rawImage.length == rawImage.expectedLength) {
          isAbort = 1;
          break;  
        }
        x++;
      }
    }
  }
  time2 = clock();
  double elapsed = (double)(time2 - time1) / CLOCKS_PER_SEC;
  printf("time elapsed: %f\n",elapsed);

  if(!BMP_NewCanvas(canvas,header->width,header->height)) {
    printf("fail to create bmp canvas\n");
    return 0;
  }

  if(header->bitsPerPixel == 8 && header->numPlanes == 1) {
    for(long i = 0; i < rawImage.length; i++) {
      unsigned char colorIndex = rawImage.data[i];
      RGB color = header->palette256[colorIndex];
      BMP_PushRGB(canvas,color);
    }

  // 24 bit color depth
  } else if(header->bitsPerPixel == 8 && header->numPlanes == 3) {
    for(long offset = 0; 
          offset < rawImage.length;
          offset+= (header->width*3)) {
      for(long x = 0; x < header->width; x++) {
        RGB color;
        color.red = rawImage.data[offset+x];
        color.green = rawImage.data[offset+x+header->width];
        color.blue = rawImage.data[offset+x+header->width*2];
        BMP_PushRGB(canvas,color);
      }  
    }
  } else {
    free(rawImage.data);
    printf("unsupported image color depth\n");
    return 0;
  }

  free(rawImage.data);

  return 1;
}

int PCX_Test(int argc,char *args[]) {

  if(argc != 2 ) {
    printf("missing pcx file arg\n");
    return 0;
  }

  char *fileName = args[1];
  FILE *fp = fopen(fileName,"rb");
  if(!fp) {
    printf("%s doest not exists\n",fileName);
    return 0;
  }

  PCX_Header header;
  if(!PCX_IsPCX(fp)) {
    return 0;
  }

  BMP_Canvas canvas;
  BMP_NewCanvas(&canvas,header.width,header.height);
  PCX_Read(fp,&header,&canvas);
  fclose(fp);

  BMP_Save(&canvas,"troloaz111o.bmp");
  BMP_Free(&canvas);

  return 0;
}
