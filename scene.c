#include "scene.h"

#define DEBUG 1

static Scene *Scene_New();
static int Scene_AddObject(
  void *primitive,
  Texture *tex,
  int type,
  double (*intersect)(Ray ray, void *primitive),
  Vector (*normal)(Vector point, void *primitive),
  void (*print)(void *primitive),
  void (*free)(void *primitive),
  Scene *scene
);
static void Scene_LoadDefaults(Scene *scene);

static int Scene_GetString(FILE *fp);
static Vector Scene_ParseVector(FILE *fp);
static double Scene_ParseFloat(FILE *fp);
static int Scene_GetTexture(FILE *fp, Texture *tex);

static int Scene_GetAntiAliasing(FILE *fp, Scene *scene);
static int Scene_GetCamera(FILE *fp, Scene *scene);
static int Scene_GetSky(FILE *fp, Scene *scene);
static int Scene_GetLamp(FILE *fp, Scene *scene);
static int Scene_GetSphere(FILE *fp, Scene *scene);
static int Scene_GetPlane(FILE *fp, Scene *scene);

enum {
  FILE_NAME = 0,
  WIDTH, HEIGHT,
  ANTIALIASING,
    NONE,
    MULTI,
    STOCHASTIC,
  LOC, NORMAL,
  CAMERA,
    UPPOINT,
    LOOKAT,
    VIEWERDISTANCE,
    WINDOW,
  SKY,
  TEXTURE,
    COLOR,
    AMBIENT,
    PHONG, PHONG_EXP, METALLIC,
    SCALE,
    MINRADIUS, MAXRADIUS,
  LAMP,
  SPHERE,
    RADIUS,
  PLANE,
  CURLY,
  TOTAL_IDS,
  ERROR
} gbTypes;

char gbStringTypes[][50] = {
  "FILE_NAME",
  "WIDTH", "HEIGHT",
  "ANTI_ALIASING",
    "NONE", "MULTI", "STOCHASTIC",
  "LOC", "NORMAL",
  "CAMERA",
    "UPPOINT",
    "LOOKAT",
    "VIEWERDISTANCE",
    "WINDOW",
  "SKY",
  "TEXTURE",
    "COLOR",
    "AMBIENT",
    "PHONG",
    "PHONG_EXP",
    "METALLIC",
    "SCALE",
    "MINRADIUS",
    "MAXRADIUS",
  "LAMP",
  "SPHERE",
    "RADIUS",
  "PLANE",
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

Vector Scene_ParseVector(FILE *fp) {
  int code;
  double x,y,z;
  code = Scene_GetString(fp);
  x = atof(gbStringBuf);
  code = Scene_GetString(fp);
  y = atof(gbStringBuf);
  code = Scene_GetString(fp);
  z = atof(gbStringBuf);
  return Vector_New(x,y,z);
}

double Scene_ParseFloat(FILE *fp) {
  int code;
  double val;
  code = Scene_GetString(fp);
  val = atof(gbStringBuf);
  return val;
}

int Scene_GetTexture(FILE *fp, Texture *tex) {
  int code;
  double phong = 0; long phongExp = 0; int isMetallic = 0;
  double minRadius = 0, maxRadius = 0;
  Vector tmp;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == COLOR) {
      if(DEBUG) printf("FOUND COLOR\n");
      code = Scene_GetString(fp);
      double limit = atof(gbStringBuf);
      code = Scene_GetString(fp);
      double r = atof(gbStringBuf);
      code = Scene_GetString(fp);
      double g = atof(gbStringBuf);
      code = Scene_GetString(fp);
      double b = atof(gbStringBuf);
      Texture_AddColor(limit,Vector_New(r,g,b),tex);
    } else if(code == AMBIENT) {
      if(DEBUG) printf("FOUND AMBIENT\n");
      code = Scene_GetString(fp);
      double ambient = atof(gbStringBuf);
      Texture_SetAmbient(ambient, tex);
    } else if(code == PHONG) {
      if(DEBUG) printf("FOUND PHONG\n");
      code = Scene_GetString(fp);
      phong = atof(gbStringBuf);
    } else if(code == PHONG_EXP) {
      if(DEBUG) printf("FOUND PHONG_EXP\n");
      code = Scene_GetString(fp);
      phongExp = atol(gbStringBuf);
    } else if(code == METALLIC) {
      if(DEBUG) printf("FOUND METALLIC\n");
      isMetallic = 1;
    } else if(code == SCALE) {
      if(DEBUG) printf("FOUND SCALE\n");
      tmp = Scene_ParseVector(fp);
      Texture_SetScale(tmp,tex);
    } else if(code == MINRADIUS) {
      if(DEBUG) printf("FOUND MIN RADIUS\n");
      code = Scene_GetString(fp);
      minRadius = atof(gbStringBuf);
    } else if(code == MAXRADIUS) {
      if(DEBUG) printf("FOUND MAX RADIUS\n");
      code = Scene_GetString(fp);
      maxRadius = atof(gbStringBuf);
    } else {
      if(DEBUG) printf("TEXTURE PROPERTY INVALID %s\n", gbStringBuf);
      return code;
    }

  }

  Texture_SetPhong(phong, phongExp, isMetallic, tex);
  Texture_SetRadii(minRadius, maxRadius, tex);

  return code;
}

int Scene_AddObject(
  void *primitive,
  Texture *tex,
  int type,
  double (*intersect)(Ray ray, void *primitive),
  Vector (*normal)(Vector point, void *primitive),
  void (*print)(void *primitive),
  void (*free)(void *primitive),
  Scene *scene
) {

  Object *obj = malloc(sizeof(Object));
  if(!obj) return 0;

  obj->primitive = primitive;
  obj->type = type;
  obj->texture = tex;
  obj->intersect = intersect;
  obj->normal = normal;
  obj->print = print;
  obj->free = free;

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
      Vector_New(0,0,-1), // camera location
      Vector_New(0,1,0), // camera up vector (tilt)
      Vector_New(0,0,0), // camera point of interest
      10, // how far viewer is behind camera view plane
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
    scene->lampList = Lamp_New(scene->cam->viewerPos);
  }

}

void Scene_Free(Scene *scene) {
  free(scene->fileName);
  if(scene->objectListLength) {
    for(Object *node = scene->objectList, *next; node; node = next) {
      next = node->next;
      (*node->free)(node->primitive);
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

    } else if(code == WIDTH) {
      if(DEBUG) printf("WIDTH found, ");
      code = Scene_GetString(fp);
      if(DEBUG) printf("value is %s\n",gbStringBuf);
      (*scene)->width = atol(gbStringBuf);

    } else if(code == HEIGHT) {
      if(DEBUG) printf("HEIGHT found, ");
      code = Scene_GetString(fp);
      if(DEBUG) printf("value is %s\n",gbStringBuf);
      (*scene)->height = atol(gbStringBuf);

    } else if(code == ANTIALIASING) {
      if(DEBUG) printf("ANTI_ALIASING found\n");
      code = Scene_GetAntiAliasing(fp, *scene);

    } else if(code == CAMERA) {
      if(DEBUG) printf("CAMERA found\n");
      code = Scene_GetCamera(fp, *scene);

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

int Scene_GetCamera(FILE *fp, Scene *scene) {

  int code;
  Vector loc, lookAt, upPoint;
  double viewerDistance;
  double minX,minY,maxX,maxY;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp);
      if(DEBUG) Vector_Print(loc);

    } else if(code == LOOKAT) {
      if(DEBUG) printf("\tLOOKAT found, ");
      lookAt = Scene_ParseVector(fp);
      if(DEBUG) Vector_Print(lookAt);

    } else if(code == UPPOINT) {
      if(DEBUG) printf("\tUPPOINT found, ");
      upPoint = Scene_ParseVector(fp);
      if(DEBUG) Vector_Print(upPoint);

    } else if(code == VIEWERDISTANCE) {
      if(DEBUG) printf("\tVIEWDISTANE found, ");
      Scene_GetString(fp);
      viewerDistance = atof(gbStringBuf);
      if(DEBUG) printf("%5.5f\n", viewerDistance);

    } else if(code == WINDOW) {
      if(DEBUG) printf("\tWINDOW found, ");
      Scene_GetString(fp);
      minX = atof(gbStringBuf);
      Scene_GetString(fp);
      minY = atof(gbStringBuf);
      Scene_GetString(fp);
      maxX = atof(gbStringBuf);
      Scene_GetString(fp);
      maxY = atof(gbStringBuf);
      if(DEBUG) printf("(%5.5f %5.5f)(%5.5f %5.5f)\n",minX,minY,maxX,maxY);

    } else {
      if(DEBUG) printf("\tCAMERA property invalid: %s\n", gbStringBuf);
      return ERROR;
    }
  }

  scene->cam = Camera_New(
    loc, upPoint, lookAt,
    viewerDistance,
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
      return ERROR;
    }
  }

  tex->type = TEXTURE_SKY;
  scene->sky = tex;

  return code;
}

int Scene_GetLamp(FILE *fp, Scene *scene) {

  int code;
  Vector loc;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp);
      if(DEBUG) Vector_Print(loc);

    } else {
      if(DEBUG) printf("\tLAMP property invalid: %s\n", gbStringBuf);
      return ERROR;
    }

  }

  Lamp *lamp = Lamp_New(loc);
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

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp);
      if(DEBUG) Vector_Print(loc);

    } else if(code == RADIUS) {
      if(DEBUG) printf("\tRADIUS found, ");
      radius = Scene_ParseFloat(fp);
      if(DEBUG) printf("%5.5f\n", radius);

    } else if(code == TEXTURE) {
      if(DEBUG) printf("\tTEXTURE found\n");
      code = Scene_GetTexture(fp,tex);
      if(DEBUG) Texture_Print(tex);

    } else {
      if(DEBUG) printf("\tSPHERE property invalid: %s\n", gbStringBuf);
      return ERROR;
    }

  }

  if(!Scene_AddObject(
    Sphere_New(loc, radius, tex),
    tex,
    OBJ_SPHERE,
    Sphere_Intersect,
    Sphere_Normal,
    Sphere_Print,
    Sphere_Free,
    scene
    )
  ) return ERROR;

  return code;
}

int Scene_GetPlane(FILE *fp, Scene *scene) {

  int code;
  Vector loc, normal;
  Texture *tex = Texture_New();

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == LOC) {
      if(DEBUG) printf("\tLOC found, ");
      loc = Scene_ParseVector(fp);
      if(DEBUG) Vector_Print(loc);

    } else if(code == NORMAL) {
      if(DEBUG) printf("\tNORMAL found, ");
      normal = Scene_ParseVector(fp);
      if(DEBUG) Vector_Print(normal);

    } else if(code == TEXTURE) {
      if(DEBUG) printf("\tTEXTURE found\n");
      code = Scene_GetTexture(fp,tex);
      if(DEBUG) Texture_Print(tex);

    } else {
      if(DEBUG) printf("\tPLANE property invalid: %s\n", gbStringBuf);
      return ERROR;
    }

  }

  if(!Scene_AddObject(
    Plane_New(loc, normal, tex),
    tex,
    OBJ_PLANE,
    Plane_Intersect,
    Plane_Normal,
    Plane_Print,
    Plane_Free,
    scene
    )
  ) return ERROR;

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
