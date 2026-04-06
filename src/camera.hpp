#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.hpp"
#include "ray.hpp"

class camera {
  public:
    double vfov;
    point3 lookfrom;
    point3 lookat;
    point3 vup;

    double defocus_angle;
    double focus_dist;

    camera(
      const double vfov,
      const point3 lookfrom,
      const point3 lookat,
      const point3 vup,

      const double defocus_angle,
      const double focus_dist
    ) : vfov(vfov),
        lookfrom(lookfrom),
        lookat(lookat),
        vup(vup),
        defocus_angle(defocus_angle),
        focus_dist(focus_dist) {};

    void viewport(unsigned int width, unsigned int height) {
      height = (height < 1) ? 1 : height;
      auto aspect_ratio = (double)width / (double)height;

      center = lookfrom;

      // Determine viewport dimensions.
      auto viewport_height = 2.0;
      auto viewport_width = viewport_height * aspect_ratio;

      // Calculate the vectors across the horizontal and down the vertical viewport edges.
      auto viewport_u = vec3(viewport_width, 0, 0);
      auto viewport_v = vec3(0, -viewport_height, 0);

      // Calculate the horizontal and vertical delta vectors from pixel to pixel.
      pixel_delta_u = viewport_u / width;
      pixel_delta_v = viewport_v / height;

      // Calculate the location of the upper left pixel.
      auto viewport_upper_left =
        center - vec3(0, 0, focus_dist) - viewport_u/2 - viewport_v/2;
      pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    ray get_ray(unsigned int i, unsigned int j) {
      auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
      auto ray_direction = pixel_center - center;
      return ray(center, ray_direction);
    }

  private:
    int    height;         // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
};

#endif
