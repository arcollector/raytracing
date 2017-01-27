#include "texture.h"

Texture *Texture_New() {
  Texture *tex = malloc(sizeof(Texture));
  tex->length = 0;
  tex->type = TEXTURE_DEFAULT;
  // used in spherical texturing
  tex->minRadius = 0;
  tex->maxRadius = 0;
  return tex;
}

Texture *Texture_FromRGB(RGB color) {
  Texture *tex = Texture_New();
  Texture_AddColor(0, Vector_New(color.red,color.green,color.blue), tex);
  return tex;
}

void Texture_AddColor(double t, Vector color, Texture *tex) {
  long i = tex->length;
  tex->limit[i] = t;
  tex->color[i].x = color.x;
  tex->color[i].y = color.y;
  tex->color[i].z = color.z;
  tex->length++;
}

Vector Texture_GetColorAt(double t, Texture *tex) {
  int index;
  for(index = 1; index < tex->length; index += 1) {
    if(tex->limit[index] > t) break;
  }
  double prev = tex->limit[index-1];
  double next = tex->limit[index];
  double factor = (t - prev) / (next - prev);
  Vector color = Vector_MulScalar(
    Vector_SubVector(
      tex->color[index],
      tex->color[index-1]
    ),
    factor
  );
  color = Vector_AddVector(tex->color[index-1], color);

  return color;
}

Vector Texture_GetColor(Vector p, Texture *tex) {
  Vector color;

  if(tex->type == TEXTURE_SKY) {
    double t = Texture_Sky(p,tex);
    color = Texture_GetColorAt(t,tex);

  // use spherical texturing
  } else if(tex->minRadius > 0 && tex->maxRadius > 0) {
    double t = Texture_Spherical(p,tex);
    color = Vector_MulScalar(Texture_GetColorAt(t,tex),t);

  // not special function to texturing
  } else {
    color = tex->color[0];
  }

  return color;
}

RGB Texture_GetColorRGB(Vector p, Texture *tex) {
  return Vector_ToRGB(Texture_GetColor(p,tex));
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
  printf("minRadius %f and maxRadius %f (if any)\n",
    tex->minRadius, tex->maxRadius
  );
  printf("=== END ===\n");
}

// ********************************

double Texture_Sky(Vector p, Texture *tex) {
  return Vector_Dot(Vector_Normalize(Vector_New(p.x,0,1)),p);
}

double Texture_Spherical(Vector p, Texture *tex) {
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

