#ifndef NOISE_TEXTURE_H
#define NOISE_TEXTURE_H

#include "perlin.hpp"
#include "texture.hpp"
#include <memory>

class noise_texture : public texture {
  public:
    noise_texture(double scale, int depth, int marble_axis)
      : scale(scale), depth(depth), marble_axis(marble_axis), noise(make_shared<perlin>()) {}

    noise_texture(double scale, int depth, int marble_axis, shared_ptr<perlin> noise)
      : scale(scale), depth(depth), marble_axis(marble_axis), noise(noise) {}

    color value(double, double, const point3& p) const override {
      if (marble_axis >= 0) {
        return color(0.5, 0.5, 0.5) * (1.0 + std::sin(scale * p[marble_axis] + 10 * noise->turb(p, depth)));
      } else {
        return color(1.0, 1.0, 1.0) * noise->turb(p, depth);
      }
    }

  private:
    double scale;
    int depth;
    int marble_axis;
    std::shared_ptr<perlin> noise;
};

#endif
