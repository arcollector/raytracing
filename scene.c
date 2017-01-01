#include "scene.h"

#define DEBUG 1

#define FILE_NAME 0
#define WIDTH 1
#define HEIGHT 2
#define SHOOT_TYPE 3
#define SINGLE 4
#define STOCHASTIC 5
#define LOC 6
#define NORMAL 7
#define CAMERA 8
#define UPPOINT 9
#define LOOKAT 10
#define VIEWERDISTANCE 11
#define WINDOW 12
#define SKY 13
#define TEXTURE 14
#define COLOR 15
#define SPHERE 16
#define RADIUS 17
#define PLANE 18
#define CURLY 19
#define TOTAL_IDS 20
#define ERROR 21

char stringTypes[][50] = {
  "FILE_NAME",
  "WIDTH", "HEIGHT",
  "SHOOT_TYPE", "SINGLE", "STOCHASTIC",
  "LOC", "NORMAL",
  "CAMERA", "UPPOINT", "LOOKAT", "VIEWERDISTANCE", "WINDOW",
  "SKY",
  "TEXTURE", "COLOR",
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

int Scene_GetTexture(FILE *fp, RGB *color) {
  int code;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == COLOR) {
      code = Scene_GetString(fp);
      color->red = (unsigned char)atoi(stringBuf);
      code = Scene_GetString(fp);
      color->green = (unsigned char)atoi(stringBuf);
      code = Scene_GetString(fp);
      color->blue = (unsigned char)atoi(stringBuf);

    } else {
      if(DEBUG) printf("TEXTURE PROPERTY INVALID %s\n", stringBuf);
    }

  }

  return code;
}

Object *Scene_AddBlankObject(Scene *scene) {

  Object *obj = scene->objList;
  if(scene->objectsTotal == 0) {
    obj = scene->objList = malloc(sizeof(Object));
    if(!obj) return NULL;
  } else {
    for(long i = 0; i < scene->objectsTotal - 1; i++) {
      obj = obj->next;
    }
    obj = obj->next = malloc(sizeof(Object));
    if(!obj) return NULL;
  }

  obj->next = NULL;
  scene->objectsTotal++;

  return obj;
}

Scene *Scene_New() {

  Scene *scene = malloc(sizeof(Scene));
  scene->fileName = malloc(256);
  scene->width = 0;
  scene->height = 0;

  scene->objectsTotal = 0;
  scene->objList = NULL;

  scene->shootType = SINGLE;

  return scene;
}

void Scene_Free(Scene *scene) {
  free(scene->fileName);
  if(scene->objectsTotal) {
    for(Object *node = scene->objList, *next; node; node = next) {
      next = node->next;
      (*node->free)(node->obj);
      free(node);
    }
  }
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

    } else if(code == SHOOT_TYPE) {
      if(DEBUG) printf("SHOOT_TYPE found\n");
      code = Scene_GetShootType(fp, scene);

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

int Scene_GetShootType(FILE *fp, Scene *scene) {

  int code = Scene_GetString(fp);
  if(code == SINGLE) {
    if(DEBUG) printf("shoot type is SINGLE\n");
    scene->shootType = SINGLE;
  } else if(code == STOCHASTIC) {
    if(DEBUG) printf("shoot type is STOCHASTIC\n");
    scene->shootType = STOCHASTIC;
  } else {
    if(DEBUG) printf("SHOOT_TYPE unknown property value %s\n",stringBuf);
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
  RGB color;

  while(!feof(fp) &&
        (code = Scene_GetString(fp)) != CURLY) {

    if(code == TEXTURE) {
      if(DEBUG) printf("\tTEXTURE found\n");
      code = Scene_GetTexture(fp,&color);
      if(DEBUG) printf("\t\tCOLOR is %d %d %d\n",color.red,color.green,color.blue);

    } else {
      if(DEBUG) printf("\tSKY property invalid: %s\n", stringBuf);
      return ERROR;
    }

  }

  scene->bkgColor = color;

  return code;
}

int Scene_GetSphere(FILE *fp, Scene *scene) {

  int code;
  Vector loc;
  double radius;
  RGB color;

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
      code = Scene_GetTexture(fp,&color);
      if(DEBUG) printf("\t\tCOLOR is %d %d %d\n",color.red,color.green,color.blue);

    } else {
      if(DEBUG) printf("\tSPHERE property invalid: %s\n", stringBuf);
      return ERROR;
    }

  }

  Object *node = Scene_AddBlankObject(scene);
  if(!node) {
    return ERROR;
  }

  node->obj = Sphere_New(loc, radius, color, scene->cam);
  node->print = Sphere_Print;
  node->intersect = Sphere_Intersect;
  node->getColor = Sphere_GetColor;
  node->free = Sphere_Free;
  node->type = SPHERE;

  return code;
}

int Scene_GetPlane(FILE *fp, Scene *scene) {

  int code;
  Vector loc, normal;
  RGB color;

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
      code = Scene_GetTexture(fp,&color);
      if(DEBUG) printf("\t\tCOLOR is %d %d %d\n",color.red,color.green,color.blue);

    } else {
      if(DEBUG) printf("\tPLANE property invalid: %s\n", stringBuf);
      return ERROR;
    }

  }

  Object *node = Scene_AddBlankObject(scene);
  if(!node) {
    return ERROR;
  }

  node->obj = Plane_New(loc, normal, color, scene->cam);
  node->print = Plane_Print;
  node->intersect = Plane_Intersect;
  node->getColor = Plane_GetColor;
  node->free = Plane_Free;
  node->type = PLANE;

  return code;
}

void Scene_Print(Scene *scene) {

  printf("==== SCENE SETUP ====\n");
  printf("fileName: %s\n", scene->fileName);
  printf("width: %ld\n", scene->width);
  printf("height: %ld\n", scene->height);
  printf("shootType: %d\n", scene->shootType);
  printf("= CAMERA =====\n");
  Camera_Print(scene->cam);
  printf("= SKY =====\n");
  printf("color is %d %d %d\n",scene->bkgColor.red,scene->bkgColor.green,scene->bkgColor.blue);
  printf("= OBJECTS ====\n");
  printf("total objects are: %ld\n", scene->objectsTotal);
  for(Object *node = scene->objList; node; node = node->next) {
    printf("I am a: %d\n", node->type);
    (*node->print)(node->obj);
  }
  printf("==== END ====\n");

}

int Scene_isShootStochastic(Scene *scene) {
  return scene->shootType == STOCHASTIC;
}

int Scene_isShootSingle(Scene *scene) {
  return scene->shootType == SINGLE;
}
