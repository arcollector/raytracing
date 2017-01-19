#include  "bbox.h"

Vector axes[3];

BBOX *BBOX_New(Object *objList) {

  axes[0] = Vector_New(1,0,0);
  axes[1] = Vector_New(0,1,0);
  axes[2] = Vector_New(0,0,1);

  BBOX *bbox = NULL, *first = NULL;

  for(Object *node = objList; node; node = node->next) {
    if(!bbox) {
      bbox = malloc(sizeof(BBOX));
      first = bbox;
    } else {
      bbox = bbox->next = malloc(sizeof(BBOX));
    }
    bbox->obj = node;
    bbox->isUnbounded = 0;
    bbox->next = NULL;

    if(node->type == OBJ_PLANE) { // not bounding box possible
      bbox->isUnbounded = 1;

    } else if(node->type == OBJ_SPHERE) {
      BBOX_Sphere(bbox);
    }
  }

  return first;
}

void BBOX_Sphere(BBOX *bbox) {

  Sphere *sphere = bbox->obj->primitive;

  for( long i = 0; i < 3; i++) {
    Vector axis = axes[i];
    double length = Vector_Dot(axis, sphere->center);
    Vector min = Vector_MulScalar(axis, length - sphere->radius);
    Vector max = Vector_MulScalar(axis, length + sphere->radius);
    bbox->min[i] = min;
    bbox->max[i] = max;
  }

}

int BBOX_Intersect(Ray ray, BBOX *bbox) {

  if(bbox->isUnbounded) return 1;

  double tMins[3] = {POSITIVE_INFINITY, POSITIVE_INFINITY, POSITIVE_INFINITY},
        tMaxs[3] = {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY};

  // calc plane intersection
  for(long i = 0; i < 3; i++) {
    Vector axis = axes[i];
    Vector min = bbox->min[i];
    Vector max = bbox->max[i];
    double deno = Vector_Dot(axis, ray.dir);
    if(fabs(deno) < EPSILON) {
      double dist1 = Vector_Dot(axis, Vector_SubVector(min, ray.start));
      double dist2 = Vector_Dot(axis, Vector_SubVector(max, ray.start));
      if(SIGN(dist1) == SIGN(dist2)) {
        return 0; // outside bounding box
      }
      continue; // infinite intersection
    }
    double nume = Vector_Dot(axis, Vector_SubVector(min, ray.start));
    double t1 = nume / deno;

    nume = Vector_Dot(axis, Vector_SubVector(max, ray.start));
    double t2 = nume / deno;

    double tMin = MIN(t1,t2);
    double tMax = MAX(t1,t2);
    if(tMax < 0) {
      return 0; // not intersection possible
    }

    tMins[i] = tMin;
    tMaxs[i] = tMax;
  }

  double tMin = MAX(MAX(tMins[0], tMins[1]), tMins[2]);
  double tMax = MIN(MIN(tMaxs[0], tMaxs[1]), tMaxs[2]);
  if(tMax < tMin) {
    return 0; // not intersecion with bounding box
  }

  return 1;
}

void BBOX_Print(BBOX *bbox) {

  printf("==== BBOX ====\n");
  for( ; bbox; bbox = bbox->next) {
    printf("object is %d\n",bbox->obj->type);
    (*bbox->obj->print)(bbox->obj->primitive);
    if(bbox->isUnbounded) {
      printf("object is unbounded\n");
      continue;
    }
    printf("x axis\n");
    Vector_Print(bbox->min[0]);
    Vector_Print(bbox->max[0]);
    printf("y axis\n");
    Vector_Print(bbox->min[1]);
    Vector_Print(bbox->max[1]);
    printf("z axis\n");
    Vector_Print(bbox->min[2]);
    Vector_Print(bbox->max[2]);
  }
  printf("==============\n");
}

void BBOX_Free(BBOX *bbox) {
  for(; bbox;) {
    BBOX *tmp = bbox->next;
    free(bbox);
    bbox = tmp;
  }
}
