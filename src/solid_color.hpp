#ifndef SOLID_COLOR_H
#define SOLID_COLOR_H

#include "texture.hpp"
#include "background.hpp"
#include "color.hpp"

class solid_color : public texture, public background {
  public:
    solid_color(const color& albedo) : albedo(albedo) {}

    solid_color(double red, double green, double blue) : solid_color(color(red,green,blue)) {}

    color sample(double, double, const point3&) const override {
      return albedo;
    }

    color sample_sphere(const ray&) const override {
      return albedo;
    }

  private:
    color albedo;
};

#endif
