#ifndef QUAD_H
#define QUAD_H

#include "plane.hpp"
#include "hittable_list.hpp"

class quad : public plane {
  public:
    quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<material> mat)
      : plane(Q, u, v, mat)
    {
      set_bounding_box();
    }

    virtual void set_bounding_box() {
      // Compute the bounding box of all four vertices.
      auto bbox_diagonal1 = aabb(Q, Q + u + v);
      auto bbox_diagonal2 = aabb(Q + u, Q + v);
      bbox = aabb(bbox_diagonal1, bbox_diagonal2);
    }

    aabb bounding_box() const override { return bbox; }

    virtual bool is_interior(double a, double b, hit_record& rec) const override {
      interval unit_interval = interval(0, 1);
      // Given the hit point in plane coordinates, return false if it is outside the
      // primitive, otherwise set the hit record UV coordinates and return true.

      if (!unit_interval.contains(a) || !unit_interval.contains(b))
        return false;

      rec.u = a;
      rec.v = b;
      return true;
    }

  private:
    aabb bbox;
};

inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat)
{
  // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

  auto sides = make_shared<hittable_list>();

  // Construct the two opposite vertices with the minimum and maximum coordinates.
  auto min = point3(std::fmin(a.x(),b.x()), std::fmin(a.y(),b.y()), std::fmin(a.z(),b.z()));
  auto max = point3(std::fmax(a.x(),b.x()), std::fmax(a.y(),b.y()), std::fmax(a.z(),b.z()));

  auto dx = vec3(max.x() - min.x(), 0, 0);
  auto dy = vec3(0, max.y() - min.y(), 0);
  auto dz = vec3(0, 0, max.z() - min.z());

  sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
  sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
  sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
  sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

  return sides;
}

#endif
