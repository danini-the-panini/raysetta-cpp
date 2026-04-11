#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "interval.hpp"
#include "image_data.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include <memory>

class image_texture : public texture {
  public:
    image_texture(shared_ptr<image_data> img) : image(img) {}
    image_texture(const char* filename) : image(make_shared<image_data>(filename)) {}

    color sample(double u, double v, const point3&) const override {
      // If we have no texture data, then return solid cyan as a debugging aid.
      if (image->height() <= 0) return color(0,1,1);

      // Clamp input texture coordinates to [0,1] x [1,0]
      u = interval(0,1).clamp(u);
      v = 1.0 - interval(0,1).clamp(v);  // Flip V to image coordinates

      auto i = int(u * image->width());
      auto j = int(v * image->height());
      auto pixel = image->pixel_data(i,j);

      auto color_scale = 1.0 / 255.0;
      return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }

  private:
    std::shared_ptr<image_data> image;
};

#endif
