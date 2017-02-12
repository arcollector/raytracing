#include "shade.h"

static Vector Shade_ComputeColor(
  Object *obj,
  Vector intersection,
  Vector eye,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Lamp *lampList,
  long lampListLength
);

RGB Shade(
  Ray ray,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
) {

  double tValue;
  Object *intersected = Intersect(
    ray,
    &tValue,
    root,treeObjectLength,
    unboundedObjectList,unboundedObjectListLength
  );

  Vector color;
  if(intersected) {
    Vector intersectionPoint = Ray_PointAt(ray,tValue);
    Vector eye = Vector_Normalize(
        Vector_FromP1toP2(intersectionPoint,scene->cam->viewerPos)
    );
    color = Shade_ComputeColor(
      intersected,
      intersectionPoint,
      eye,
      root,treeObjectLength,
      unboundedObjectList,unboundedObjectListLength,
      scene->lampList,scene->lampListLength
    );
  } else {
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
      VECTOR_NULL,
      scene->sky
    );
  }

  return Vector_ToRGB(color);
}

Vector Shade_ComputeColor(
  Object *obj,
  Vector intersection,
  Vector eye,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Lamp *lampList,
  long lampListLength
) {

  Vector reflect;
  void *primitive = obj->primitive;
  Vector normal = (*obj->normal)(intersection,primitive);
  // move the intersection off from the primitive by a litte ammount
  // to avoid self intersection on the recursive calls
  intersection = Vector_AddVector(intersection,Vector_MulScalar(normal,.01));
  Texture *tex = obj->texture;
  Vector baseColor = Texture_GetColor(intersection,normal,tex);
  Vector white = Vector_New(1,1,1);
  // color * Ka
  if(gbDebug) printf("baseColor "), Vector_Print(baseColor);
  Vector color = Vector_MulScalar(baseColor,tex->kA);
  if(gbDebug) printf("baseColor * kA "), Vector_Print(color);
  for(Lamp *lamp = lampList; lamp; lamp = lamp->next) {
    if(gbDebug) printf("light "), Vector_Print(lamp->position);
    Vector sumTerm = Vector_New(0,0,0);
    // ray from intersection point to lamp location
    Ray pl = Ray_New(intersection,lamp->position);
    if(gbDebug) printf("ray point -> light "), Ray_Print(pl);
    double dist = Ray_Length(pl);
    // normalize ray dir
    pl.dir = Vector_DivScalar(pl.dir,dist);
    /// Lambertian calc
    double cosI = Vector_Dot(normal,pl.dir);
    // object normal is not facing the light
    if(cosI < 0) continue;
    // look for shadow, check for intersection with other objects
    if(gbDebug) printf("checking shadow %f\n",dist);
    if(Intersect_Shadow(
        pl,
        dist,
        root,treeObjectLength,
        unboundedObjectList,unboundedObjectListLength
      )) {
      if(gbDebug) printf("shadowed!\n");
      continue;
    }
    // color * Ka + sum[ color * cosI ]
    sumTerm = Vector_AddVector(sumTerm,Vector_MulScalar(baseColor,cosI));
    if(gbDebug) printf("cosI = %f baseColor*cosI ",cosI), Vector_Print(sumTerm);
    // Phong calc
    if(tex->kS > 0) {
      Vector tmp = Vector_MulScalar(normal, 2*cosI);
      reflect = Vector_SubVector(tmp, pl.dir);
      double cosR = Vector_Dot(eye, reflect);
      if(cosR > 0) {
        // color * Ka + sum[ color * cosI + white * Ks * cosR^expS ]
        double factor = tex->kS*pow(cosR,tex->expS);
        tmp = Vector_MulScalar(tex->isMetallic ? baseColor : white, factor);
        if(gbDebug) printf("cosR = %f white*cosR^expS*kS = %f ",cosR,factor), Vector_Print(tmp);
        sumTerm = Vector_AddVector(sumTerm,tmp);
      }
    }
    if(gbDebug) printf("sum term "), Vector_Print(sumTerm);
    color = Vector_AddVector(color,sumTerm);
  }

  // TODO trace the reflect vector

  // TODO trace the refract vector
  if(gbDebug) printf("final "), Vector_Print(color);
  return color;
}
