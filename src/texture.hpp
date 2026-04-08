#ifndef TEXTURE_H
#define TEXTURE_H

#include "vec3.hpp"
#include "color.hpp"

class texture {
  public:
    virtual ~texture() = default;

    virtual color sample(double u, double v, const point3& p) const = 0;
};

#endif
