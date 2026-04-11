#ifndef TEXTURE_H
#define TEXTURE_H

#include "background.hpp"
#include "ray.hpp"
#include "vec3.hpp"
#include "color.hpp"

class texture : public background {
  public:
    virtual ~texture() = default;

    virtual color sample(double u, double v, const point3& p) const = 0;

    color sample_sphere(const ray& r) const override {
      auto unit_direction = unit_vector(r.direction());
      double u, v;
      get_sphere_uv(unit_direction, u, v);
      return sample(u, v, unit_direction);
    }
};

#endif
