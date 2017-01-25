#include  "bbox.h"

#define BBOXTREE_LEAF_BUNCHING 4

int gbIsEndRecursion = 0;

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

BBOX *BBOXList_New(
  Object *objList,
  Object **unboundObjList,
  long *bboxListLength
) {

  BBOX *first = NULL, *prev = NULL;
  *bboxListLength = 0;

  for(Object *node = objList, *aux = NULL; node; node = node->next) {
    BBOX *bbox = BBOX_New(node);
    // unbounded objects cannot be part of the hierarchy bbox tree
    if(!bbox->isUnbounded) {
      if(!first) {
        prev = first = bbox;
      } else {
        prev = prev->next = bbox;
      }
      ++*bboxListLength;
    } else {
      if(!aux) {
        *unboundObjList = bbox->obj;
        aux = *unboundObjList;
      } else {
        aux = aux->next = bbox->obj;
      }
    }
  }

  return first;
}

BBOXTree *BBOXTree_New(Object *objList, Object **unboundObjList) {

  // setup bounding box of each primtive
  // and save them in a BBOX
  long bboxListLength;
  BBOX *bboxList = BBOXList_New(
    objList, unboundObjList, &bboxListLength
  );
  //BBOXList_Print(bboxList); printf("\n");

  // build hierarchy tree
  BBOXTree *root = BBOXTree_BuildHierarchy(
    bboxList, bboxListLength, 0
  );

  // no more need it
  BBOXList_Free(bboxList);

  return root;
}

long BBOX_ToObjectList(BBOX *bboxList, Object **object) {

  if(!bboxList) {
    return 0;
  }

  long totalObjects = 1;
  BBOX *bbox = bboxList;
  *object = bbox->obj;
  Object *aux = *object;
  for(bbox = bbox->next; bbox; bbox = bbox->next) {
    aux = aux->next = bbox->obj;
    totalObjects++;
  }

  return totalObjects;
}

void BBOXTree_ComputeNodeBBOX(BBOXTree *node, BBOX *bboxList) {

  // look global min, max
  BBOX nodeBBOX;
  nodeBBOX.isUnbounded = 0;
  nodeBBOX.next = NULL;

  BBOX *bbox = bboxList;
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
      Vector_AddVector(
        nodeBBOX.min[i], nodeBBOX.max[i]
      ),
      2
    );
  }

  node->bbox = nodeBBOX;
  //Vector_Print(nodeBBOX.min[0]); Vector_Print(nodeBBOX.min[1]); Vector_Print(nodeBBOX.min[2]);
  //Vector_Print(nodeBBOX.max[0]); Vector_Print(nodeBBOX.max[1]); Vector_Print(nodeBBOX.max[2]);
  //Vector_Print(nodeBBOX.centroid[0]); Vector_Print(nodeBBOX.centroid[1]); Vector_Print(nodeBBOX.centroid[2]);
}

int BBOXTree_GenerateSplitLists(
  BBOXTree *node,
  BBOX *list,
  BBOX **leftList,
  long *leftListLength,
  BBOX **rightList,
  long *rightListLength
) {

  BBOX *left[BBOX_AXES_COUNT], *right[BBOX_AXES_COUNT];
  BBOX *prevLeft[BBOX_AXES_COUNT], *prevRight[BBOX_AXES_COUNT];
  long leftLength[BBOX_AXES_COUNT], rightLength[BBOX_AXES_COUNT];
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    leftLength[i] = 0;
    rightLength[i] = 0;
    left[i] = NULL;
    right[i] = NULL;
  }

  BBOX nodeBBOX = node->bbox;
  for(BBOX *bbox = list; bbox; bbox = bbox->next) {
    for(long i = 0; i < BBOX_AXES_COUNT; i++) {
      double dist = Vector_Dot(
        BBOX_GetAxis(i),
        Vector_SubVector(bbox->centroid[i],nodeBBOX.centroid[i])
      );
      // only c can handle this awesomeness!!
      BBOX **clone, **prev;
      if(dist >= 0) {
        rightLength[i]++;
        prev = &prevRight[i];
        clone = !right[i] ? &right[i] : &(prevRight[i]->next);
      } else {
        leftLength[i]++;
        prev = &prevLeft[i];
        clone = !left[i] ? &left[i] : &(prevLeft[i]->next);
      }
      *clone = malloc(sizeof(BBOX));
      memcpy(*clone,bbox,sizeof(BBOX));
      (*clone)->next = NULL;
      *prev = *clone;
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

  /*for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    printf("%ld) %ld %ld\n",i,leftLength[i],rightLength[i]);
  }*/

  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    if(i == axisIndex) continue;
    BBOXList_Free(left[i]);
    BBOXList_Free(right[i]);
  }

  printf("using axis %ld for splitting\n", axisIndex);
  if(!leftLength[axisIndex] || !rightLength[axisIndex]) {
    BBOXList_Free(left[axisIndex]);
    BBOXList_Free(right[axisIndex]);
    printf("FATAL ERROR leftLength[axisIndex] or rightLength[axisIndex] is 0\n");
    return 1;
  }

  *leftList = left[axisIndex];
  *rightList = right[axisIndex];
  *leftListLength = leftLength[axisIndex];
  *rightListLength = rightLength[axisIndex];

  return 0;
}

BBOXTree *BBOXTree_BuildHierarchy(
  BBOX *list,
  long listLength,
  int forceGrouping
) {

  if(gbIsEndRecursion) {
    return NULL;
  }

  if(!listLength) {
    return NULL;
  }

  BBOXTree *tree = malloc(sizeof(BBOXTree));
  tree->left = tree->right = NULL;
  tree->objectList = NULL;
  tree->objectListLength = 0;

  BBOXTree_ComputeNodeBBOX(tree, list);

  if(!forceGrouping && listLength > BBOXTREE_LEAF_BUNCHING) {
    BBOX *left, *right;
    long leftLength, rightLength;
    int isFailure = BBOXTree_GenerateSplitLists(
      tree, list, &left, &leftLength, &right, &rightLength
    );
    if(isFailure) {
      gbIsEndRecursion = 1;
      return NULL;
    }
    printf("splitting occurred\n");
    printf("left.length: %ld ", leftLength);
    printf("right.length: %ld\n", rightLength);

    tree->left = BBOXTree_BuildHierarchy(left, leftLength, 0);
    tree->right = BBOXTree_BuildHierarchy(right, rightLength, 0);

    BBOXList_Free(left);
    BBOXList_Free(right);

  } else {
    tree->objectListLength = BBOX_ToObjectList(
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
  printf("==============\n");
}

void BBOXList_Print(BBOX *bboxList) {

  printf("==== BBOX LIST ====\n");
  long length = 0;
  for(BBOX *bbox = bboxList; bbox; bbox = bbox->next) {
    BBOX_Print(bbox);
    length++;
  }
  printf("total elems are: %ld\n", length);
  printf("=== END BOX LIST ===\n");
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

void BBOX_Free(BBOX *bbox) {
  free(bbox);
  bbox = NULL;
}

void BBOXList_Free(BBOX *bboxList) {
  for(BBOX *bbox = bboxList; bbox;) {
    BBOX *tmp = bbox->next;
    free(bbox);
    bbox = tmp;
  }
  bboxList = NULL;
}

void BBOXTree_Free(BBOXTree *tree) {
  if(!tree) return;
  if(tree->objectListLength) return;

  BBOXTree_Free(tree->left);
  BBOXTree_Free(tree->right);

  free(tree);
  tree = NULL;
}

// ===============================
// * BBOX Intersect functions *
// ===============================

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
