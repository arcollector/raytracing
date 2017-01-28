#include "texture.h"

Texture *Texture_New() {
  Texture *tex = malloc(sizeof(Texture));
  tex->length = 0;
  tex->type = TEXTURE_DEFAULT;
  tex->scale = Vector_New(1,1,1);
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
  Ray ray,
  Vector p,
  Vector normal,
  Texture *tex
) {
  Vector color;

  if(tex->type == TEXTURE_SKY) {
    double t = Texture_Sky(ray,tex);
    color = Vector_MulScalar(Texture_GetColorAt(t,tex),t);

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

RGB Texture_GetColorRGB(
  Ray ray,
  Vector p,
  Vector normal,
  Texture *tex   
) {
  return Vector_ToRGB(Texture_GetColor(ray,p,normal,tex));
}

void Texture_SetScale(Vector v, Texture *tex) {
  tex->scale.x = v.x > 0 ? v.x : 1; // avoid dividing by 0
  tex->scale.y = v.y > 0 ? v.y : 1;
  tex->scale.z = v.z > 0 ? v.z : 1;
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
  printf("scale is: ");
  Vector_Print(tex->scale);
  printf("minRadius %f and maxRadius %f (if any)\n",
    tex->minRadius, tex->maxRadius
  );
  printf("=== END ===\n");
}

// ********************************

double Texture_Sky(Ray ray, Texture *tex) {
  Vector pvp = Ray_PointAt(
    ray,
    -Vector_Dot(ray.start,ray.start) / Vector_Dot(ray.start,ray.dir)
  );
  return 1 - (fabs(pvp.y) / tex->scale.y);
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

