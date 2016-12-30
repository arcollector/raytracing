#ifndef RGB_H
#define RGB_H

struct RGBStruct {
  unsigned int red, green, blue;
};

typedef struct RGBStruct RGB;

RGB RGB_New(unsigned int red, unsigned int green, unsigned int blue);

#endif
