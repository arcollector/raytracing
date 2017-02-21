#include "shoot.h"

static Ray Shoot_BuildRay(double x, double y, Camera *cam);

static Vector Shoot_Single(
  double x, double y,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength,
  Scene *scene
);

/* Used for multisampling
 * this struct represents a pixel where
 * each coordinate (in the struct) refers
 * to a offset of the pixel where is stored
 * the color of the pixel multisampled
 * (0,0), (1,0), (2,0), (3,0), (4,0)
 * (0,1), (1,1), (2,1), (3,1), (4,1)
 * (0,2), (1,2), (2,2), (3,2), (4,2)
 * (0,3), (1,3), (2,3), (3,3), (4,3)
 * (0,4), (1,4), (2,4), (3,4), (4,4)
 */
typedef struct {
  Vector pixel[5][5];
  int flag[5][5];
} Window;

static Vector Shoot_Multi(
  double x, double y, int i, int j,
  Window *window, int upto,
  int isStochastic,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength,
  Scene *scene
);

// maximum diff between avg color and corner colors
// used in multi/stochastic sampling
#define AVG_DIFF 5./255.

// stochastic jittering used to generate a poisson disk
// distribution of the ray samples
#define JITTER() (rand()/(double)RAND_MAX*-.125)

Ray Shoot_BuildRay(double x, double y, Camera *cam) {

  // from screen space
  Vector ps = Vector2d_New(x,y);
  // to view plane space (windowing)
  // pvp is in cam space
  Vector pvp = Vector_MulMatrix(ps,cam->win);
  //Vector_Print(ps); Vector_Print(pvp);
  // to world space
  Vector pw = Vector_MulMatrix(pvp,cam->invLocal);
  //Vector_Print(pw);
  // build ray (world space)
  Ray ray = Ray_Normalize(Ray_FromP1toP2(cam->pos,pw));
  //Ray_Print(ray);

  return ray;
}

RGB Shoot(
  long x, long y,
  Scene *scene,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength
) {

  Vector pixel;
  // if stochastic or multisamplig
  if(scene->aa == AA_STOCHASTIC || scene->aa == AA_MULTI) {
    Window window = { .flag = {0} };
    pixel = Shoot_Multi(
      x,y,0,0,&window,4,
      scene->aa == AA_STOCHASTIC,
      root,treeObjectLength,
      unboundedObjectList,unboundedObjectListLength,
      scene
    );

  // else if single ray shooting
  } else {
    pixel = Shoot_Single(
      x,y,
      root,treeObjectLength,
      unboundedObjectList,unboundedObjectListLength,
      scene
    );
  }

  return Vector_ToRGB(pixel);
}

Vector Shoot_Single(
  double x, double y,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength,
  Scene *scene
) {
  // build ray
  Ray ray = Shoot_BuildRay(x+.5,y+.5,scene->cam);
  Vector pixel = Shade(
    ray,
    0,
    root,treeObjectLength,
    unboundedObjectList,unboundedObjectListLength,
    scene
  );
  return pixel;
}

Vector Shoot_Multi(
  double x, double y, int i, int j,
  Window *window, int upto,
  int isStochastic,
  BBOXTree *root, long treeObjectLength,
  Object *unboundedObjectList, long unboundedObjectListLength,
  Scene *scene
) {

  Ray ray;

  double r1,r2,r3,r4,r5,r6,r7,r8;
  if(isStochastic) {
    r1 = JITTER();
    r2 = JITTER();
    r3 = JITTER();
    r4 = JITTER();
    r5 = JITTER();
    r6 = JITTER();
    r7 = JITTER();
    r8 = JITTER();
  } else {
    r1 = r2 = r3 = r4 = r5 = r6 = r7 = r8 = 0;
  }

  // compute topleft, topright, bottomleft and bottomright colors
  Vector c1, c2, c3, c4, avg;

  #define AVG_4_COLORS(c1,c2,c3,c4) \
    Vector_New( \
        ((c1.x + c2.x + c3.x + c4.x)/4.), \
        ((c1.y + c2.y + c3.y + c4.y)/4.), \
        ((c1.z + c2.z + c3.z + c4.z)/4.) \
      )

  #define COMPUTE_CORNER_COLOR(ii,jj,rx,ry) \
    if(!window->flag[jj][ii]) { \
      ray = Shoot_BuildRay( \
        x + (ii)/4. + rx, \
        y + (jj)/4. + ry, \
        scene->cam \
      ); \
      window->pixel[jj][ii] = Shade( \
        ray, \
        0, \
        root,treeObjectLength, \
        unboundedObjectList,unboundedObjectListLength, \
        scene \
      ); \
      window->flag[jj][ii] = 1; \
    }

  // x + i, y + j
  COMPUTE_CORNER_COLOR(i,j,r1,r2);
  c1 = window->pixel[j][i];

  // x + i+offset, y + j
  COMPUTE_CORNER_COLOR(i+upto,j,r3,r4);
  c2 = window->pixel[j][i+upto];

  // x + i, y + j+offset
  COMPUTE_CORNER_COLOR(i,j+upto,r5,r6);
  c3 = window->pixel[j+upto][i];

  // x + i+offset, y + j+offset
  COMPUTE_CORNER_COLOR(i+upto,j+upto,r7,r8);
  c4 = window->pixel[j+upto][i+upto];

  //RGB_Print(c1); RGB_Print(c2); RGB_Print(c3); RGB_Print(c4); printf("\n");

  avg = AVG_4_COLORS(c1,c2,c3,c4);
  // how much differ avg and corners colors
  if(upto == 1 ||
      fabs(avg.x - c1.x) <= AVG_DIFF &&
      fabs(avg.y - c1.y) <= AVG_DIFF &&
      fabs(avg.z - c1.z) <= AVG_DIFF &&
      fabs(avg.x - c2.x) <= AVG_DIFF &&
      fabs(avg.y - c2.y) <= AVG_DIFF &&
      fabs(avg.z - c2.z) <= AVG_DIFF &&
      fabs(avg.x - c3.x) <= AVG_DIFF &&
      fabs(avg.y - c3.y) <= AVG_DIFF &&
      fabs(avg.z - c3.z) <= AVG_DIFF &&
      fabs(avg.x - c4.x) <= AVG_DIFF &&
      fabs(avg.y - c4.y) <= AVG_DIFF &&
      fabs(avg.z - c4.z) <= AVG_DIFF) {
    // enough
    return avg;
  }

  #define SHOOT_AGAIN(ii,jj) \
    Shoot_Multi( \
      x,y, ii,jj, \
      window, upto, isStochastic, \
      root, treeObjectLength, \
      unboundedObjectList, unboundedObjectListLength, \
      scene \
     )

  upto /= 2;
  // keep sampling pixel
  c1 = SHOOT_AGAIN(i,j);
  c2 = SHOOT_AGAIN(i+upto,j);
  c3 = SHOOT_AGAIN(i,j+upto);
  c4 = SHOOT_AGAIN(i+upto,j+upto);

  return AVG_4_COLORS(c1,c2,c3,c4);
}
