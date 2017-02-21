#include "scene.h"

#define DEBUG 1

static Scene *Scene_New();
static int Scene_AddObject(
  void *primitive,
  int type,
  Hit *(*intersect)(Ray ray, void *primitive),
  Vector (*normal)(Vector point, void *primitive),
  Texture *tex,
  void (*print)(void *primitive),
  void (*free)(void *primitive),
  void (*bbox)(BBOX *bbox),
  ClipList *clipList,
  long clipListLength,
  Scene *scene
);
static void Scene_LoadDefaults(Scene *scene);
static int Scene_GetString(FILE *fp);
static Vector Scene_ParseVector(FILE *fp, int *code);
static double Scene_ParseFloat(FILE *fp, int *code);
static long Scene_ParseLong(FILE *fp, int *code);
static int Scene_GetTexture(FILE *fp, Texture *tex);
static int Scene_GetClip(FILE *fp, int code, ClipList **clipList);
static int Scene_GetClipPlane(FILE *fp, ClipList **node);
static int Scene_GetClipCone(FILE *fp, int clipType, ClipList **node);
static int Scene_GetClipBox(FILE *fp, int clipType, ClipList **node);
static int Scene_GetClipSphere(FILE *fp, int clipType, ClipList **node);
static int Scene_GetAntiAliasing(FILE *fp, Scene *scene);
static int Scene_GetObserver(FILE *fp, Scene *scene);
static int Scene_GetSky(FILE *fp, Scene *scene);
static int Scene_GetLamp(FILE *fp, Scene *scene);
static int Scene_GetSphere(FILE *fp, Scene *scene);
static int Scene_GetPlane(FILE *fp, Scene *scene);
static int Scene_GetPolygon(FILE *fp, Scene *scene);

enum {
  FILE_NAME = 0,
  XRES, YRES,
  ANTIALIASING,
    NONE,
    MULTI,
    STOCHASTIC,
  LOC, NORMAL, RADIUS, APEX, MIN_RADIUS, BASE, MAX_RADIUS,
  OBSERVER,
    UP,
    LOOKAT,
    WINDOW,
  SKY,
  TEXTURE,
    COLOR,
    AMBIENT,
    PHONG, PHONG_EXP, METALLIC,
    REFLECT,
    REFRACT, INDEX,
    SCALE,
    CHECKER,
  CLIP_PLANE,
  CLIP_IN_BOX,
  CLIP_OUT_BOX,
  CLIP_IN_CONE,
  CLIP_OUT_CONE,
  CLIP_IN_SPHERE,
  CLIP_OUT_SPHERE,
  LAMP,
    STRENGTH, FALLOFF,
  SPHERE,
  PLANE,
  POLYGON,
    VERTEX,
  CURLY,
  TOTAL_IDS,
  ERROR
} gbTypes;

char gbStringTypes[][50] = {
  "FILE_NAME",
  "XRES", "YRES",
  "ANTI_ALIASING",
    "NONE",
    "MULTI",
    "STOCHASTIC",
  "LOC", "NORMAL", "RADIUS", "APEX", "MIN_RADIUS", "BASE", "MAX_RADIUS",
  "OBSERVER",
    "UP",
    "LOOKAT",
    "WINDOW",
  "SKY",
  "TEXTURE",
    "COLOR",
    "AMBIENT",
    "PHONG", "PHONG_EXP", "METALLIC",
    "REFLECT",
    "REFRACT", "INDEX",
    "SCALE",
    "CHECKER",
  "CLIP_PLANE",
  "CLIP_IN_BOX",
  "CLIP_OUT_BOX",
  "CLIP_IN_CONE",
  "CLIP_OUT_CONE",
  "CLIP_IN_SPHERE",
  "CLIP_OUT_SPHERE",
  "LAMP",
    "STRENGTH", "FALLOFF",
  "SPHERE",
  "PLANE",
  "POLYGON",
    "VERTEX",
  "}",
  '\0'
};

char gbStringBuf[100];

int Scene_GetString(FILE *fp) {

  char ch;
  int flag = 0;
  int i;

  gbStringBuf[0] = '\0';
  while((ch = toupper(fgetc(fp))) != EOF) {
    if(flag == 1) {
      if(ch == '*') {
        if((ch = fgetc(fp)) == '/') {
          flag = 0;
        }
      }
    } else {
      if(ch == '/') {
        if((ch = fgetc(fp)) == '*') {
          flag = 1;
          continue;
        }
      }
      if(isalnum(ch) || ch == '.' || ch == '-' || ch == '}') {
        gbStringBuf[0] = ch;
        break;
      }
    }
  }
  i = 1;
  while((ch = toupper(fgetc(fp))) != EOF) {
    if(isalnum(ch) || ch == '_' || ch == '.') {
      gbStringBuf[i++] = ch;
    } else {
      gbStringBuf[i] = '\0';
      break;
    }
  }
  for(i = 0; i < TOTAL_IDS; i++) {
    if(strcmp(gbStringBuf, gbStringTypes[i]) == 0) {
      break;
    }
  }
  //printf("String buffer contains: %s which is type %d\n", i);
  return i;
}

Vector Scene_ParseVector(FILE *fp, int *code) {
  double x,y,z;
  *code = Scene_GetString(fp);
  x = atof(gbStringBuf);
  *code = Scene_GetString(fp);
  y = atof(gbStringBuf);
  *code = Scene_GetString(fp);
  z = atof(gbStringBuf);
  return Vector_New(x,y,z);
}

double Scene_ParseFloat(FILE *fp, int *code) {
  double val;
  *code = Scene_GetString(fp);
  val = atof(gbStringBuf);
  return val;
}

long Scene_ParseLong(FILE *fp, int *code) {
  long val;
  *code = Scene_GetString(fp);
  val = atol(gbStringBuf);
  return val;
}

int Scene_GetTexture(FILE *fp, Texture *tex) {
  int code;
  double phong = 0; long phongExp = 0; int isMetallic = 0;
  double rfl = 0, rfr = 0, ior = IOR_VACUUM;
  double minRadius = 0, maxRadius = 0;
  Vector tmp;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == COLOR) {
      if(DEBUG) printf("FOUND COLOR\n");
      double limit = Scene_ParseFloat(fp, &code);
      double r = Scene_ParseFloat(fp, &code);
      double g = Scene_ParseFloat(fp, &code);
      double b = Scene_ParseFloat(fp, &code);
      Texture_AddColor(limit,Vector_New(r,g,b),tex);
    } else if(code == AMBIENT) {
      if(DEBUG) printf("FOUND AMBIENT\n");
      code = Scene_GetString(fp);
      double ambient = atof(gbStringBuf);
      Texture_SetAmbient(ambient, tex);
    } else if(code == PHONG) {
      if(DEBUG) printf("FOUND PHONG\n");
      phong = Scene_ParseFloat(fp, &code);
    } else if(code == PHONG_EXP) {
      if(DEBUG) printf("FOUND PHONG_EXP\n");
      phongExp = Scene_ParseLong(fp, &code);
    } else if(code == METALLIC) {
      if(DEBUG) printf("FOUND METALLIC\n");
      isMetallic = 1;
    } else if(code == REFLECT) {
      if(DEBUG) printf("FOUND REFLECT\n");
      rfl = Scene_ParseFloat(fp, &code);
    } else if(code == REFRACT) {
      if(DEBUG) printf("FOUND REFRACTION\n");
      rfr = Scene_ParseFloat(fp, &code);
    } else if(code == INDEX) {
      if(DEBUG) printf("FOUND INDEX\n");
      ior = Scene_ParseFloat(fp, &code);
    } else if(code == SCALE) {
      if(DEBUG) printf("FOUND SCALE\n");
      tmp = Scene_ParseVector(fp, &code);
      Texture_SetScale(tmp,tex);
    } else if(code == MIN_RADIUS) {
      if(DEBUG) printf("FOUND MIN RADIUS\n");
      minRadius = Scene_ParseFloat(fp, &code);
    } else if(code == MAX_RADIUS) {
      if(DEBUG) printf("FOUND MAX RADIUS\n");
      maxRadius = Scene_ParseFloat(fp, &code);
    } else if(code == CHECKER) {
      if(DEBUG) printf("FOUND CHECKER\n");
      tex->type = TEXTURE_CHECKER;
    } else {
      if(DEBUG) printf("TEXTURE PROPERTY INVALID %s\n", gbStringBuf);
      return code;
    }

  }

  Texture_SetPhong(phong, phongExp, isMetallic, tex);
  Texture_SetReflect(rfl, tex);
  Texture_SetRefraction(rfr, tex);
  Texture_SetIOR(ior, tex);
  Texture_SetRadii(minRadius, maxRadius, tex);

  Texture_Setup(tex);

  return code;
}

int Scene_GetClip(FILE *fp, int code, ClipList **clipList) {

  ClipList *node;
  if(code == CLIP_PLANE) {
    if(DEBUG) printf("CLIP_PLANE found\n");
    code = Scene_GetClipPlane(fp, &node);

  } else if(code == CLIP_IN_CONE) {
    if(DEBUG) printf("CLIP_IN_CONE found\n");
    code = Scene_GetClipCone(fp, CLIP_IN, &node);

  } else if(code == CLIP_OUT_CONE) {
    if(DEBUG) printf("CLIP_OUT_CONE found\n");
    code = Scene_GetClipCone(fp, CLIP_OUT, &node);

  } else if(code == CLIP_IN_BOX) {
    if(DEBUG) printf("CLIP_IN_BOX found\n");
    code = Scene_GetClipBox(fp, CLIP_IN, &node);

  } else if(code == CLIP_OUT_BOX) {
    if(DEBUG) printf("CLIP_OUT_BOX found\n");
    code = Scene_GetClipBox(fp, CLIP_OUT, &node);

  } else if(code == CLIP_IN_SPHERE) {
    if(DEBUG) printf("CLIP_IN_SPHERE found\n");
    code = Scene_GetClipSphere(fp, CLIP_IN, &node);

  } else if(code == CLIP_OUT_SPHERE) {
    if(DEBUG) printf("CLIP_OUT_SPHERE found\n");
    code = Scene_GetClipSphere(fp, CLIP_OUT, &node);

  } else {
    if(DEBUG) printf("CLIP property unknown %s\n",gbStringBuf);
    return 0;
  }

  ClipList_InsertNode(node, clipList);
  return 1;
}

int Scene_GetClipPlane(FILE *fp, ClipList **node) {

  int code;
  Vector loc, normal;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(loc);

    } else if(code == NORMAL) {
      if(DEBUG) printf("\tNORMAL found, ");
      normal = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(normal);

    } else {
      if(DEBUG) printf("\tCLIP_PLANE property invalid: %s\n", gbStringBuf);
      return ERROR;
    }
  }

  ClipPlane *clipper = ClipPlane_New(loc, normal);
  if(!clipper) return ERROR;

  *node = ClipList_NewNode(
    clipper, ClipPlane_Clip, ClipPlane_Print
  );
  if(!*node) {
    Clipper_Free(clipper);
    return ERROR;
  }

  return code;
}

int Scene_GetClipCone(FILE *fp, int clipType, ClipList **node) {

  int code;
  Vector apex, base;
  double minRadius, maxRadius;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == APEX) {
      if(DEBUG) printf("\tAPEX found, ");
      apex = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(apex);

    } else if(code == BASE) {
      if(DEBUG) printf("\tBASE found, ");
      base = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(base);

    } else if(code == MIN_RADIUS) {
      if(DEBUG) printf("\tMIN_RADIUS found, ");
      minRadius = Scene_ParseFloat(fp, &code);
      if(DEBUG) printf("%f\n", minRadius);

    } else if(code == MAX_RADIUS) {
      if(DEBUG) printf("\tMAX_RADIUS found, ");
      maxRadius = Scene_ParseFloat(fp, &code);
      if(DEBUG) printf("%f\n", maxRadius);

    } else {
      if(DEBUG) printf("\tCLIP_CONE property invalid: %s\n", gbStringBuf);
      return ERROR;
    }
  }

  ClipCone *clipper = ClipCone_New(
    apex, minRadius, base, maxRadius, clipType
  );
  if(!clipper) return ERROR;

  *node = ClipList_NewNode(
    clipper, ClipCone_Clip, ClipCone_Print
  );
  if(!*node) {
    Clipper_Free(clipper);
    return ERROR;
  }

  return code;
}

int Scene_GetClipBox(FILE *fp, int clipType, ClipList **node) {

  int code;
  Vector apex, base;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == APEX) {
      if(DEBUG) printf("\tAPEX found, ");
      apex = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(apex);

    } else if(code == BASE) {
      if(DEBUG) printf("\tBASE found, ");
      base = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(base);

    } else {
      if(DEBUG) printf("\tCLIP_BOX property invalid: %s\n", gbStringBuf);
      return ERROR;
    }
  }

  ClipBox *clipper = ClipBox_New(apex, base, clipType);
  if(!clipper) return ERROR;

  *node = ClipList_NewNode(
    clipper, ClipBox_Clip, ClipBox_Print
  );
  if(!*node) {
    Clipper_Free(clipper);
    return ERROR;
  }

  return code;
}

static int Scene_GetClipSphere(FILE *fp, int clipType, ClipList **node) {

  int code;
  Vector loc;
  double radius;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(loc);

    } else if(code == RADIUS) {
      if(DEBUG) printf("\tRADIUS found, ");
      radius = Scene_ParseFloat(fp, &code);
      if(DEBUG) printf("%f\n",radius);

    } else {
      if(DEBUG) printf("\tCLIP_SPHERE property invalid: %s\n", gbStringBuf);
      return ERROR;
    }
  }

  ClipSphere *clipper = ClipSphere_New(loc, radius, clipType);
  if(!clipper) return ERROR;

  *node = ClipList_NewNode(
    clipper, ClipSphere_Clip, ClipSphere_Print
  );
  if(!*node) {
    Clipper_Free(clipper);
    return ERROR;
  }

  return code;
}

int Scene_AddObject(
  void *primitive,
  int type,
  Hit *(*intersect)(Ray ray, void *primitive),
  Vector (*normal)(Vector point, void *primitive),
  Texture *tex,
  void (*print)(void *primitive),
  void (*free)(void *primitive),
  void (*bbox)(BBOX *bbox),
  ClipList *clipList,
  long clipListLength,
  Scene *scene
) {

  Object *obj = malloc(sizeof(Object));
  if(!obj) return 0;

  obj->primitive = primitive;
  obj->type = type;
  obj->intersect = intersect;
  obj->normal = normal;
  obj->texture = tex;
  obj->print = print;
  obj->free = free;
  obj->bbox = bbox;
  obj->clipList = clipList;
  obj->clipListLength = clipListLength;

  obj->next = scene->objectList;
  scene->objectList = obj;

  scene->objectListLength++;

  return 1;
}

Scene *Scene_New() {

  Scene *scene = malloc(sizeof(Scene));
  if(!scene) return NULL;
  scene->fileName = malloc(256);
  if(!scene->fileName) return NULL;
  scene->width = 0;
  scene->height = 0;

  scene->cam = NULL;

  scene->objectListLength = 0;
  scene->objectList = NULL;

  scene->sky = NULL;

  scene->lampList = NULL;
  scene->lampListLength = 0;

  return scene;
}

void Scene_LoadDefaults(Scene *scene) {

  if(!scene->cam) {
    scene->cam = Camera_New(
      Vector_New(0,0,-11), // camera location
      Vector_New(0,1,0), // camera up vector (tilt)
      Vector_New(0,0,-1), // camera point of interest
      Vector2d_New(-10,-10), Vector2d_New(10,10) // view plane dims
    );
  }
  if(!scene->sky) {
    Texture *tex = Texture_New();
    tex->type = TEXTURE_SKY;
    Texture_AddColor(0,Vector_FromRGB(RGB_New(70,70,70)),tex);
    scene->sky = tex;
  }
  if(!scene->lampList) {
    scene->lampList = Lamp_New(
      scene->cam->pos,
      LAMP_STRENGTH_DEFAULT,
      LAMP_FALLOFF_DEFAULT
    );
  }
}

void Scene_Free(Scene *scene) {
  free(scene->fileName);
  Camera_Free(scene->cam);
  if(scene->objectListLength) {
    for(Object *node = scene->objectList, *next; node; node = next) {
      next = node->next;
      (*node->free)(node->primitive);
      if(node->clipListLength) ClipList_Free(node->clipList);
      free(node);
    }
  }
  if(scene->lampListLength) {
    Lamp_Free(scene->lampList);
  }
  Texture_Free(scene->sky);
  free(scene);
}

int Scene_Setup(FILE *fp, Scene **scene) {

  *scene = Scene_New();
  if(!*scene) return 0;

  while(!feof(fp)) {
    int code = Scene_GetString(fp);
    if(code == FILE_NAME) {
      if(DEBUG) printf("FILE_NAME found, ");
      code = Scene_GetString(fp);
      if(DEBUG) printf("value is %s\n",gbStringBuf);
      strcpy((*scene)->fileName,gbStringBuf);

    } else if(code == XRES) {
      if(DEBUG) printf("XRES found, ");
      code = Scene_GetString(fp);
      if(DEBUG) printf("value is %s\n",gbStringBuf);
      (*scene)->width = atol(gbStringBuf);

    } else if(code == YRES) {
      if(DEBUG) printf("YRES found, ");
      code = Scene_GetString(fp);
      if(DEBUG) printf("value is %s\n",gbStringBuf);
      (*scene)->height = atol(gbStringBuf);

    } else if(code == ANTIALIASING) {
      if(DEBUG) printf("ANTI_ALIASING found\n");
      code = Scene_GetAntiAliasing(fp, *scene);

    } else if(code == OBSERVER) {
      if(DEBUG) printf("OBSERVER found\n");
      code = Scene_GetObserver(fp, *scene);

    } else if(code == SKY) {
      if(DEBUG) printf("SKY found\n");
      code = Scene_GetSky(fp, *scene);

    } else if(code == LAMP) {
      if(DEBUG) printf("LAMP found\n");
      code = Scene_GetLamp(fp, *scene);

    } else if(code == SPHERE) {
      if(DEBUG) printf("SPHERE found\n");
      code = Scene_GetSphere(fp, *scene);

    } else if(code == PLANE) {
      if(DEBUG) printf("PLANE found\n");
      code = Scene_GetPlane(fp, *scene);

    } else if(code == POLYGON) {
      if(DEBUG) printf("POLYGON found\n");
      code = Scene_GetPolygon(fp, *scene);

    } else {
      if(strlen(gbStringBuf)) {
        if(DEBUG) printf("UNKNOWN ID NAME: %s\n",gbStringBuf);
        return 0;
      }
    }

    if(code == ERROR) {
      if(DEBUG) printf("SOMETHING WENT WRONG\n");
      return 0;
    }
  }

  Scene_LoadDefaults(*scene);

  return 1;
}

int Scene_GetAntiAliasing(FILE *fp, Scene *scene) {

  int code = Scene_GetString(fp);
  if(code == NONE) {
    if(DEBUG) printf("AA is NONE\n");
    scene->aa = AA_NONE;
  } else if(code == MULTI) {
    if(DEBUG) printf("AA is MULTI\n");
    scene->aa = AA_MULTI;
  } else if(code == STOCHASTIC) {
    if(DEBUG) printf("AA is STOCHASTIC\n");
    scene->aa = AA_STOCHASTIC;
  } else {
    if(DEBUG) printf("ANTI_ALIASING unknown property value %s\n",gbStringBuf);
    code = ERROR;
  }

  return code;
}

int Scene_GetObserver(FILE *fp, Scene *scene) {

  int code;
  Vector loc, lookAt, up = Vector_New(0,1,0);
  double minX,minY,maxX,maxY;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(loc);

    } else if(code == LOOKAT) {
      if(DEBUG) printf("\tLOOKAT found, ");
      lookAt = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(lookAt);

    } else if(code == UP) {
      if(DEBUG) printf("\tUP found, ");
      up = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(up);

    } else if(code == WINDOW) {
      if(DEBUG) printf("\tWINDOW found, ");
      minX = Scene_ParseFloat(fp, &code);
      minY = Scene_ParseFloat(fp, &code);
      maxX = Scene_ParseFloat(fp, &code);
      maxY = Scene_ParseFloat(fp, &code);
      if(DEBUG) printf("(%5.5f %5.5f)(%5.5f %5.5f)\n",minX,minY,maxX,maxY);

    } else {
      if(DEBUG) printf("\tCAMERA property invalid: %s\n", gbStringBuf);
      return ERROR;
    }
  }

  // already have a camera? if so, overwrite
  if(scene->cam) Camera_Free(scene->cam);
  scene->cam = Camera_New(
    loc, up, lookAt,
    Vector2d_New(minX,minY), Vector2d_New(maxX,maxY)
  );

  return code;
}

int Scene_GetSky(FILE *fp, Scene *scene) {

  int code;
  Texture *tex = Texture_New();

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == TEXTURE) {
      if(DEBUG) printf("\tTEXTURE found\n");
      code = Scene_GetTexture(fp,tex);
      if(DEBUG) Texture_Print(tex);

    } else {
      if(DEBUG) printf("\tSKY property invalid: %s\n", gbStringBuf);
      Texture_Free(tex);
      return ERROR;
    }
  }

  // allow duplicated sky declarations, just overwrite
  if(scene->sky) Texture_Free(scene->sky);
  tex->type = TEXTURE_SKY;
  scene->sky = tex;

  return code;
}

int Scene_GetLamp(FILE *fp, Scene *scene) {

  int code;
  Vector loc;
  double strength = LAMP_STRENGTH_DEFAULT,
         falloff = LAMP_FALLOFF_DEFAULT;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(loc);

    } else if(code == STRENGTH) {
      if(DEBUG) printf("\tSTRENGTH found, ");
      strength = Scene_ParseFloat(fp, &code);
      if(DEBUG) printf("strength: %f\n", strength);

    } else if(code == FALLOFF) {
      if(DEBUG) printf("\tFALLOFF found, ");
      falloff = Scene_ParseFloat(fp, &code);
      if(DEBUG) printf("falloff: %f\n", falloff);

    } else {
      if(DEBUG) printf("\tLAMP property invalid: %s\n", gbStringBuf);
      return ERROR;
    }
  }

  Lamp *lamp = Lamp_New(loc, strength, falloff);
  lamp->next = scene->lampList;
  scene->lampList = lamp;

  scene->lampListLength++;

  return code;
}

int Scene_GetSphere(FILE *fp, Scene *scene) {

  int code;
  Vector loc;
  double radius;
  Texture *tex = Texture_New();
  if(!tex) return ERROR;
  ClipList *clipList = NULL;
  long clipListLength = 0;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(loc);

    } else if(code == RADIUS) {
      if(DEBUG) printf("\tRADIUS found, ");
      radius = Scene_ParseFloat(fp, &code);
      if(DEBUG) printf("%f\n", radius);

    } else if(code == TEXTURE) {
      if(DEBUG) printf("\tTEXTURE found\n");
      code = Scene_GetTexture(fp,tex);
      if(DEBUG) Texture_Print(tex);

    } else if(Scene_GetClip(fp, code, &clipList)) {
      clipListLength++;

    } else {
      if(DEBUG) printf("\tSPHERE property invalid: %s\n", gbStringBuf);
      Texture_Free(tex);
      ClipList_Free(clipList);
      return ERROR;
    }
  }

  Sphere *sphere = Sphere_New(loc, radius, tex);
  if(!sphere) {
    Texture_Free(tex);
    ClipList_Free(clipList);
    return ERROR;
  }

  if(!Scene_AddObject(
    sphere,
    OBJ_SPHERE,
    Sphere_Intersect,
    Sphere_Normal,
    tex,
    Sphere_Print,
    Sphere_Free,
    Sphere_BBOX,
    clipList,
    clipListLength,
    scene
    )
  ) {
    Sphere_Free(sphere);
    ClipList_Free(clipList);
    return ERROR;
  }

  return code;
}

int Scene_GetPlane(FILE *fp, Scene *scene) {

  int code;
  Vector loc, normal;
  Texture *tex = Texture_New();
  if(!tex) return ERROR;
  ClipList *clipList = NULL;
  long clipListLength = 0;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(loc);

    } else if(code == NORMAL) {
      if(DEBUG) printf("\tNORMAL found, ");
      normal = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(normal);

    } else if(code == TEXTURE) {
      if(DEBUG) printf("\tTEXTURE found\n");
      code = Scene_GetTexture(fp,tex);
      if(DEBUG) Texture_Print(tex);

    } else if(Scene_GetClip(fp, code, &clipList)) {
      clipListLength++;

    } else {
      if(DEBUG) printf("\tPLANE property invalid: %s\n", gbStringBuf);
      Texture_Free(tex);
      ClipList_Free(clipList);
      return ERROR;
    }
  }

  Plane *plane = Plane_New(loc, normal, tex);
  if(!plane) {
    Texture_Free(tex);
    ClipList_Free(clipList);
    return ERROR;
  }

  if(!Scene_AddObject(
    plane,
    OBJ_PLANE,
    Plane_Intersect,
    Plane_Normal,
    tex,
    Plane_Print,
    Plane_Free,
    NULL,
    clipList,
    clipListLength,
    scene
    )
  ) {
    Plane_Free(plane);
    ClipList_Free(clipList);
    return ERROR;
  }

  return code;
}

int Scene_GetPolygon(FILE *fp, Scene *scene) {

  int code;
  Vector loc, normal;
  Texture *tex = Texture_New();
  if(!tex) return ERROR;
  Polygon *poly = Polygon_New(tex);
  if(!poly) {
    Texture_Free(tex);
    return ERROR;
  }
  Vector *vertices = NULL;
  long verticesLength = 0;
  ClipList *clipList = NULL;
  long clipListLength = 0;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(loc);

    } else if(code == NORMAL) {
      if(DEBUG) printf("\tNORMAL found, ");
      normal = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(normal);

    } else if(code == TEXTURE) {
      if(DEBUG) printf("\tTEXTURE found\n");
      code = Scene_GetTexture(fp,tex);
      if(DEBUG) Texture_Print(tex);

    } else if(code == VERTEX) {
      if(DEBUG) printf("\tVERTEX found\n");
      Vector vertex = Scene_ParseVector(fp, &code);
      if(DEBUG) Vector_Print(vertex);
      vertices = realloc(vertices,sizeof(Vector)*(verticesLength+1));
      vertices[verticesLength++] = vertex;

    } else if(Scene_GetClip(fp, code, &clipList)) {
      clipListLength++;

    } else {
      if(DEBUG) printf("\tPOLYGON property invalid: %s\n", gbStringBuf);
      if(vertices) free(vertices);
      Polygon_Free(poly);
      ClipList_Free(clipList);
      return ERROR;
    }
  }

  if(verticesLength < 3) {
    if(DEBUG) printf("\tPOLYGON missing VERTEX or VERTEX count less than 3\n");
    if(vertices) free(vertices);
    Polygon_Free(poly);
    ClipList_Free(clipList);
    return ERROR;
  }

  if(!Polygon_Setup(verticesLength, vertices, poly)) {
    free(vertices);
    Polygon_Free(poly);
    ClipList_Free(clipList);
    return ERROR;
  }
  free(vertices);

  if(!Scene_AddObject(
    poly,
    OBJ_POLYGON,
    Polygon_Intersect,
    Polygon_Normal,
    tex,
    Polygon_Print,
    Polygon_Free,
    Polygon_BBOX,
    clipList,
    clipListLength,
    scene
    )
  ) {
    Polygon_Free(poly);
    ClipList_Free(clipList);
    return ERROR;
  }

  return code;
}

void Scene_Print(Scene *scene) {

  printf("==== SCENE SETUP ====\n");
  printf("fileName: %s\n", scene->fileName);
  printf("width: %ld\n", scene->width);
  printf("height: %ld\n", scene->height);
  printf("antiAliasing: %d\n", scene->aa);
  printf("= CAMERA =====\n");
  Camera_Print(scene->cam);
  printf("= LAMP =====\n");
  for(Lamp *lamp = scene->lampList; lamp; lamp = lamp->next) {
    Lamp_Print(lamp);
  }
  printf("= SKY =====\n");
  Texture_Print(scene->sky);
  printf("= OBJECTS ====\n");
  printf("total objects are: %ld\n", scene->objectListLength);
  for(Object *node = scene->objectList; node; node = node->next) {
    printf("I am a: %d\n", node->type);
    (*node->print)(node->primitive);
  }
  printf("==== END ====\n");
}
