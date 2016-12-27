#include "rgb.h"

RGB RGB_New(unsigned int red, unsigned int green, unsigned int blue) {
  RGB color;
  color.red = red;
  color.green = green;
  color.blue = blue;
  return color;
}

