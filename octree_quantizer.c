#include <stdio.h>
#include <stdlib.h>

#include "rgb.h"
#include "octree_quantizer.h"

int gbNodeLevel = 7;
int gbLeafLevel = 8;
int gbLeafCount = 0;
Octree_Node *ReduceList[] = { NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL };

Octree_Node *Octree_Insert( Octree_Node *node, RGB color, int level ) {

  if( node == NULL ) {
    //printf("\tCREATING A NEW NODE AT LEVEL %d\n",level);
    node = calloc( 1, sizeof( Octree_Node ) );
    node->level = level;
    node->isLeaf = level >= gbLeafLevel;
    if( node->isLeaf ) {
      gbLeafCount++;
      //printf("\t\tTHE NEW NODE IS A LEAF!\n");
    }
    node->childrenCount = 0;
    node->colorCount = 0;
    node->redSum = node->greenSum = node->blueSum = 0;
    node->nextReducible = NULL;
  }

  node->colorCount++;
  node->redSum += color.red;
  node->greenSum += color.green;
  node->blueSum += color.blue;

  if( !(node->isLeaf) && level < gbLeafLevel ) {
    int branch = (((color.red >> (7 - level)) & 1) << 2) |
                 (((color.green >> (7 - level)) & 1) << 1) |
                 ((color.blue >> (7 - level)) & 1);

    if( node->children[branch] == NULL ) {
      node->childrenCount++;
      if( node->childrenCount == 2 ) {
        node->nextReducible = ReduceList[level];
	ReduceList[level] = node;
      }
    }

    //printf("\tBRANCHING TO LEVEL %d\n", level + 1);
    node->children[branch] = Octree_Insert( (node->children[branch]),
                                                  color, level + 1 );
  }

  return node;
}

void Octree_Kill( Octree_Node *node ) {

  if( node == NULL ) {
    return;
  }
  if( node->isLeaf ) {
    return;
  }

  for( int i = 0; i < 8; i++ ) {
    Octree_Kill( (node->children[i]) );
    free( (node->children[i]) );
    node->children[i] = NULL;
  }
  node->childrenCount = 0;

}

void Octree_ReduceIfNeeded( void ) {

  //printf("===== OCTREE REDUCE LIST =====\n");

  if( gbLeafCount < 256 ) {
    return;
  }

  int currentLevel = gbNodeLevel;

  if( currentLevel < 0 || currentLevel > 7 ) {
    printf( "ERROR: gbNodeLevel erroneus value: %d\n", currentLevel );
    exit( 0 );
  }
  while( ReduceList[currentLevel] == NULL ) {
    currentLevel--;
    if( currentLevel < 0 ) {
      printf("\tIRRECDUCTIBLE TREE!!!\n");
      return;
    }
  }

  //printf("\tREDUCING TREE AT LEVEL %d\n", currentLevel );

  Octree_Node *node = ReduceList[currentLevel];
  //printf("\tNODE TO BE REDUCED HAS %ld CHILDREN\n", node->childrenCount );
  ReduceList[currentLevel] = node->nextReducible;
  if( node->nextReducible ) {
    //printf("\tNEXT NODE HAS %ld CHILDREN\n", node->nextReducible->childrenCount );
  } else {
    //printf("\tTHIS IS THE LAST FROM THIS LEVEL %d\n", currentLevel );
  }

  node->isLeaf = 1;
  gbLeafCount = gbLeafCount - node->childrenCount + 1;

  for( int i = 0; i < 8; i++ ) {
    Octree_Kill( (node->children[i]) );
    free( (node->children[i]) );
    node->children[i] = NULL;
  }
  node->childrenCount = 0;

  if( node->level < gbNodeLevel ) {
    gbNodeLevel = node->level;
    gbLeafLevel = gbNodeLevel + 1;
  }

  //printf("\tNOW NODE LEVEL IS %d\n", gbNodeLevel );
  //printf("\tNOW LEAF LEVEL IS %d\n", gbLeafLevel );
  //printf("========== END REDUCE ==========\n" );

}

void printTree( Octree_Node *node ) {

  printf("==== PRINTING =====\n");
  printf("\tLEVEL: %d\n",node->level);
  printf("\tCHILDREN COUNT: %ld\n", node->childrenCount );
  printf("\tCOLOR COUNT: %ld\n", node->colorCount );

  node = node->children[0]; // level 0 to 1
  node = node->children[0]; // level 1 to 2
  node = node->children[0]; // level 2 to 3
  node = node->children[0]; // level 3 to 4
  node = node->children[0]; // level 4 to 5
  node = node->children[0]; // level 5 to 6

  printf("\tLEVEL: %d\n",node->level);
  printf("\tCHILDREN COUNT: %ld\n", node->childrenCount );
  printf("\tCOLOR COUNT: %ld\n", node->colorCount );

  if( node->isLeaf ) {
    printf("\tI AM LEAF NOW\n");
    return;
  }

  Octree_Node *child0 = node->children[0]; // level 7
  printf("\tLEVEL: %d\n",child0->level);
  printf("\tNODE COUNT: %ld\n", child0->childrenCount );
  printf("\tCOLOR COUNT: %ld\n", child0->colorCount );

  Octree_Node *child1 = node->children[1]; // level 7
  printf("\tLEVEL: %d\n",child1->level);
  printf("\tNODE COUNT: %ld\n", child1->childrenCount );
  printf("\tCOLOR COUNT: %ld\n", child1->colorCount );
  printf("\tI AM LEAF? %d\n", child1->isLeaf );

  printf("\tNODE #2? %d\n", node->children[2] != NULL );

}

void Octree_GeneratePalette( Octree_Node *node, RGB palette[],
                                                  int *paletteLength ) {

  if( node == NULL ) {
    return;
  }

  if( node->isLeaf || node->level >= gbLeafLevel ) {
    palette[(*paletteLength)].red = (unsigned int)
                                    (node->redSum / node->colorCount);
    palette[(*paletteLength)].green = (unsigned int)
                                    (node->greenSum / node->colorCount);
    palette[(*paletteLength)].blue = (unsigned int)
                                    (node->blueSum / node->colorCount);
    node->isLeaf = 1;
    node->paletteIndex = (*paletteLength);
    (*paletteLength)++;
  } else {
    for( int i = 0; i < 8; i++ ) {
      Octree_GeneratePalette( (node->children[i]), palette,
                                                       paletteLength );
    }
  }

}

int Octree_FindColorPaletteIndex( Octree_Node *node, RGB color ) {

  if( node == NULL ) {
    printf("ERROR: SOMETHING WEIRD HAPPENS LOOKING FOR COLOR INDEX\n");
    exit(0);
  }

  if( node->isLeaf ) {
    return node->paletteIndex;
  }

  int level = node->level;
  int branch = (((color.red >> (7 - level)) & 1) << 2) |
	       (((color.green >> (7 - level)) & 1) << 1) |
	       ((color.blue >> (7 - level)) & 1);

  return Octree_FindColorPaletteIndex( (node->children[branch]), color );
}

void Octree_Free( Octree_Node *node ) {

  if( node == NULL ) {
    return;
  }
  if( node->isLeaf ) {
    return;
  }

  for( int i = 0; i < 8; i++ ) {
    Octree_Free( node->children[i] );
    free( node->children[i] );
    node->children[i] = NULL;
  }

}

int Octree_Test() {

  Octree_Node *root = NULL;

  RGB color0 = { 0,0,0 };
  RGB color1 = { 0,0,1 };
  RGB color2 = { 0,1,0 };
  RGB color3 = { 0,1,1 };
  RGB color4 = { 1,0,0 };
  RGB color5 = { 1,0,1 };
  RGB color6 = { 1,1,0 };
  RGB color7 = { 1,1,1 };

  RGB color8 = { 0,0,2 };
  RGB color9 = { 0,0,3 };
  RGB color10 = { 0,1,2 };
  RGB color11 = { 0,1,3 };
  RGB color12 = { 1,0,2 };
  RGB color13 = { 1,0,3 };
  RGB color14 = { 1,1,2 };
  RGB color15 = { 1,1,3 };

  /*root = Octree_Insert( root, color0, 0 );
  root = Octree_Insert( root, color1, 0 );
  root = Octree_Insert( root, color2, 0 );
  root = Octree_Insert( root, color3, 0 );
  root = Octree_Insert( root, color4, 0 );
  root = Octree_Insert( root, color5, 0 );
  root = Octree_Insert( root, color6, 0 );
  root = Octree_Insert( root, color7, 0 );

  root = Octree_Insert( root, color8, 0 );
  root = Octree_Insert( root, color9, 0 );
  root = Octree_Insert( root, color10, 0 );
  root = Octree_Insert( root, color11, 0 );
  root = Octree_Insert( root, color12, 0 );
  root = Octree_Insert( root, color13, 0 );
  root = Octree_Insert( root, color14, 0 );
  root = Octree_Insert( root, color15, 0 );

  printTree( root );

  Octree_Reduce();
  Octree_Reduce();
  Octree_Reduce();
  Octree_Reduce();

  printTree( root );
*/

/*
  RGB color16 = { 255,0,0 };
  RGB color17 = { 255,0,8 };
  RGB color18 = { 255,8,0 };
  RGB color19 = { 0,0,0 };
  RGB color20 = { 0,0,1 };

  root = Octree_Insert( root, color16, 0 );
  root = Octree_Insert( root, color17, 0 );
  root = Octree_Insert( root, color18, 0 );
  root = Octree_Insert( root, color19, 0 );

  printf("ROOT NODE must have 2 children == %ld\n", root->childrenCount );
  printf("ROOT NODE must have 4 colors == %ld\n", root->colorCount );
  Octree_Node *node = root;
  node = node->children[4]; // from 0 to 1
  node = node->children[4]; // from 1 to 2
  node = node->children[4]; // from 2 to 3
  node = node->children[4]; // from 3 to 4
  printf("ThIS NODE AT LEVEL 4 must have 3 children == %ld\n", node->childrenCount );

  Octree_Reduce();

  printf("NOW THE NODE AT LEVE 4 must have 0 children == %ld\n", node->childrenCount );

  printf("INSERTING COLOR ----------->\n");
  Octree_Insert( root, color20, 0 );

  Octree_Reduce();
*/

  for( int y = 0; y < 500; y++ ) {
    for( int x = 0; x < 500; x++ ) {
      RGB color;
      color.red = rand() % 256;
      color.green = rand() % 256;
      color.blue = rand() % 256;
      root = Octree_Insert( root, color, 0 );
      Octree_ReduceIfNeeded();
    }
  }

  printf("\tNOW NODE LEVEL IS %d\n", gbNodeLevel );
  printf("\tNOW LEAF LEVEL IS %d\n", gbLeafLevel );

  RGB palette[256];
  int paletteLength = 0;
  Octree_GeneratePalette( root, palette, &paletteLength );
  RGB color = palette[0];
  printf( "%d -> {%d,%d,%d}\n", 0,color.red,color.green,color.blue);
  color = palette[1];
  printf( "%d -> {%d,%d,%d}\n", 1,color.red,color.green,color.blue);

  color.red = 255;
  color.green = 0;
  color.blue = 0;
  int colorIndex = Octree_FindColorPaletteIndex( root, color );
  printf( "red color is index %d\n", colorIndex );
  color = palette[colorIndex];
  printf( "red color is {%d,%d,%d}\n", color.red,color.green,color.blue);

  Octree_Free( root );

  return 0;

}
