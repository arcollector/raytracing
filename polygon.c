#include "polygon.h"

Polygon *Polygon_New(Texture *tex) {

  Polygon *poly = malloc(sizeof(Polygon));
  if(!poly) return NULL;

  poly->normal = Vector_New(0,0,0);
  poly->verticesLength = 0;
  poly->vertices = NULL;
  poly->edgesLength = 0;
  poly->edges = NULL;
  poly->tex = tex;
  poly->local = Matrix_New();
  poly->invLocal = Matrix_New();

  return poly;
}

int Polygon_SetVertices(long pointsLength, Vector *points, Polygon *poly) {
  if(pointsLength < 3) return 0;
  poly->vertices = calloc(pointsLength,sizeof(Vertex));
  if(!poly->vertices) return 0;
  for(long i = 0; i < pointsLength; i++) {
    poly->vertices[i].p = points[i];
  }
  poly->verticesLength = pointsLength;
  // polygon vertices must be specified
  // in cw ordering so that the normal will
  // point to the outside of the polygon
  Vector p0 = points[0],
         p1 = points[1],
         p2 = points[2];
  poly->normal = Vector_Normalize(
    Vector_Cross(
      Vector_FromP1toP2(p0, p1),
      Vector_FromP1toP2(p0, p2)
    )
  );
  poly->edges = calloc(pointsLength,sizeof(Edge));
  if(!poly->edges) return 0;
  for(long i = 0; i < pointsLength; i++) {
    Vertex v1 = poly->vertices[i];
    Vertex v2 = poly->vertices[(i+1)%pointsLength];
    poly->edges[i].v1 = v1;
    poly->edges[i].v2 = v2; 
  }
  poly->edgesLength = pointsLength;
  return 1;
}

Hit *Polygon_Intersect(Ray ray, void *_poly) {
  Polygon *poly = _poly;

  double deno = Vector_Dot(poly->normal, ray.dir);
  if(fabs(deno) < EPSILON) { // not intersection possible
    return NULL;
  }

  double nume = Vector_Dot(
    poly->normal,
    Vector_SubVector(poly->vertices[0].p, ray.start)
  );

  double t = nume/deno;
  if(fabs(t) < EPSILON) {
    return NULL;
  }

  Vector pi = Ray_PointAt(ray,t);
  // find out dominant axis
  Vector n = poly->normal;
  double nx = fabs(n.x);
  double ny = fabs(n.y);
  double nz = fabs(n.z);
  int useX = nx >= ny && ny >= nz ? 1 : 0;
  int useY = ny >= nx && nx >= nz ? 1 : 0;
  int useZ = !useX && !useY ? 1 : 0;
  int prevSign, isFirstRun = 1;
  // test if pi is inside polygon projected edges
  for(long i = 0; i < poly->edgesLength; i++) {
    Edge edge = poly->edges[i];
    Vector v1 = edge.v1.p;
    Vector v2 = edge.v2.p;
    double res;
    if(useX) {
      res = (pi.y - v1.y)*(v2.z - v1.z) -
            (pi.z - v1.z)*(v2.y - v1.y);
    } else if(useY) {
      res = (pi.x - v1.x)*(v2.z - v1.z) -
            (pi.z - v1.z)*(v2.x - v1.x);
    } else {
      res = (pi.y - v1.y)*(v2.x - v1.x) -
            (pi.x - v1.x)*(v2.y - v1.y);
    }
    double sign = SIGN(res);
    if(isFirstRun) {
      isFirstRun = 0;
    } else if(sign != 0 && sign != prevSign) {
      return NULL;
    }
    prevSign = sign;
  }

  return Hit_New(1,t);
}

Vector Polygon_Normal(Vector point, void *_poly) {
  Polygon *poly = _poly;
  return poly->normal;
}

void Polygon_Free(void *_poly) {
  Polygon *poly = _poly;
  if(poly->vertices) free(poly->vertices);
  if(poly->edges) free(poly->edges);
  if(poly->tex) Texture_Free(poly->tex);
  free(poly);
}

void Polygon_Print(void *_poly) {
  Polygon *poly = _poly;
  printf("==== POLYGON ====\n");
  printf("normal "); Vector_Print(poly->normal);
  printf("verticesLength %ld\n", poly->verticesLength);
  printf("====== VERTICES ==\n");
  for(long i = 0; i < poly->verticesLength; i++) {
    Vector_Print(poly->vertices[i].p);
  }
  printf("====== EDGES ==\n");
  for(long i = 0; i < poly->edgesLength; i++) {
    Vector_Print(poly->edges[i].v1.p);
    Vector_Print(poly->edges[i].v2.p);
    printf(" ....\n");
  }
  printf("====== TEXTURE ==\n");
  Texture_Print(poly->tex);
}

void Polygon_BBOX(BBOX *bbox) {
  Polygon *poly = bbox->obj->primitive;
  Vertex *vertex = poly->vertices;
  Vector min = vertex->p;
  Vector max = min;
  for(long i = 0; i < poly->verticesLength; i++) {
    Vector p = poly->vertices[i].p;
    min.x = MIN(min.x, p.x);
    min.y = MIN(min.y, p.y);
    min.z = MIN(min.z, p.z);
    max.x = MAX(max.x, p.x);
    max.y = MAX(max.y, p.y);
    max.z = MAX(max.z, p.z);
  }
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    Vector axis = BBOX_GetAxis(i);
    bbox->min[i] = Vector_MulScalar(axis, Vector_Dot(axis, min));
    bbox->max[i] = Vector_MulScalar(axis, Vector_Dot(axis, max));
    bbox->centroid[i] = Vector_DivScalar(
      Vector_AddVector(bbox->min[i], bbox->max[i]),
      2
    );
  }
}
