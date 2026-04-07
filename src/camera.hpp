#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.hpp"
#include "ray.hpp"
#include "util.hpp"

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
      auto theta = degrees_to_radians(vfov);
      auto h = std::tan(theta/2);
      auto viewport_height = 2 * h * focus_dist;
      auto viewport_width = viewport_height * aspect_ratio;

      // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
      w = unit_vector(lookfrom - lookat);
      u = unit_vector(cross(vup, w));
      v = cross(w, u);

      // Calculate the vectors across the horizontal and down the vertical viewport edges.
      vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
      vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

      // Calculate the horizontal and vertical delta vectors from pixel to pixel.
      pixel_delta_u = viewport_u / width;
      pixel_delta_v = viewport_v / height;

      // Calculate the location of the upper left pixel.
      auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
      pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

      // Calculate the camera defocus disk basis vectors.
      auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
      defocus_disk_u = u * defocus_radius;
      defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
      // Construct a camera ray originating from the origin and directed at randomly sampled
      // point around the pixel location i, j.

      auto offset = sample_square();
      auto pixel_sample = pixel00_loc
        + ((i + offset.x()) * pixel_delta_u)
        + ((j + offset.y()) * pixel_delta_v);

      auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
      auto ray_direction = pixel_sample - ray_origin;

      return ray(ray_origin, ray_direction, random_double());
    }

    vec3 sample_square() const {
      // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
      return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
      // Returns a random point in the camera defocus disk.
      auto p = random_in_unit_disk();
      return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

  private:
    int    height;         // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;        // Camera frame basis vectors
    vec3   defocus_disk_u; // Defocus disk horizontal radius
    vec3   defocus_disk_v; // Defocus disk vertical radius
};

#endif
