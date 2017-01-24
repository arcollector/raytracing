#include  "bbox.h"

Vector BBOX_GetAxis(int index) {
  static Vector axes[BBOX_AXES_COUNT];
  static int areCreated = 0;
  if(!areCreated) {
    axes[0] = Vector_New(1,0,0);
    axes[1] = Vector_New(0,1,0);
    axes[2] = Vector_New(0,0,1);
    // TODO adding more axes, make the raytracer slower
    // check if can find a solution for this situation
    axes[3] = Vector_Normalize(Vector_New(1,1,1));
    axes[4] = Vector_Normalize(Vector_New(-1,1,1));
    axes[5] = Vector_Normalize(Vector_New(-1,-1,1));
    axes[6] = Vector_Normalize(Vector_New(1,-1,1));
  }
  areCreated = 1;
  return axes[index];
}

BBOX *BBOX_New(Object *obj) {

  BBOX *bbox = malloc(sizeof(BBOX));
  bbox->obj = obj;
  bbox->isUnbounded = 0;
  bbox->next = NULL;

  if(obj->type == OBJ_PLANE) { // not bounding box possible
    bbox->isUnbounded = 1;

  } else if(obj->type == OBJ_SPHERE) {
    BBOX_Sphere(bbox);
  }

  return bbox;
}

BBOXList *BBOXList_New(Object *objList, Object **unboundObjList) {

  BBOX *first = NULL, *prev = NULL;
  long bboxListLength = 0;

  Object *aux = NULL;

  for(Object *node = objList; node; node = node->next) {
    BBOX *bbox = BBOX_New(node);
    // unbounded objects cannot be part of the hierarchy bbox tree
    if(!bbox->isUnbounded) {
      if(!first) {
        prev = first = bbox;
      } else {
        prev = prev->next = bbox;
      }
      bboxListLength++;
    } else {
      if(!aux) {
        *unboundObjList = bbox->obj;
        aux = *unboundObjList;
      } else {
        aux = aux->next = bbox->obj;
      }
    }
  }

  BBOXList *bboxList = malloc(sizeof(BBOXList));
  bboxList->bbox = first;
  bboxList->length = bboxListLength;

  return bboxList;
}

BBOXTree *BBOXTree_New(Object *objList, Object **unboundObjList) {

  // setup bounding box of each primtive
  // and save them in a BBOXList
  BBOXList *bboxList = BBOXList_New(objList, unboundObjList);

  //BBOXList_Print(bboxList); printf("\n");

  // build hierarchy tree
  BBOXTree *root = BBOXTree_BuildHierarchy(bboxList, 0);

  // no more need it
  BBOXList_Free(bboxList);

  return root;
}

long BBOXList_ToObjectList(BBOXList *bboxList, Object **object) {

  if(!bboxList->bbox) {
    return 0;
  }

  long totalObjects = 1;
  BBOX *bbox = bboxList->bbox;
  *object = bbox->obj;
  Object *aux = *object;
  for(bbox = bbox->next; bbox; bbox = bbox->next) {
    aux = aux->next = bbox->obj;
    totalObjects++;
  }

  return totalObjects;
}

void BBOXTree_ComputeNodeBBOX(BBOXTree *node, BBOXList *bboxList) {

  // look global min, max
  BBOX nodeBBOX;
  BBOX *bbox = bboxList->bbox;
  long totalObjects = 0;
  for(long i = 0; i < BBOX_AXES_COUNT; i++) { // use the first bbox as start point
    totalObjects++;
    nodeBBOX.min[i] = bbox->min[i];
    nodeBBOX.max[i] = bbox->max[i];
  }
  for(bbox = bbox->next; bbox; bbox = bbox->next) { // keep iterating
    totalObjects++;
    for(long i = 0; i < BBOX_AXES_COUNT; i++) {
      nodeBBOX.min[i].x = MIN(nodeBBOX.min[i].x, bbox->min[i].x);
      nodeBBOX.min[i].y = MIN(nodeBBOX.min[i].y, bbox->min[i].y);
      nodeBBOX.min[i].z = MIN(nodeBBOX.min[i].z, bbox->min[i].z);
      nodeBBOX.max[i].x = MAX(nodeBBOX.max[i].x, bbox->max[i].x);
      nodeBBOX.max[i].y = MAX(nodeBBOX.max[i].y, bbox->max[i].y);
      nodeBBOX.max[i].z = MAX(nodeBBOX.max[i].z, bbox->max[i].z);
    }
  }
  // compute centrod of bounding box
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    nodeBBOX.centroid[i] = Vector_DivScalar(
      Vector_AddVector(nodeBBOX.min[i], nodeBBOX.max[i]),
      2
    );
  }

  node->bbox = nodeBBOX;
  //Vector_Print(nodeBBOX.min[0]); Vector_Print(nodeBBOX.min[1]); Vector_Print(nodeBBOX.min[2]);
  //Vector_Print(nodeBBOX.max[0]); Vector_Print(nodeBBOX.max[1]); Vector_Print(nodeBBOX.max[2]);
  //Vector_Print(nodeBBOX.centroid[0]); Vector_Print(nodeBBOX.centroid[1]); Vector_Print(nodeBBOX.centroid[2]);
}

void BBOXTree_GenerateSplittedList(
  BBOXTree *node,
  BBOXList *list,
  BBOXList *leftList,
  BBOXList *rightList
) {

  BBOX nodeBBOX = node->bbox;

  // TODO: fix this ugliness
  BBOX *left[BBOX_AXES_COUNT][100],
      *right[BBOX_AXES_COUNT][100];

  long leftLength[BBOX_AXES_COUNT], rightLength[BBOX_AXES_COUNT];
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    leftLength[i] = 0;
    rightLength[i] = 0;
  }

  for(BBOX *bbox = list->bbox; bbox; bbox = bbox->next) {
    for(long i = 0; i < BBOX_AXES_COUNT; i++) {
      double dist = Vector_Dot(BBOX_GetAxis(i),
        Vector_SubVector(bbox->centroid[i], nodeBBOX.centroid[i])
      );
      if(dist >= 0) {
        right[i][rightLength[i]++] = bbox;
      } else {
        left[i][leftLength[i]++] = bbox;
      }
    }
  }

  // check which axis is better balanced
  long dist = abs(rightLength[0] - leftLength[0]);
  long axisIndex = 0;
  for(long i = 1; i < BBOX_AXES_COUNT; i++) {
    long temp = abs(rightLength[i] - leftLength[i]);
    if(temp < dist) {
      dist = temp;
      axisIndex = i;
    }
  }

  printf("using axis %ld for spplting\n", axisIndex);
  // TODO: fix this ugliness
  if(!leftLength[axisIndex]) {
    printf("fatal error leftLength[axisIndex] is 0\n");
    exit(0);
  }
  if(!rightLength[axisIndex]) {
    printf("fatal error rightLength[axisIndex] is 0\n");
    exit(0);
  }

  // populate left and right BBOXList lists
  BBOX *aux;
  leftList->length = leftLength[axisIndex];
  aux = leftList->bbox = left[axisIndex][0];
  for(long i = 1; i < leftLength[axisIndex]; i++) {
    aux = aux->next = left[axisIndex][i];
  }
  aux->next = NULL;
  rightList->length = rightLength[axisIndex];
  aux = rightList->bbox = right[axisIndex][0];
  for(long i = 1; i < rightLength[axisIndex]; i++) {
    aux = aux->next = right[axisIndex][i];
  }
  aux->next = NULL;
}

BBOXTree *BBOXTree_BuildHierarchy(
  BBOXList *list,
  int forceGrouping
) {

  if(!list->length) {
    return NULL;
  }

  BBOXTree *tree = malloc(sizeof(BBOXTree));
  tree->left = tree->right = NULL;
  tree->objectList = NULL;
  tree->objectListLength = 0;

  BBOXTree_ComputeNodeBBOX(tree, list);

  if(!forceGrouping && list->length > 4) {
    BBOXList *left = malloc(sizeof(BBOXList)),
            *right = malloc(sizeof(BBOXList));
    BBOXTree_GenerateSplittedList(tree, list, left, right);
    printf("splitting occurred\n");
    printf("left.length: %ld\n", left->length);
    printf("right.length: %ld\n", right->length);

    tree->left = BBOXTree_BuildHierarchy(left, 0);
    tree->right = BBOXTree_BuildHierarchy(right, 0);

    free(left);
    free(right);

  } else {
    tree->objectListLength = BBOXList_ToObjectList(
      list, &tree->objectList
    );
    printf("leaf generated with %ld children\n", tree->objectListLength);
  }

  return tree;
}

void BBOX_Print(BBOX *bbox) {

  printf("object is %d\n",bbox->obj->type);
  (*bbox->obj->print)(bbox->obj->primitive);
  if(bbox->isUnbounded) {
    printf("object is unbounded\n");
    return;
  }
  printf("x axis\n");
  Vector_Print(bbox->min[0]);
  Vector_Print(bbox->max[0]);
  Vector_Print(bbox->centroid[0]);
  printf("y axis\n");
  Vector_Print(bbox->min[1]);
  Vector_Print(bbox->max[1]);
  Vector_Print(bbox->centroid[1]);
  printf("z axis\n");
  Vector_Print(bbox->min[2]);
  Vector_Print(bbox->max[2]);
  Vector_Print(bbox->centroid[2]);
}

void BBOXList_Print(BBOXList *bboxList) {

  printf("==== BBOX ====\n");
  printf("==== total elems are: %ld\n", bboxList->length);
  for(BBOX *bbox = bboxList->bbox; bbox; bbox = bbox->next) {
    BBOX_Print(bbox);
  }
  printf("==============\n");
}

void BBOXTree_Print(BBOXTree *tree) {

  BBOXTree *stack[1000];
  long stackLength = 0;
  stack[stackLength++] = tree;
  long level = 0;
  long totalNodes = 0;

  printf("===== Printing BBOX Tree =====\n");
  while(stackLength) {

    BBOXTree *node = stack[--stackLength];
    BBOX nodeBBOX = node->bbox;
    printf("level %ld\n", level);
    Vector_Print(nodeBBOX.min[0]); Vector_Print(nodeBBOX.min[1]); Vector_Print(nodeBBOX.min[2]);
    Vector_Print(nodeBBOX.max[0]); Vector_Print(nodeBBOX.max[1]); Vector_Print(nodeBBOX.max[2]);

    if(node->left) stack[stackLength++] = node->left;
    if(node->right) stack[stackLength++] = node->right;
    if(node->objectListLength) {
      printf("node is a leaf with %ld objects\n", node->objectListLength);
    }

    if(!totalNodes) {
      level++;
      totalNodes = pow(2,level);
    } else {
      totalNodes -= 2;
    }
  }
}

void BBOXList_Free(BBOXList *bboxList) {
  for(BBOX *bbox = bboxList->bbox; bbox;) {
    BBOX *tmp = bbox->next;
    free(bbox);
    bbox = tmp;
  }
  free(bboxList);
}

void BBOXTree_Free(BBOXTree *tree) {

  if(!tree) return;
  if(tree->objectListLength) return;

  BBOXTree_Free(tree->left);
  BBOXTree_Free(tree->right);

  free(tree);

}

int BBOX_Intersect(Ray ray, BBOX *bbox) {

  if(bbox->isUnbounded) return 1;

  double tMins[BBOX_AXES_COUNT] = {NEGATIVE_INFINITY, NEGATIVE_INFINITY, NEGATIVE_INFINITY},
        tMaxs[BBOX_AXES_COUNT] = {POSITIVE_INFINITY, POSITIVE_INFINITY, POSITIVE_INFINITY};

  // calc plane intersection
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    Vector axis = BBOX_GetAxis(i);;
    Vector min = bbox->min[i];
    Vector max = bbox->max[i];
    double deno = Vector_Dot(axis, ray.dir);
    //printf("axis  %ld\n",i);
    //printf("\tdeno %5.5f\n", deno);
    if(fabs(deno) < EPSILON) {
      double dist1 = Vector_Dot(axis, Vector_SubVector(min, ray.start));
      double dist2 = Vector_Dot(axis, Vector_SubVector(max, ray.start));
      if(SIGN(dist1) == SIGN(dist2)) {
        //printf("\tray is outside bounding box\n");
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
    //printf("\tt1 %5.5f,t2 %5.5f,tMin %5.5f,tMax %5.5f\n", t1,t2,tMin,tMax);
    if(tMax < 0) {
      return 0; // not intersection possible
    }

    tMins[i] = tMin;
    tMaxs[i] = tMax;
  }

  double tMin = MAX(MAX(tMins[0], tMins[1]), tMins[2]);
  double tMax = MIN(MIN(tMaxs[0], tMaxs[1]), tMaxs[2]);
  //printf("\ttMin %5.5f, tMax %5.5f\n", tMin,tMax);

  if(tMax < tMin) {
    return 0; // not intersecion with bounding box
  }

  return 1;
}

int BBOXTree_NodeIntersect(Ray ray, BBOXTree *node) {
  return BBOX_Intersect(ray, &node->bbox);
}

void BBOX_Sphere(BBOX *bbox) {

  Sphere *sphere = bbox->obj->primitive;

  for( long i = 0; i < BBOX_AXES_COUNT; i++) {
    Vector axis = BBOX_GetAxis(i);
    double length = Vector_Dot(axis, sphere->center);
    Vector min = Vector_MulScalar(axis, length - sphere->radius);
    Vector max = Vector_MulScalar(axis, length + sphere->radius);
    bbox->min[i] = min;
    bbox->max[i] = max;
    bbox->centroid[i] = Vector_DivScalar(
      Vector_AddVector(bbox->min[i], bbox->max[i]), 2
    );
  }
}
