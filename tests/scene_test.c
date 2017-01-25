#include <stdio.h>

#include "../scene.h"

int main(int argc, char *argv[]) {

  FILE *fp = fopen("scene.txt","rb");
  if(!fp) {
    return 0;
  }

  Scene *scene = Scene_New(fp);
  if(!scene) {
    fclose(fp);
    return 0;
  }

  if(!Scene_Setup(fp, scene)) {
    Scene_Free(scene);
    fclose(fp);
    return 0;
  }
  fclose(fp);

  Scene_Print(scene);

  Scene_Free(scene);

  return 0;
}
