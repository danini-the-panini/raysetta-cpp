#ifndef GRADIENT_H
#define GRADIENT_H

#include "background.hpp"
#include "color.hpp"

class gradient : public background {
  public:
    gradient(const color& top, const color& bottom)
      : top(top), bottom(bottom) {}

    color sample_sphere(const ray& r) const override {
      vec3 unit_direction = unit_vector(r.direction());
      auto a = 0.5*(unit_direction.y() + 1.0);
      return (1.0-a)*bottom + a*top;
    }

  private:
    color top;
    color bottom;
};

#endif
