#ifndef TRI_H
#define TRI_H

#include "plane.hpp"

class tri : public plane {
  public:
    tri(const point3& a, const vec3& b, const vec3& c, shared_ptr<material> mat)
      : plane(a, b - a, c - a, mat)
    {
      // TODO: uv coordinates
      // TODO: front face based on clockwizeness of points
      bbox = aabb(a, b) + c;
    }

    aabb bounding_box() const override { return bbox; }

    virtual bool is_interior(double a, double b, hit_record& rec) const override {
      if (a < 0.0 || b < 0.0 || a + b > 1.0)
        return false;

      // TODO: uv coordinates
      rec.u = a;
      rec.v = b;
      return true;
    }

  private:
    aabb bbox;
};

#endif
