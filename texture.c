#include "texture.h"

static Vector Texture_GetColorAt(double t, Texture *tex);

static double Texture_Sky(Vector p, Texture *tex);
static double Texture_Spherical(Vector p, Texture *tex);
static Vector Texture_Checker(Vector p, Texture *tex);

Texture *Texture_New() {
  Texture *tex = malloc(sizeof(Texture));
  if(!tex) return NULL;
  tex->length = 0;
  tex->type = TEXTURE_DEFAULT;
  tex->translate = Vector_New(0,0,0);
  tex->scale = Vector_New(1,1,1);
  tex->local = Matrix_New();
  tex->invLocal = Matrix_New();
  tex->rfl = 0;
  tex->rfr = 0;
  tex->ior = IOR_VACUUM;
  tex->kA = 0.1;
  tex->kS = 0;
  tex->expS = 0;
  tex->isMetallic = 0;
  tex->minRadius = 0;
  tex->maxRadius = 0;
  return tex;
}

Texture *Texture_FromRGB(RGB color) {
  Texture *tex = Texture_New();
  Texture_AddColor(0, Vector_New(color.red,color.green,color.blue), tex);
  return tex;
}

void Texture_Setup(Texture *tex) {

  if(tex->type == TEXTURE_CHECKER) {
    // texture checker use modulo operand so to avoid any
    // point with a negative coordinate we need a large translation
    tex->translate = Vector_New(-1000,-1000,-1000);
    if(tex->length < 2) {
      Texture_AddColor(0, Vector_New(1,1,1), tex);
      Texture_AddColor(0, Vector_New(0,0,0), tex);
    }

  } else if(tex->minRadius > 0 || tex->maxRadius > 0) {
    tex->type = TEXTURE_SPHERICAL;
  }

  Matrix scale = Matrix_New();
  scale._00 = 1/tex->scale.x;
  scale._11 = 1/tex->scale.y;
  scale._22 = 1/tex->scale.z;

  Matrix translate = Matrix_New();
  translate._30 = -tex->translate.x;
  translate._31 = -tex->translate.y;
  translate._32 = -tex->translate.z;

  tex->local = Matrix_Mul(scale,translate);
}

void Texture_AddColor(double t, Vector color, Texture *tex) {
  long i = tex->length;
  if(i == TEXTURE_MAX_LENGTH) return;
  tex->limit[i] = t;
  tex->color[i] = color;
  tex->length++;
}

Vector Texture_GetColorAt(double t, Texture *tex) {
  if(tex->length == 1) return tex->color[0];
  t = fmod(t,1.01);
  long index;
  for(index = 1; index < tex->length; index++) {
    if(tex->limit[index] > t) break;
  }
  double prev = tex->limit[index-1];
  double next = tex->limit[index];
  double factor = (t - prev) / (next - prev);
  Vector max = tex->color[index];
  Vector min = tex->color[index-1];
  Vector color = Vector_MulScalar(Vector_SubVector(max,min),factor);
  color = Vector_AddVector(min, color);
  return color;
}

Vector Texture_GetColor(
  Vector p,
  Vector normal,
  Texture *tex
) {

  Vector color;
  if(tex->type == TEXTURE_SKY) {
    double t = Texture_Sky(p,tex);
    color = Vector_MulScalar(Texture_GetColorAt(t,tex),t);

  } else if(tex->type == TEXTURE_CHECKER) {
    color = Texture_Checker(p,tex);

  } else if(tex->type == TEXTURE_SPHERICAL) {
    double t = Texture_Spherical(p,tex);
    color = Vector_MulScalar(Texture_GetColorAt(t,tex),t);

  // not special function to texturing
  } else {
    color = tex->color[0];
  }

  return color;
}

RGB Texture_GetColorRGB(
  Vector p,
  Vector normal,
  Texture *tex
) {
  return Vector_ToRGB(Texture_GetColor(p,normal,tex));
}

void Texture_SetAmbient(double ambient, Texture *tex) {
  tex->kA = ambient;
}

void Texture_SetScale(Vector v, Texture *tex) {
  tex->scale = v;
}

void Texture_SetPhong(
  double phong,
  long phongExp,
  int isMetallic,
  Texture *tex
) {
  tex->kS = phong;
  tex->expS = phongExp;
  tex->isMetallic = isMetallic;
}

void Texture_SetReflect(double rfl, Texture *tex) {
  tex->rfl = rfl;
}

void Texture_SetRefraction(double rfr, Texture *tex) {
  tex->rfr = rfr;
}

void Texture_SetIOR(double ior, Texture *tex) {
  tex->ior = ior;
}

void Texture_SetRadii(double minRadius, double maxRadius, Texture *tex) {
  tex->minRadius = minRadius;
  tex->maxRadius = maxRadius;
}

void Texture_Free(Texture *tex) {
  free(tex);
}

void Texture_Print(Texture *tex) {
  printf("=== TEXTURE ===\n");
  printf("there are %ld colors\n",tex->length);
  for(long i = 0; i < tex->length; i++) {
    Vector c = tex->color[i];
    printf("%ld) %f %f %f\n",i,c.x,c.y,c.z);
  }
  printf("rfl is %f rfr is %f index is %f\n", tex->rfl, tex->rfr, tex->ior);
  printf("kA is %f\n", tex->kA);
  printf("kS is %f and expS is %ld\n", tex->kS, tex->expS);
  printf("scale is: ");
  Vector_Print(tex->scale);
  printf("minRadius %f and maxRadius %f (if any)\n",
    tex->minRadius, tex->maxRadius
  );
  printf("=== END ===\n");
}

// ********************************

double Texture_Sky(Vector p, Texture *tex) {
  return 1 - (fabs(p.y) / tex->scale.y);
}

double Texture_Spherical(Vector p, Texture *tex) {
  p = Vector_MulMatrix(p, tex->local);
  double minRadius = tex->minRadius;
  double maxRadius = tex->maxRadius;
  double radius = minRadius + maxRadius;
  double distance = fmod(Vector_Length(p),radius);

  if(distance < minRadius * .25)
    return .5 + 2 * distance / minRadius;

  if(distance < minRadius * .75)
    return 1;

  if(distance < minRadius)
    return 2.5 - 2 * distance / minRadius;

  distance -= minRadius;

  if(distance < maxRadius * .25)
    return .5 - 2 * distance / maxRadius;

  if(distance < maxRadius * .75)
    return 0;

  return 2 * distance / maxRadius - 1.5;
}

Vector Texture_Checker(Vector p, Texture *tex) {
  p = Vector_MulMatrix(p, tex->local);
  return ((int)p.x + (int)p.y + (int)p.z) % 2 == 0 ?
    tex->color[0] : tex->color[1];
}
