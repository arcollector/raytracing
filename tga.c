#include "tga.h"

int TGA_IsTGA( FILE *fp ) {

  int returnValue = 1;
  long seek = ftell(fp);

  fseek(fp, 2, SEEK_SET);
  if(fgetc(fp) != 2) {
#if TGA_DEBUG == 1
    printf("2 error\n");
#endif
    returnValue = 0;
  }

  fseek(fp, 16, SEEK_SET);
  if(fgetc(fp) != 24) {
#if TGA_DEBUG == 1
    printf("24 error\n");
    returnValue = 0;
#endif
  }
  
  fseek(fp, 17, SEEK_SET);
  if(fgetc(fp) != 32) {
#if TGA_DEBUG == 1
    printf("32 error\n");
#endif
    returnValue = 0;
  }

  fseek( fp, seek, SEEK_SET );

  return returnValue;
}

int TGA_Read(FILE *fp, BMP_Canvas *canvas) {

  fseek(fp, 12, SEEK_SET);
  long width = (fgetc(fp)) | (fgetc(fp) << 8);
  long height = (fgetc(fp)) | (fgetc(fp) << 8);

  // use BMP canvas struct
  BMP_NewCanvas( canvas, width, height );

  fseek(fp, 18, SEEK_SET);
  while(1) {
    char blue = fgetc(fp);
    char green = fgetc(fp);
    char red = fgetc(fp);
    if(feof(fp)) {
      break;
    }
    RGB color = {red,green,blue};
    if(!BMP_PushRGB(canvas,color)) {
#if TGA_DEBUG == 1
      printf("something bad happened while reading tga file\n");
#endif
      return 0;
    }
  }

  if( canvas->length != canvas->expectedLength ) {
#if TGA_DEBUG == 1
    printf("%ld vs %ld\n", canvas->length, canvas->expectedLength );
#endif
    return 0;
  }

  return 1;
}

/*
int main(int argc, char *argv[]) {

  char *filename = argv[1];
  FILE *fp = fopen(filename,"rb");
  if(!fp) {
    printf("%s fail to open!\n",filename);
    return 0;
  }

  long width, height;
  if(!TGA_Open(fp,&width,&height)) {
    printf("not a tga file\n");
    return 0;
  }

  //printf("width: %ld, height: %ld\n",width,height);
  BMP_Canvas canvas;
  if(!BMP_NewCanvas(&canvas,width,height)) {
    printf("fail to create bmp canvas\n");
    return 0;
  }

  TGA_Read(fp,&canvas);

  long strLength = strlen(filename);
  char *targetFile = malloc(strLength+1);
  strcpy(targetFile,filename);
  targetFile[strLength-4] = '.';
  targetFile[strLength-3] = 'b';
  targetFile[strLength-2] = 'm';
  targetFile[strLength-1] = 'p';
  BMP_Save(&canvas,targetFile);
  BMP_Free(&canvas);

  return 0;

}
*/

