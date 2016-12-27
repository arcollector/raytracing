#ifndef OCTREE_QUANTIZER_H
#define OCTREE_QUANTIZER_H

struct Octree_NodeStruct {
  int level; // node depth
  int isLeaf;
  long colorCount;
  long redSum, greenSum, blueSum;
  int paletteIndex;
  long childrenCount;
  struct Octree_NodeStruct *children[8]; 
  struct Octree_NodeStruct *nextReducible;
};

typedef struct Octree_NodeStruct Octree_Node;

Octree_Node *Octree_Insert( Octree_Node *node, RGB color, int level );
void Octree_ReduceIfNeeded( void );
void Octree_GeneratePalette( Octree_Node *node, RGB palette[], 
                                                  int *paletteLength );
int Octree_FindColorPaletteIndex( Octree_Node *node, RGB color );
void Octree_Free( Octree_Node *node );

#endif

