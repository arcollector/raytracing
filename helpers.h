#ifndef HELPERS_H
#define HELPERS_H

#include <stdlib.h>

#define PI 3.141592653589793
#define EPSILON 10e-5
#define POSITIVE_INFINITY 1e10
#define NEGATIVE_INFINITY -POSITIVE_INFINITY

#define SIGN(x) (fabs(x) <= EPSILON ? 0 : (x) > 0 ? 1 : -1)

#define DEG2RAD(x) ((x)/180.*PI)

#define ROUND(x) (long)((x)+.5)
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define RANDOM(min,max) (rand()/(double)RAND_MAX*((max)-(min))+(min))

#endif
