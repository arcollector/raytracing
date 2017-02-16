#include "shade.h"

#define MAX_LEVEL 10

static Vector Shade_ComputeColor(
  Hit *hit,
  Ray ray,
  int level,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
);

static Ray Shade_RayReflect(
  Vector dir,
  Vector intersection,
  Vector normal
);

static int Shade_RayRefract(
  Vector intersection,
  Vector incident,
  Vector normal,
  double nI,
  double nR,
  Ray *rayRefract,
  Object *obj
);

Vector Shade(
  Ray ray,
  int level,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
) {

  double tValue;
  Hit *hit = Intersect(
    ray,
    &tValue,
    root,treeObjectLength,
    unboundedObjectList,unboundedObjectListLength
  );

  Vector color = Vector_New(0,0,0);

  if(hit) {
    color = Shade_ComputeColor(
      hit,
      ray,
      level,
      root,treeObjectLength,
      unboundedObjectList,unboundedObjectListLength,
      scene
    );
    Hit_Free(hit);

  } else {
    // not calculate sky at recursive calls
    if(level == 0) {
      // there not intersection point in this case
      // use the view plane point instead
      // to do this, transform ray in world space to cam space
      ray = Ray_Transform(ray,scene->cam->local);
      color = Texture_GetColor(
        // then calculate intersection point on the view plane
        Ray_PointAt(
          ray,
          -Vector_Dot(ray.start,ray.start) / Vector_Dot(ray.start,ray.dir)
        ),
        VECTOR_ZERO,
        scene->sky
      );
    }
  }

  return color;
}

Vector Shade_ComputeColor(
  Hit *hit,
  Ray ray,
  int level,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
) {

  if(level == MAX_LEVEL) return Vector_New(0,0,0);
  // for phong calc
  Vector white = Vector_New(1,1,1);

  Hit_Begin(hit);
  Vector intersection = Ray_PointAt(ray, Hit_Next(hit));
  Vector eye = Vector_Normalize(
    Vector_FromP1toP2(intersection, scene->cam->viewerPos)
  );
  Object *obj = hit->object;
  void *primitive = obj->primitive;
  Vector normal = (*obj->normal)(intersection,primitive);
  // move the intersection off from the primitive by a little amount
  // to avoid self intersection on the recursive calls
  Vector pi = Vector_AddVector(intersection,Vector_MulScalar(normal,EPSILON));
  Texture *tex = obj->texture;
  Vector baseColor = Texture_GetColor(pi,normal,tex);
  // color * Ka
  Vector color = Vector_MulScalar(baseColor,tex->kA);
  for(Lamp *lamp = scene->lampList; lamp; lamp = lamp->next) {
    Vector sumTerm = Vector_New(0,0,0);
    Ray pl = Ray_FromP1toP2(pi,lamp->position);
    double dist = Ray_Length(pl);
    // normalize ray dir
    pl.dir = Vector_DivScalar(pl.dir,dist);
    // Lambertian calc
    double cosI = Vector_Dot(normal,pl.dir);
    // object normal is not facing the light
    if(cosI < 0) continue;
    double attenuate = 1;
    if(Intersect_Shadow(
        pl,
        dist,
        &attenuate,
        root,treeObjectLength,
        unboundedObjectList,unboundedObjectListLength
      )) {
      continue;
    }
    // color * Ka + sum[ color * cosI ]
    sumTerm = Vector_AddVector(sumTerm,Vector_MulScalar(baseColor,cosI));
    sumTerm = Vector_MulScalar(sumTerm,attenuate);
    // Phong calc
    if(tex->kS > 0) {
      Vector reflect = Vector_Reflect(pl.dir, normal);
      double cosR = Vector_Dot(eye, reflect);
      if(cosR > 0) {
        // color * Ka + sum[ color * cosI + white * Ks * cosR^expS ]
        double factor = tex->kS * pow(cosR, tex->expS);
        Vector tmp = Vector_MulScalar(tex->isMetallic ? baseColor : white, factor);
        sumTerm = Vector_AddVector(sumTerm,tmp);
      }
    }
    color = Vector_AddVector(color,sumTerm);
  }

  if(tex->rfl > 0) {
    Vector rflColor = Shade(
      Shade_RayReflect(ray.dir,intersection,normal),
      level + 1,
      root, treeObjectLength,
      unboundedObjectList, unboundedObjectListLength,
      scene
    );
    color = Vector_AddVector(color,Vector_MulScalar(rflColor, tex->rfl));
  }

  if(tex->rfr > 0) {
    Ray rayRefract;
    if(Shade_RayRefract(
      intersection,
      ray.dir,
      normal,
      scene->sky->ior,
      tex->ior,
      &rayRefract,
      obj
    )) {
      Vector rfrColor = Shade(
        rayRefract,
        level + 1,
        root, treeObjectLength,
        unboundedObjectList, unboundedObjectListLength,
        scene
      );
      color = Vector_AddVector(color,Vector_MulScalar(rfrColor, tex->rfr));
    }
  }

  return color;
}

Ray Shade_RayReflect(
  Vector dir,
  Vector intersection,
  Vector normal
) {
  return Ray_New(
    // move intersection to avoid self intersection
    Vector_AddVector(intersection,Vector_MulScalar(normal,EPSILON)),
    // calc reflect vector and use it as the ray dir
    Vector_Reflect(dir,normal)
  );
}

int Shade_RayRefract(
  Vector intersection,
  Vector incident,
  Vector normal,
  double nI,
  double nR,
  Ray *rayRefract,
  Object *obj
) {
  // incident must pointing to nR medium
  // normal must pointing to nI medium
  int isTIR;
  Vector refract = Vector_Refract(
    incident,
    normal,
    nI,
    nR,
    &isTIR
  );
  void *primitive = obj->primitive;
  // using the refract vector, build a ray and intersect it
  // with current primitive, find out where this refract ray
  // will intersect
  Ray ray = Ray_New(intersection,refract);
  Hit *hit = (*obj->intersect)(ray,primitive);
  if(!hit) {
    printf("refract calculation: hit is null!");
    exit(0);
  }
  // we need the exit point (second t)
  Hit_Begin(hit);
  // ignore first t
  Hit_Next(hit);
  // some primitives returns only one t, so
  // in these cases reset hit list
  if(Hit_IsEnd(hit)) Hit_Begin(hit);
  intersection = Ray_PointAt(ray,Hit_Next(hit));
  Hit_Free(hit);
  // use the normal at the exit point
  // we need normal pointing to medium 1
  normal = Vector_Negate((*obj->normal)(intersection,primitive));
  // calc exit refract vector
  // it will an exit refract vector if a tir dont ocurr
  incident = refract;
  refract = Vector_Refract(
    incident,
    normal,
    nR,
    nI,
    &isTIR
  );
  // we are trapped inside primitive
  // TODO handle tir situations
  if(isTIR) return 0;

  *rayRefract = Ray_New(
    // move intersection point by a little to avoid self intersection
    // remember that normal was negated
    Vector_AddVector(intersection,Vector_MulScalar(normal,-EPSILON)),
    refract
  );

  return 1;
}
