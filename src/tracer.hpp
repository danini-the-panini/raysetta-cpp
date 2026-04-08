#ifndef TRACER_H
#define TRACER_H

#include "scene.hpp"
#include "color.hpp"
#include "material.hpp"

class tracer {
  public:
    tracer(unsigned int width, unsigned int height, unsigned int samples, unsigned int depth, scene& scn)
      : width(width), height(height), samples(samples), depth(depth), pixel_samples_scale(1.0 / samples), scn(scn), cam(camera(width, height, scn.cam)) {}

    pixel trace(unsigned int x, unsigned int y) const {
      color pixel_color(0,0,0);
      for (unsigned int sample = 0; sample < samples; sample++) {
        ray r = cam.get_ray(x, y);
        pixel_color += ray_color(r, depth);
      }
      return to_pixel(pixel_samples_scale * pixel_color);
    }

  private:
    unsigned int width;
    unsigned int height;
    unsigned int samples;
    unsigned int depth;
    double pixel_samples_scale;
    scene scn;
    camera cam;

    color ray_color(const ray& r, unsigned int depth) const {
      // If we've exceeded the ray bounce limit, no more light is gathered.
      if (depth <= 0)
        return color(0,0,0);

      hit_record rec;
      // If the ray hits nothing, return the background color.
      if (!scn.world->hit(r, interval(0.001, infinity), rec))
          return scn.bg->sample_sphere(r);

      ray scattered;
      color attenuation;
      color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

      if (!rec.mat->scatter(r, rec, attenuation, scattered))
          return color_from_emission;

      color color_from_scatter = attenuation * ray_color(scattered, depth-1);

      return color_from_emission + color_from_scatter;
    }
};

#endif
