#ifndef CUBE_MAP_H
#define CUBE_MAP_H

#include "background.hpp"
#include "image_texture.hpp"
#include "ray.hpp"
#include "texture.hpp"

class cube_map : public background {
  public:
    cube_map(
      shared_ptr<texture> t0,
      shared_ptr<texture> t1,
      shared_ptr<texture> t2,
      shared_ptr<texture> t3,
      shared_ptr<texture> t4,
      shared_ptr<texture> t5
    ) : t0(t0), t1(t1), t2(t2), t3(t3), t4(t4), t5(t5)  {}

    cube_map(
      const char* f0,
      const char* f1,
      const char* f2,
      const char* f3,
      const char* f4,
      const char* f5
    ) : t0(make_shared<image_texture>(f0)),
        t1(make_shared<image_texture>(f1)),
        t2(make_shared<image_texture>(f2)),
        t3(make_shared<image_texture>(f3)),
        t4(make_shared<image_texture>(f4)),
        t5(make_shared<image_texture>(f5)) {}

    color sample_sphere(const ray& r) const override {
      auto dir = r.direction();
      auto x = fabs(dir.x());
      auto y = fabs(dir.y());
      auto z = fabs(dir.z());

      double ma, u, v;

      shared_ptr<texture> t;
      if (z >= x && z >= y) {
        // front/back
        t = dir.z() < 0.0 ? t5 : t4;
        ma = 0.5 / z;
        u = dir.z() < 0.0 ? -dir.x() : dir.x();
        v = dir.y();
      } else if (y >= x)  {
        // top/bottom
        t = dir.y() < 0.0 ? t3 : t2;
        ma = 0.5 / y;
        u = dir.x();
        v = dir.y() < 0.0 ? dir.z() : -dir.z();
      } else {
        // left/right
        t = dir.x() < 0.0 ? t1 : t0;
        ma = 0.5 / x;
        u = dir.x() < 0.0 ? dir.z() : -dir.z();
        v = dir.y();
      }

      u = u * ma + 0.5;
      v = v * ma + 0.5;
      return t->sample(u, v, unit_vector(dir));
    }

  private:
    shared_ptr<texture> t0;
    shared_ptr<texture> t1;
    shared_ptr<texture> t2;
    shared_ptr<texture> t3;
    shared_ptr<texture> t4;
    shared_ptr<texture> t5;
};

#endif
