#include "shade.h"

#define MAX_LEVEL 10

static Vector Shade_ComputeColor(
  Object *obj,
  Vector intersection,
  Vector rayDir,
  Vector eye,
  int level,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
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
  Object *intersected = Intersect(
    ray,
    &tValue,
    root,treeObjectLength,
    unboundedObjectList,unboundedObjectListLength
  );

  Vector color = Vector_New(0,0,0);
  if(intersected) {
    Vector intersectionPoint = Ray_PointAt(ray,tValue);
    Vector eye = Vector_Normalize(
      Vector_FromP1toP2(intersectionPoint,scene->cam->viewerPos)
    );
    color = Shade_ComputeColor(
      intersected,
      intersectionPoint,
      ray.dir,
      eye,
      level,
      root,treeObjectLength,
      unboundedObjectList,unboundedObjectListLength,
      scene
    );
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
        VECTOR_NULL,
        scene->sky
      );
    }
  }

  return color;
}

Vector Shade_ComputeColor(
  Object *obj,
  Vector intersection,
  Vector rayDir,
  Vector eye,
  int level,
  BBOXTree *root,
  long treeObjectLength,
  Object *unboundedObjectList,
  long unboundedObjectListLength,
  Scene *scene
) {

  if(level == MAX_LEVEL) return Vector_New(0,0,0);

  Vector white = Vector_New(1,1,1);
  Vector normal = (*obj->normal)(intersection,obj->primitive);
  // move the intersection off from the primitive by a little amount
  // to avoid self intersection on the recursive calls
  intersection = Vector_AddVector(intersection,Vector_MulScalar(normal,.01));
  Texture *tex = obj->texture;
  Vector baseColor = Texture_GetColor(intersection,normal,tex);
  //if(gbDebug) printf("baseColor "), Vector_Print(baseColor);
  // color * Ka
  Vector color = Vector_MulScalar(baseColor,tex->kA);
  //if(gbDebug) printf("baseColor * kA "), Vector_Print(color);
  for(Lamp *lamp = scene->lampList; lamp; lamp = lamp->next) {
    //if(gbDebug) printf("light "), Vector_Print(lamp->position);
    Vector sumTerm = Vector_New(0,0,0);
    Ray pl = Ray_FromP1toP2(intersection,lamp->position);
    double dist = Ray_Length(pl);
    // normalize ray dir
    pl.dir = Vector_DivScalar(pl.dir,dist);
    // Lambertian calc
    double cosI = Vector_Dot(normal,pl.dir);
    // object normal is not facing the light
    if(cosI < 0) continue;
    if(Intersect_Shadow(
        pl,
        dist,
        root,treeObjectLength,
        unboundedObjectList,unboundedObjectListLength
      )) {
      //if(gbDebug) printf("\tshadowed!\n");
      continue;
    }
    // color * Ka + sum[ color * cosI ]
    sumTerm = Vector_AddVector(sumTerm,Vector_MulScalar(baseColor,cosI));
    //if(gbDebug) printf("\tcosI = %f baseColor*cosI ",cosI), Vector_Print(sumTerm);
    // Phong calc
    if(tex->kS > 0) {
      Vector tmp = Vector_MulScalar(normal, 2*cosI);
      Vector reflect = Vector_SubVector(tmp, pl.dir);
      double cosR = Vector_Dot(eye, reflect);
      if(cosR > 0) {
        // color * Ka + sum[ color * cosI + white * Ks * cosR^expS ]
        double factor = tex->kS * pow(cosR, tex->expS);
        tmp = Vector_MulScalar(tex->isMetallic ? baseColor : white, factor);
        //if(gbDebug) printf("\tcosR = %f white*cosR^expS*kS = %f ",cosR,factor), Vector_Print(tmp);
        sumTerm = Vector_AddVector(sumTerm,tmp);
      }
    }
    //if(gbDebug) printf("\tsum term "), Vector_Print(sumTerm);
    color = Vector_AddVector(color,sumTerm);
  }

  if(tex->rfl > 0) {
    // assume that ray dir is pointing to the surface
    // so negate it to correct its direction
    Vector dir = Vector_Negate(rayDir);
    double cosI = Vector_Dot(normal, dir);
    Vector tmp = Vector_MulScalar(normal, 2*cosI);
    Vector reflect = Vector_SubVector(tmp, dir);
    Vector rflColor = Shade(
      Ray_New(intersection, reflect),
      level + 1,
      root, treeObjectLength,
      unboundedObjectList, unboundedObjectListLength,
      scene
    );
    color = Vector_AddVector(color,Vector_MulScalar(rflColor, tex->rfl));
  }

  // TODO trace the refract vector

  //if(gbDebug) printf("final "), Vector_Print(color);
  return color;
}
