#include <stdio.h>
#define TT_IMPLEMENTATION
#include "../3rdparty/tinytime.h"

int main(int argc, char *args[]) {

  ttTime();
  for(long y = 0; y < 2000; y++) {
    for(long x = 0; x < 2000; x++) {
      for(long i = 0; i < 256; i++) { // max quad tree depth
        for(long j = 0; j < 31*3+1; j++) { // total objects

        }
      }
    }
  }
  printf("elapsed time: %f seconds\n",ttTime());

  return 0;
}
