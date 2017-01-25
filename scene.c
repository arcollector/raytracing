#include "scene.h"

#define DEBUG 1

#define FILE_NAME 0
#define WIDTH 1
#define HEIGHT 2
#define ANTIALIASING 3
#define NONE 4
#define MULTI 5
#define STOCHASTIC 6
#define LOC 7
#define NORMAL 8
#define CAMERA 9
#define UPPOINT 10
#define LOOKAT 11
#define VIEWERDISTANCE 12
#define WINDOW 13
#define SKY 14
#define TEXTURE 15
#define COLOR 16
#define MINRADIUS 17
#define MAXRADIUS 18
#define SPHERE 19
#define RADIUS 20
#define PLANE 21
#define CURLY 22
#define TOTAL_IDS 23
#define ERROR 24

char stringTypes[][50] = {
  "FILE_NAME",
  "WIDTH", "HEIGHT",
  "ANTI_ALIASING", "NONE", "MULTI", "STOCHASTIC",
  "LOC", "NORMAL",
  "CAMERA", "UPPOINT", "LOOKAT", "VIEWERDISTANCE", "WINDOW",
  "SKY",
  "TEXTURE", "COLOR", "MINRADIUS", "MAXRADIUS",
  "SPHERE", "RADIUS",
  "PLANE",
  "}",
  '\0'
};

char stringBuf[100];

int Scene_GetString(FILE *fp) {

  char ch;
  int flag = 0;
  int i;

  stringBuf[0] = '\0';
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
        stringBuf[0] = ch;
        break;
      }
    }
  }
  i = 1;
  while((ch = toupper(fgetc(fp))) != EOF) {
    if(isalnum(ch) || ch == '_' || ch == '.') {
      stringBuf[i++] = ch;
    } else {
      stringBuf[i] = '\0';
      break;
    }
  }
  for(i = 0; i < TOTAL_IDS; i++) {
    if(strcmp(stringBuf, stringTypes[i]) == 0) {
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
  x = atof(stringBuf);
  code = Scene_GetString(fp);
  y = atof(stringBuf);
  code = Scene_GetString(fp);
  z = atof(stringBuf);
  return Vector_New(x,y,z);
}

double Scene_ParseFloat(FILE *fp) {
  int code;
  double val;
  code = Scene_GetString(fp);
  val = atof(stringBuf);
  return val;
}

int Scene_GetTexture(FILE *fp, Texture *tex) {
  int code;
  double minRadius = 0, maxRadius = 0;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == COLOR) {
      if(DEBUG) printf("FOUND COLOR\n");
      code = Scene_GetString(fp);
      double limit = atof(stringBuf);
      code = Scene_GetString(fp);
      double r = (unsigned char)atof(stringBuf);
      code = Scene_GetString(fp);
      double g = (unsigned char)atof(stringBuf);
      code = Scene_GetString(fp);
      double b = (unsigned char)atof(stringBuf);
      Texture_AddColor(limit, Vector_New(r,g,b), tex);
    } else if(code == MINRADIUS) {
      if(DEBUG) printf("FOUND MIN RADIUS\n");
      code = Scene_GetString(fp);
      minRadius = atof(stringBuf);
    } else if(code == MAXRADIUS) {
      if(DEBUG) printf("FOUND MAX RADIUS\n");
      code = Scene_GetString(fp);
      maxRadius = atof(stringBuf);
    } else {
      if(DEBUG) printf("TEXTURE PROPERTY INVALID %s\n", stringBuf);
      return code;
    }

  }

  Texture_SetRadii(minRadius, maxRadius, tex);

  return code;
}

Object *Scene_AddBlankObject(Scene *scene) {

  Object *obj = scene->objectList;
  if(scene->objectListLength == 0) {
    obj = scene->objectList = malloc(sizeof(Object));
    if(!obj) return NULL;
  } else {
    for(long i = 0; i < scene->objectListLength - 1; i++) {
      obj = obj->next;
    }
    obj = obj->next = malloc(sizeof(Object));
    if(!obj) return NULL;
  }

  obj->next = NULL;
  scene->objectListLength++;

  return obj;
}

Scene *Scene_New() {

  // TODO: create a scene with various defaults
  // for an easy test scenes

  Scene *scene = malloc(sizeof(Scene));
  scene->fileName = malloc(256);
  scene->width = 0;
  scene->height = 0;

  scene->objectListLength = 0;
  scene->objectList = NULL;

  scene->sky = NULL;

  scene->aa = AA_NONE;

  return scene;
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
  Texture_Free(scene->sky);
  free(scene);
}

int Scene_Setup(FILE *fp, Scene *scene) {

  while(!feof(fp)) {
    int code = Scene_GetString(fp);
    if(code == FILE_NAME) {
      if(DEBUG) printf("FILE_NAME found, ");
      code = Scene_GetString(fp);
      if(DEBUG) printf("value is %s\n",stringBuf);
      strcpy(scene->fileName,stringBuf);

    } else if(code == WIDTH) {
      if(DEBUG) printf("WIDTH found, ");
      code = Scene_GetString(fp);
      if(DEBUG) printf("value is %s\n",stringBuf);
      scene->width = atol(stringBuf);

    } else if(code == HEIGHT) {
      if(DEBUG) printf("HEIGHT found, ");
      code = Scene_GetString(fp);
      if(DEBUG) printf("value is %s\n",stringBuf);
      scene->height = atol(stringBuf);

    } else if(code == ANTIALIASING) {
      if(DEBUG) printf("ANTI_ALIASING found\n");
      code = Scene_GetAntiAliasing(fp, scene);

    } else if(code == CAMERA) {
      if(DEBUG) printf("CAMERA found\n");
      code = Scene_GetCamera(fp, scene);

    } else if(code == SKY) {
      if(DEBUG) printf("SKY found\n");
      code = Scene_GetSky(fp, scene);

    } else if(code == SPHERE) {
      if(DEBUG) printf("SPHERE found\n");
      code = Scene_GetSphere(fp, scene);

    } else if(code == PLANE) {
      if(DEBUG) printf("PLANE found\n");
      code = Scene_GetPlane(fp, scene);

    } else {
      if(strlen(stringBuf)) {
        if(DEBUG) printf("UNKNOWN ID NAME: %s\n",stringBuf);
        return 0;
      }
    }

    if(code == ERROR) {
      if(DEBUG) printf("SOMETHING WENT WRONG\n");
      return 0;
    }
  }

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
    if(DEBUG) printf("ANTI_ALIASING unknown property value %s\n",stringBuf);
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
      viewerDistance = atof(stringBuf);
      if(DEBUG) printf("%5.5f\n", viewerDistance);

    } else if(code == WINDOW) {
      if(DEBUG) printf("\tWINDOW found, ");
      Scene_GetString(fp);
      minX = atof(stringBuf);
      Scene_GetString(fp);
      minY = atof(stringBuf);
      Scene_GetString(fp);
      maxX = atof(stringBuf);
      Scene_GetString(fp);
      maxY = atof(stringBuf);
      if(DEBUG) printf("(%5.5f %5.5f)(%5.5f %5.5f)\n",minX,minY,maxX,maxY);

    } else {
      if(DEBUG) printf("\tCAMERA property invalid: %s\n", stringBuf);
      return ERROR;
    }
  }

  scene->cam = Camera_New(
    loc, upPoint, lookAt,
    viewerDistance,
    Vector_New(minX,minY,0),Vector_New(maxX,maxY,0)
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
      if(DEBUG) printf("\tSKY property invalid: %s\n", stringBuf);
      return ERROR;
    }

  }

  scene->sky = tex;

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
      if(DEBUG) printf("\tSPHERE property invalid: %s\n", stringBuf);
      return ERROR;
    }

  }

  Object *node = Scene_AddBlankObject(scene);
  if(!node) {
    return ERROR;
  }

  node->primitive = Sphere_New(loc, radius, tex, scene->cam);
  node->print = Sphere_Print;
  node->intersect = Sphere_Intersect;
  node->getColor = Sphere_GetColor;
  node->free = Sphere_Free;
  node->type = OBJ_SPHERE;

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
      if(DEBUG) printf("\tPLANE property invalid: %s\n", stringBuf);
      return ERROR;
    }

  }

  Object *node = Scene_AddBlankObject(scene);
  if(!node) {
    return ERROR;
  }

  node->primitive = Plane_New(loc, normal, tex, scene->cam);
  node->print = Plane_Print;
  node->intersect = Plane_Intersect;
  node->getColor = Plane_GetColor;
  node->free = Plane_Free;
  node->type = OBJ_PLANE;

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
