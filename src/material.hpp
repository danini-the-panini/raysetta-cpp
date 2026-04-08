#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.hpp"
#include "color.hpp"

class material {
  public:
    virtual ~material() = default;

    virtual color emitted(double, double, const point3&) const {
      return color(0,0,0);
    }

    virtual bool scatter(const ray&, const hit_record&, color&, ray&) const {
      return false;
    }
};

#endif
