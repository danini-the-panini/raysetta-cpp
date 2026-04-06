#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.hpp"
#include "color.hpp"

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(const ray&, const hit_record&, color&, ray&) const { return false; }
};

#endif
