#ifndef COLOR_H
#define COLOR_H

#include "vec3.hpp"

#include <iostream>

using color = vec3;

union pixel {
  unsigned char value[3];
  struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
  };
};

pixel to_pixel(const color& c);

void write_pixel(std::ostream& out, const pixel& px);

#endif
