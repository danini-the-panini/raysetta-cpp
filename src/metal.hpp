#ifndef METAL_H
#define METAL_H

#include "material.hpp"
#include "texture.hpp"

class metal : public material {
  public:
    metal(const color& albedo, double fuzz) : tex(make_shared<solid_color>(albedo)), fuzz(fuzz < 1 ? fuzz : 1) {}
    metal(shared_ptr<texture> tex, double fuzz) : tex(tex), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
      vec3 reflected = reflect(r_in.direction(), rec.normal);
      reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
      scattered = ray(rec.p, reflected, r_in.time());
      attenuation = tex->sample(rec.u, rec.v, rec.p);
      return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    shared_ptr<texture> tex;
    double fuzz;
};

#endif
