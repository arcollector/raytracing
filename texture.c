#include "texture.h"

Texture *Texture_New() {
  Texture *tex = malloc(sizeof(Texture));
  tex->length = 0;
  // used in spherical texturing
  tex->minRadius = 0;
  tex->maxRadius = 0;
  return tex;
}

Vector Texture_GetColorWithinLimit(double t, Texture *tex) {
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
  // use spherical texturing
  if(tex->minRadius > 0 && tex->maxRadius) {
    double t = Texture_Spherical(p,tex);
    if(tex->length == 1) {
      color = tex->color[0];
    } else {
      color = Texture_GetColorWithinLimit(t,tex);
    }
    color = Vector_MulScalar(color,t);

  // not special function to texturing
  } else {
    if(tex->length == 1) {
      color = tex->color[0];
    } else {
      color = tex->color[(int)RANDOM(0,tex->length)%tex->length];
    }
  }

  return color;
}

RGB Texture_GetColorRGB(Vector p, Texture *tex) {
  Vector color = Texture_GetColor(p,tex);
  return RGB_New(
    MAX(0,MIN(255,ROUND(color.x*255))),
    MAX(0,MIN(255,ROUND(color.y*255))),
    MAX(0,MIN(255,ROUND(color.z*255)))
  );
}

double Texture_Spherical(Vector p, Texture *tex) {

  int debug = 0;
  if(debug) Vector_Print(p);

  double minRadius = tex->minRadius;
  double maxRadius = tex->maxRadius;

  double distance = Vector_Length(p);

  double radius = minRadius + maxRadius;
  distance = fmod(distance, radius);
  if(debug) printf("distance (fmod?) is: %f\n",distance);

  if(distance < minRadius * .25) {
    if(debug) printf(".5 to 1\n");
    return .5 + (1 - .5)*(distance/(minRadius*.25));
  }

  if(distance < minRadius * .75) {
    if(debug) printf("1\n");
    return 1;
  }

  if(distance < minRadius) {
    if(debug) printf("1 to .5\n");
    return .5 + (1 - .5)*(-(distance - minRadius*.75)/(minRadius - minRadius*.75)+1);
  }

  distance -= minRadius;

  if(distance < maxRadius * .25) {
    if(debug) printf(".5 to 0\n");
    return 0 + (.5 - 0)*(-(distance)/(maxRadius*.25)+1);
  }

  if(distance < maxRadius * .75) {
    if(debug) printf("0\n");
    return 0;
  }

  if(debug) printf("0 to .5\n");
  return 0 + (.5 - 0)*((distance - maxRadius*.75)/(maxRadius - maxRadius *.75));
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
