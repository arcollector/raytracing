#include "bmp.h"

int BMP_IsBMP( FILE *fp ) {

  int returnValue = 0;

  long seek = ftell( fp );
  fseek( fp, 0, SEEK_SET );
  if( fgetc( fp ) == 'B' && fgetc( fp ) == 'M' ) {
    returnValue = 1;
  }

  fseek( fp, seek, SEEK_SET );
  return returnValue;

}

int BMP_NewCanvas( BMP_Canvas *canvas, long width, long height ) {

  canvas->width = width;
  canvas->height = height;
  canvas->length = 0;
  canvas->expectedLength = width * height;
  canvas->data = calloc( width*height, sizeof(RGB) );
  if( canvas->data == NULL ) {
    printf("fail to allocate canvas memory\n");
    return 0;
  }

  return 1;
}

 
int BMP_Read( FILE *fp, BMP_Header *header, BMP_Canvas *canvas ) {

  //printf("EOF? %d\n", feof(fp) );

  fread( &header->B, sizeof(uint8_t), 1, fp );
  fread( &header->M, sizeof(uint8_t), 1, fp );
  fread( &header->fileSize, sizeof(uint32_t), 1, fp );
  fread( &header->reserved0, sizeof(uint16_t), 1, fp );
  fread( &header->reserved1, sizeof(uint16_t), 1, fp );
  fread( &header->canvasAddress, sizeof(uint32_t), 1, fp );
  fread( &header->headerSize, sizeof(uint32_t), 1, fp );
  fread( &header->width, sizeof(uint32_t), 1, fp );
  fread( &header->height, sizeof(uint32_t), 1, fp );
  fread( &header->imagePlanes, sizeof(uint16_t), 1, fp );
  fread( &header->colorDepth, sizeof(uint16_t), 1, fp );
  fread( &header->compressionType, sizeof(uint32_t), 1, fp );
  fread( &header->canvasSize, sizeof(uint32_t), 1, fp );
  fread( &header->ignore1, sizeof(uint32_t), 1, fp );
  fread( &header->ignore2, sizeof(uint32_t), 1, fp );
  fread( &header->ignore3, sizeof(uint32_t), 1, fp );
  fread( &header->ignore4, sizeof(uint32_t), 1, fp );

  if( feof( fp ) ) {
#if BMP_DEBUG == 1
    printf( "bad bmp image\n" );
#endif
    return 0;
  }

  /*
  printf( "====== BMP HEADER =====\n" );
  printf( "sig: %c%c\n", header->B,header->M );
  printf( "file size: %d\n", header->fileSize );
  printf( "reserved 1: %d\n", header->reserved0 );
  printf( "reserved 2: %d\n", header->reserved1 );
  printf( "canvas address: %d\n", header->canvasAddress );
  printf( "header size: %d\n", header->headerSize );
  printf( "width: %d\n", header->width );
  printf( "height: %d\n", header->height );
  printf( "image planes: %d\n", header->imagePlanes );
  printf( "color depth: %d\n", header->colorDepth );
  printf( "compression type: %d\n", header->compressionType );
  printf( "canvas size: %d\n", header->canvasSize );
  printf( "ignore 1: %d\n", header->ignore1 );
  printf( "ignore 2: %d\n", header->ignore2 );
  printf( "ignore 3: %d\n", header->ignore3 );
  printf( "ignore 4: %d\n", header->ignore4 );
  printf( "=======================\n" );
  */

  char b = header->B;
  char m = header->M; 
  if( b != 'B' || m != 'M' ) {
#if BMP_DEBUG == 1
    printf("not a bmp file\n");
#endif
    return 0;
  }

  int fileSize = header->fileSize;
  
  int canvasAddress = header->canvasAddress;
  if( canvasAddress != BMP_24BITS_ADDRESS ) {
    printf("only 24 bits images supported\n");
    return 0;
  }

  int headerSize = header->headerSize;
  if( headerSize != BMP_HEADER_SIZE ) {
    printf("bmp header size field value invalid\n");
    return 0;
  }

  int width = header->width;
  int height = header->height;

  int imagePlanes = header->imagePlanes;
  if( imagePlanes != BMP_IMAGE_PLANES ) {
    printf("bmp invalid image planes value\n");
    return 0;
  }

  int colorDepth = header->colorDepth;
  if( colorDepth != BMP_COLOR_DEPTH_24BITS ) {
    printf("only 24 bits image supported\n");
    return 0;
  }
 
  int compressionType = header->compressionType;
  if( compressionType != BMP_NO_COMPRESSION ) {
    printf("bmp compression type unsupported\n");
    return 0;
  }

  if(!BMP_NewCanvas( canvas, width, height )) {
    return 0;
  }  

  //printf("canvas address must be 54 == %ld\n", ftell(fp));

  long extraBytes = width % 4;
  for( int y = height - 1; y >= 0; y-- ) {
    for( int x = 0; x < width; x++ ) {
      unsigned int blue = fgetc( fp );
      unsigned int green = fgetc( fp );
      unsigned int red = fgetc( fp );
      //printf("{%d,%d,%d}\n", red,green,blue);
      long address = (y*width+x);
      //printf("address: %ld\n", address );
      canvas->data[address].red = red;
      canvas->data[address].green = green;
      canvas->data[address].blue = blue;
      canvas->length++;
    }
    for( int x = 0; x < extraBytes; x++ ) {
      fgetc( fp );
    }
  }

  //printf("seek pos: %ld\n", ftell(fp) );
  //printf("EOF? %d\n", feof(fp) );

  if( feof( fp ) ) {
    printf( "corrupted bmp image\n" );
    return 0;
  }

  return 1;
}

int BMP_PushRGB( BMP_Canvas *canvas, RGB color ) {
  if( canvas->length >= canvas->expectedLength ) {
    return 0;
  }
  canvas->data[canvas->length++] = color;
  return 1;
}

#define LE_1 0x000000ff
#define LE_2 0x0000ff00
#define LE_3 0x00ff0000
#define LE_4 0xff000000

int BMP_Save( BMP_Canvas *canvas, char *filename ) {

  long width = canvas->width;
  long height = canvas->height;
  long extraBytes = width % 4;
  long canvasSize = height*width*3 + height*extraBytes;
  long fileSize = 54 + canvasSize;

  FILE *fp = fopen( filename, "wb" );
  if( fp == NULL ) {
    printf("error creating %s\n", filename );
    return 0;
  }

  fputc( 'B', fp );
  fputc( 'M', fp );
  fputc( (fileSize & LE_1), fp );
  fputc( (fileSize & LE_2) >> 8, fp );
  fputc( (fileSize & LE_3) >> 16, fp );
  fputc( (fileSize & LE_4) >> 32, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 54, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 40, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( (width & LE_1), fp );
  fputc( (width & LE_2) >> 8, fp );
  fputc( (width & LE_3) >> 16, fp );
  fputc( (width & LE_4) >> 32, fp );
  fputc( (height & LE_1), fp );
  fputc( (height & LE_2) >> 8, fp );
  fputc( (height & LE_3) >> 16, fp );
  fputc( (height & LE_4) >> 32, fp );
  fputc( 1, fp );
  fputc( 0, fp );
  fputc( 24, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( (canvasSize & LE_1), fp );
  fputc( (canvasSize & LE_2) >> 8, fp );
  fputc( (canvasSize & LE_3) >> 16, fp );
  fputc( (canvasSize & LE_4) >> 32, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );
  fputc( 0, fp );

  for( int y = height - 1; y >= 0; y-- ) {
    for( int x = 0; x < width; x++ ) {
      long address = y*width + x;
      RGB color = canvas->data[address];
      fputc( color.blue, fp );
      fputc( color.green, fp );
      fputc( color.red, fp );
    }
    for( int x = 0; x < extraBytes; x++ ) {
      fputc( 0, fp );
    }
  }

  fclose( fp );

  return 1;
}

void BMP_Free( BMP_Canvas *canvas ) {
  
  free( canvas->data );
  canvas->data = NULL;
}

int BMP_Test( int argc, char *argv[] ) {

  if( argc < 3 ) {
    printf("missing bmp filename\n");
    return 0;
  }
  
  FILE *fp = fopen( argv[1], "rb" );
  if( fp == NULL ) {
    printf("bmp filename does not exists\n");
    return 0;
  }
  
  BMP_Header bmpHeader;
  BMP_Canvas bmpCanvas;
  if( !BMP_Read( fp, &bmpHeader, &bmpCanvas ) ) {
    return 0;
  }

  if( !BMP_Save( &bmpCanvas, argv[2] ) ) {
    return 0;
  }

  fclose( fp );

  return 0;

}

