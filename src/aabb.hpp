#ifndef AABB_H
#define AABB_H

#include "ray.hpp"
#include "interval.hpp"

class aabb {
  public:
    interval x, y, z;

     // The default AABB is empty, since intervals are empty by default.
    aabb() {
      pad_to_minimums();
    }

    aabb(const interval& x, const interval& y, const interval& z)
      : x(x), y(y), z(z) {
      pad_to_minimums();
    }

    aabb(const point3& a, const point3& b) {
      // Treat the two points a and b as extrema for the bounding box, so we don't require a
      // particular minimum/maximum coordinate order.

      x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
      y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
      z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);

      pad_to_minimums();
    }

    aabb(const aabb& box0, const aabb& box1) {
      x = interval(box0.x, box1.x);
      y = interval(box0.y, box1.y);
      z = interval(box0.z, box1.z);
    }

    const interval& axis_interval(int n) const {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const {
        const point3& ray_orig = r.origin();
        const vec3&   ray_dir  = r.direction();

        for (int axis = 0; axis < 3; axis++) {
            const interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }

    int longest_axis() const {
      // Returns the index of the longest axis of the bounding box.

      if (x.size() > y.size())
        return x.size() > z.size() ? 0 : 2;
      else
        return y.size() > z.size() ? 1 : 2;
    }

    aabb& operator+=(const aabb& b) {
      x = interval(x, b.x);
      y = interval(y, b.y);
      z = interval(z, b.z);
      return *this;
    }

    aabb& operator+=(const point3& p) {
      x = (p.x() < x.min) ? interval(p.x(), x.max) : ((p[0] > x.max) ? interval(x.min, p.x()) : x);
      y = (p.y() < y.min) ? interval(p.y(), y.max) : ((p[0] > y.max) ? interval(y.min, p.y()) : y);
      z = (p.z() < z.min) ? interval(p.z(), z.max) : ((p[0] > z.max) ? interval(z.min, p.z()) : z);
      return *this;
    }

    static const aabb empty, universe;

  private:

    void pad_to_minimums() {
      // Adjust the AABB so that no side is narrower than some delta, padding if necessary.

      double delta = 0.0001;
      if (x.size() < delta) x = x.expand(delta);
      if (y.size() < delta) y = y.expand(delta);
      if (z.size() < delta) z = z.expand(delta);
    }
};

inline aabb operator+(const aabb& a, const aabb& b) {
  return aabb(a, b);
}

inline aabb operator+(const aabb& b, const point3& p) {
  return aabb(
    (p.x() < b.x.min) ? interval(p.x(), b.x.max) : ((p[0] > b.x.max) ? interval(b.x.min, p.x()) : b.x),
    (p.y() < b.y.min) ? interval(p.y(), b.y.max) : ((p[0] > b.y.max) ? interval(b.y.min, p.y()) : b.y),
    (p.z() < b.z.min) ? interval(p.z(), b.z.max) : ((p[0] > b.z.max) ? interval(b.z.min, p.z()) : b.z)
  );
}

inline aabb operator+(const point3& p, const aabb& b) {
  return b + p;
}

#endif
