#include  "bbox.h"

static long BBOXList_ToObjectList(BBOX *bboxList, Object **object);
static void BBOXTree_ComputeNodeBBOX(BBOXTree *node, BBOX *bboxList);
static void BBOXTree_GenerateSplitLists(
  BBOXTree *node,
  BBOX *list,
  long listLength,
  BBOX **leftList,
  long *leftListLength,
  BBOX **rightList,
  long *rightListLength
);
static void BBOXArr_ToList(
  BBOX **dest,
  BBOX **soure,
  long sourceLength
);
static BBOXTree *BBOXTree_BuildHierarchy(
  BBOX *list,
  long listLength,
  int forceGrouping
);

#define BBOXTREE_LEAF_BUNCHING 3

Vector BBOX_GetAxis(int index) {
  static Vector axes[7];
  static int areCreated = 0;
  if(!areCreated) {
    axes[0] = Vector_New(1,0,0);
    axes[1] = Vector_New(0,1,0);
    axes[2] = Vector_New(0,0,1);
    axes[3] = Vector_Normalize(Vector_New(1,1,1));
    axes[4] = Vector_Normalize(Vector_New(-1,1,1));
    axes[5] = Vector_Normalize(Vector_New(-1,-1,1));
    axes[6] = Vector_Normalize(Vector_New(1,-1,1));
  }
  areCreated = 1;
  return axes[index];
}

BBOX *BBOX_New(Object *obj) {

  // primitive dont support bbox
  // it will form part of the unbounded objects list
  if(!obj->bbox) return NULL;

  BBOX *bbox = malloc(sizeof(BBOX));
  bbox->obj = obj;
  bbox->next = NULL;

  (*obj->bbox)(bbox);

  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    bbox->centroid[i] = Vector_DivScalar(
      Vector_AddVector(bbox->min[i], bbox->max[i]),
      2
    );
  }

  return bbox;
}

BBOX *BBOXList_New(
  Object *objectList,
  long *bboxListLength,
  Object **unboundedObjectList,
  long *unboundedObjectListLength
) {

  BBOX *first = NULL, *prev = NULL;
  *bboxListLength = 0;
  for(Object *obj = objectList, *aux = NULL; obj; obj = obj->next) {
    BBOX *bbox = BBOX_New(obj);
    // unbounded objects cannot be part of the hierarchy bbox tree
    if(!bbox) {
      if(!aux) {
        aux = *unboundedObjectList = obj;
      } else {
        aux = aux->next = obj;
      }
      ++*unboundedObjectListLength;
    } else {
      if(!prev) {
        prev = first = bbox;
      } else {
        prev = prev->next = bbox;
      }
      ++*bboxListLength;
    }
  }
  return first;
}

BBOXTree *BBOXTree_New(
  Object *objectList,
  long *treeObjectLength,
  Object **unboundedObjectList,
  long *unboundedObjectListLength
) {
  // setup bounding box of each primitive
  // and save them in a BBOX list
  // treeObjectLength will contain
  // the total number of objects that the
  // BBOXTree will hold
  BBOX *bboxList = BBOXList_New(
    objectList,
    treeObjectLength,
    unboundedObjectList,
    unboundedObjectListLength
  );
  //BBOXList_Print(bboxList);

  // build hierarchy tree
  BBOXTree *root = BBOXTree_BuildHierarchy(
    bboxList, *treeObjectLength, 0
  );

  // no more need it
  BBOXList_Free(bboxList);

  return root;
}

static long BBOXList_ToObjectList(BBOX *bboxList, Object **object) {

  if(!bboxList) return 0;

  long totalObjects = 1;
  BBOX *bbox = bboxList;
  Object *aux = *object = bbox->obj;
  for(bbox = bbox->next; bbox; bbox = bbox->next) {
    aux = aux->next = bbox->obj;
    totalObjects++;
  }

  return totalObjects;
}

void BBOXTree_ComputeNodeBBOX(BBOXTree *node, BBOX *bboxList) {

  // look for global min, max
  node->bbox.next = NULL;

  BBOX *bbox = bboxList;
  long totalObjects = 0;
  // use the first bbox as start point
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    totalObjects++;
    node->bbox.min[i] = bbox->min[i];
    node->bbox.max[i] = bbox->max[i];
  }
  // keep iterating
  for(bbox = bbox->next; bbox; bbox = bbox->next) {
    totalObjects++;
    for(long i = 0; i < BBOX_AXES_COUNT; i++) {
      node->bbox.min[i].x = MIN(node->bbox.min[i].x, bbox->min[i].x);
      node->bbox.min[i].y = MIN(node->bbox.min[i].y, bbox->min[i].y);
      node->bbox.min[i].z = MIN(node->bbox.min[i].z, bbox->min[i].z);
      node->bbox.max[i].x = MAX(node->bbox.max[i].x, bbox->max[i].x);
      node->bbox.max[i].y = MAX(node->bbox.max[i].y, bbox->max[i].y);
      node->bbox.max[i].z = MAX(node->bbox.max[i].z, bbox->max[i].z);
    }
  }
  // compute centrod of bounding box
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    node->bbox.centroid[i] = Vector_DivScalar(
      Vector_AddVector(
        node->bbox.min[i], node->bbox.max[i]
      ),
      2
    );
  }
  /*
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    Vector_Print(node->bbox.min[i]);
    Vector_Print(node->bbox.max[i]);
    Vector_Print(node->bbox.centroid[i]);
  }
  */
}

void BBOXTree_GenerateSplitLists(
  BBOXTree *node,
  BBOX *list,
  long listLength,
  BBOX **leftList,
  long *leftListLength,
  BBOX **rightList,
  long *rightListLength
) {

  BBOX **left[BBOX_AXES_COUNT],
       **right[BBOX_AXES_COUNT];
  long leftLength[BBOX_AXES_COUNT] = {0},
      rightLength[BBOX_AXES_COUNT] = {0};

  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    left[i] = calloc(listLength,sizeof(BBOX *));
    right[i] = calloc(listLength,sizeof(BBOX *));
  }

  for(BBOX *bbox = list; bbox; bbox = bbox->next) {
    for(long i = 0; i < BBOX_AXES_COUNT; i++) {
      double dist = Vector_Dot(
        BBOX_GetAxis(i),
        Vector_SubVector(bbox->centroid[i],node->bbox.centroid[i])
      );
      if(dist >= 0) {
        right[i][rightLength[i]++] = bbox;
      } else {
        left[i][leftLength[i]++] = bbox;
      }
    }
  }

  // check which axis is better balanced
  long dist = labs(rightLength[0] - leftLength[0]);
  long axisIndex = 0;
  for(long i = 1; i < BBOX_AXES_COUNT; i++) {
    long tmp = labs(rightLength[i] - leftLength[i]);
    if(tmp < dist) {
      dist = tmp;
      axisIndex = i;
    }
  }

  /*for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    printf("%ld) %ld %ld\n",i,leftLength[i],rightLength[i]);
  }*/

  printf("using axis %ld for splitting\n", axisIndex);
  if(!leftLength[axisIndex] || !rightLength[axisIndex]) {
    printf("FATAL ERROR leftLength[axisIndex] or rightLength[axisIndex] is 0\n");
    exit(0);
  }

  *leftListLength = leftLength[axisIndex];
  BBOXArr_ToList(leftList, left[axisIndex], *leftListLength);

  *rightListLength = rightLength[axisIndex];
  BBOXArr_ToList(rightList, right[axisIndex], *rightListLength);

  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    free(left[i]);
    free(right[i]);
  }
}

void BBOXArr_ToList(
  BBOX **dest,
  BBOX **source,
  long sourceLength
) {

  BBOX *aux = *dest = source[0];
  for(long i = 1; i < sourceLength; i++) {
    aux = aux->next = source[i];
  }
  aux->next = NULL;
}

BBOXTree *BBOXTree_BuildHierarchy(
  BBOX *list,
  long listLength,
  int forceGrouping
) {

  if(!listLength) return NULL;

  BBOXTree *node = malloc(sizeof(BBOXTree));
  node->left = node->right = NULL;
  node->objectList = NULL;
  node->objectListLength = 0;

  BBOXTree_ComputeNodeBBOX(node, list);

  if(!forceGrouping && listLength > BBOXTREE_LEAF_BUNCHING) {
    BBOX *left, *right;
    long leftLength, rightLength;
    BBOXTree_GenerateSplitLists(
      node,
      list, listLength,
      &left, &leftLength,
      &right, &rightLength
    );
    printf("splitting occurred");
    printf("\tleft.length: %ld ", leftLength);
    printf("\tright.length: %ld\n", rightLength);

    node->left = BBOXTree_BuildHierarchy(left, leftLength, 0);
    node->right = BBOXTree_BuildHierarchy(right, rightLength, 0);

  } else {
    node->objectListLength = BBOXList_ToObjectList(
      list, &node->objectList
    );
    printf("leaf generated with %ld children\n", node->objectListLength);
  }

  return node;
}

void BBOX_Print(BBOX *bbox) {

  printf("===== BBOX =====\n");
  printf("object is %d\n",bbox->obj->type);
  (*bbox->obj->print)(bbox->obj->primitive);
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    printf("%ld axis\n",i);
    Vector_Print(bbox->min[i]);
    Vector_Print(bbox->max[i]);
    Vector_Print(bbox->centroid[i]);
  }
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
    printf("level %ld\n", level);
    printf("bounding box is\n");
    for(long i = 0; i < BBOX_AXES_COUNT; i++) {
      printf("%ld) min\n",i); Vector_Print(node->bbox.min[i]);
      printf("%ld) max\n",i); Vector_Print(node->bbox.max[i]);
      printf("%ld) centroid\n",i); Vector_Print(node->bbox.centroid[i]);
    }
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
}

void BBOXList_Free(BBOX *bboxList) {
  for(BBOX *bbox = bboxList; bbox;) {
    BBOX *tmp = bbox->next;
    free(bbox);
    bbox = tmp;
  }
}

void BBOXTree_Free(BBOXTree *tree) {
  if(!tree) return;
  if(tree->objectListLength) return;

  BBOXTree_Free(tree->left);
  BBOXTree_Free(tree->right);

  free(tree);
}

// ********************************

int BBOX_Intersect(Ray ray, BBOX *bbox) {

  double tMins[BBOX_AXES_COUNT], tMaxs[BBOX_AXES_COUNT];
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    tMins[i] = NEGATIVE_INFINITY;
    tMaxs[i] = POSITIVE_INFINITY;
  }
  // calc plane intersection
  for(long i = 0; i < BBOX_AXES_COUNT; i++) {
    Vector axis = BBOX_GetAxis(i);;
    Vector min = bbox->min[i];
    Vector max = bbox->max[i];
    double deno = Vector_Dot(axis, ray.dir);
    //if(gbDebug) printf("axis  %ld -> ",i), Vector_Print(axis);
    //if(gbDebug) printf("\tdeno %5.5f\n", deno);
    if(fabs(deno) < EPSILON) {
      double dist1 = Vector_Dot(axis, Vector_SubVector(min, ray.start));
      double dist2 = Vector_Dot(axis, Vector_SubVector(max, ray.start));
      if(SIGN(dist1) == SIGN(dist2)) {
        //if(gbDebug) printf("\tray is outside bounding box\n");
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
    //if(gbDebug) printf("\tt1 %5.5f,t2 %5.5f,tMin %5.5f,tMax %5.5f\n", t1,t2,tMin,tMax);
    if(tMax < 0) {
      return 0; // not intersection possible
    }

    tMins[i] = tMin;
    tMaxs[i] = tMax;
  }

  double tMin = tMins[0], tMax = tMaxs[0];
  for(long i = 1; i < BBOX_AXES_COUNT; i++) {
    tMin = MAX(tMin,tMins[i]);
    tMax = MIN(tMax,tMaxs[i]);
  }

  if(tMax < tMin) {
    return 0; // not intersecion with bounding box
  }

  return 1;
}

int BBOXTree_NodeIntersect(Ray ray, BBOXTree *node) {
  return BBOX_Intersect(ray, &node->bbox);
}
