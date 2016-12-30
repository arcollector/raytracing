#include <stdio.h>

#include "../rgb.h"
#include "../octree_quantizer.h"

#define BMP_DEBUG 1
#include "../bmp.h"

#define TGA_DEBUG 1
#include "../tga.h"

#define PCX_DEBUG 1
#include "../pcx.h"

int main( int argc, char *argv[] ) {

  if( argc < 3 ) {
    printf("you need to specify source.bmp|tga|pcx and target.bmp filenames\n");
    return 0;
  }

  char *sourceFile = argv[1];
  FILE *fp = fopen( sourceFile, "rb" );
  if( fp == NULL ) {
    printf("source bmp/tga/pcx cannot open: %s\n", sourceFile);
    return 0;
  }

  BMP_Canvas bmpCanvas;
  if( BMP_IsBMP( fp ) ) {
    BMP_Header bmpHeader;
    if( !BMP_Read( fp, &bmpHeader, &bmpCanvas ) ) {
      printf("bmp file: %s is corrupted\n", sourceFile);
      return 0;
    }
  } else if( TGA_IsTGA( fp ) ) {
    if( !TGA_Read( fp, &bmpCanvas ) ) {
      printf("tga file: %s is corrupted\n", sourceFile);
      return 0;
    }
  } else if( PCX_IsPCX( fp ) ) {
    PCX_Header pcxHeader;
    if( !PCX_Read( fp, &pcxHeader, &bmpCanvas ) ) {
      printf("pcx file: %s is corrupted\n", sourceFile);
      return 0;
    }
  } else {
    printf("fail to read image file: %s", sourceFile);
    return 0;
  }
  fclose( fp );

  Octree_Node *root = NULL;
  for( int i = 0; i < bmpCanvas.length; i++ ) {
    RGB color = bmpCanvas.data[i];
    root = Octree_Insert( root, color, 0 );
    Octree_ReduceIfNeeded();
  }

  RGB palette[256];
  int paletteLength = 0;
  Octree_GeneratePalette( root, palette, &paletteLength );

  for( int i = 0; i < bmpCanvas.length; i++ ) {
    RGB color = bmpCanvas.data[i];
    int colorIndex = Octree_FindColorPaletteIndex( root, color );
    color = palette[colorIndex];
    bmpCanvas.data[i] = color;
   }

  if( !BMP_Save( &bmpCanvas, argv[2] ) ) {
    return 0;
  }

  BMP_Free( &bmpCanvas );
  Octree_Free( root );

  return 0;
}
