#ifndef POLYGON_H
#define POLYGON_H

#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"
#include "vector.h"
#include "matrix.h"
#include "ray.h"
#include "texture.h"
#include "hit.h"
#include "bbox.h"

typedef struct VertexStruct {
  Vector p;
} Vertex;

typedef struct EdgeStruct {
  Vertex v1, v2;
} Edge;

typedef struct {
  Vertex *vertices;
  long verticesLength;
  Edge *edges;
  long edgesLength;
  Vector normal;
  Texture *tex;
  Matrix local, invLocal;
} Polygon, Facet;

typedef struct {
  Facet *facets;
  long facetsLength;
  Texture *texture;
} Mesh;

Polygon *Polygon_New(Texture *tex);
int Polygon_Setup(long pointsLength, Vector points[], Polygon *poly);

Hit *Polygon_Intersect(Ray ray, void *poly);
Vector Polygon_Normal(Vector point, void *poly);
void Polygon_Free(void *poly);
void Polygon_Print(void *poly);

void Polygon_BBOX(BBOX *bbox);

#endif
