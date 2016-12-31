#include "rgb.h"

RGB RGB_New(unsigned int red, unsigned int green, unsigned int blue) {
  RGB color;
  color.red = red;
  color.green = green;
  color.blue = blue;
  return color;
}

RGBl RGBl_New(long red, long green, long blue) {
  RGBl color;
  color.red = red;
  color.green = green;
  color.blue = blue;
  return color;
}

RGBf RGBf_New(double red, double green, double blue) {
  RGBf color;
  color.red = red;
  color.green = green;
  color.blue = blue;
  return color;
}

RGBf RGB_toRGBf(RGB rgb) {
  return RGBf_New(
    rgb.red/255.,
    rgb.green/255.,
    rgb.blue/255.
  );
}

RGB RGBf_toRGB(RGBf rgb) {
  return RGB_New(
    rgb.red*255.,
    rgb.green*255.,
    rgb.blue*255.
  );
}

int RGB_Equals(RGB c1, RGB c2, int threshold) {
  RGB cmp = RGB_New(
    abs(c1.red - c2.red),
    abs(c1.green - c2.green),
    abs(c1.blue - c2.blue)
  );
  return cmp.red <= threshold &&
        cmp.blue <= threshold &&
        cmp.green <= threshold;
}

void RGB_Print(RGB c) {
  printf("color is: %d %d %d\n",c.red,c.green,c.blue);
}

void RGBl_Print(RGBl c) {
  printf("color is: %ld %ld %ld\n",c.red,c.green,c.blue);
}

void RGBf_Print(RGBf c) {
  printf("color is: %5.5f %5.5f %5.5f\n",c.red,c.green,c.blue);
}
