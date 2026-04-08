#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "color.hpp"
#include "ray.hpp"

class background {
  public:
    virtual ~background() = default;

    virtual color sample_sphere(const ray&) const = 0;
};

#endif
