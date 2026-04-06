#include <iostream>
#include <string>

#include "vendor/CLI11.hpp"

#include "vec3.hpp"
#include "color.hpp"
#include "ray.hpp"

color ray_color(const ray& r) {
  vec3 unit_direction = unit_vector(r.direction());
  auto a = 0.5*(unit_direction.y() + 1.0);
  return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
}

int main(int argc, char** argv) {
  CLI::App app{"raysetta c++ raytracer"};
  argv = app.ensure_utf8(argv);

  app.set_help_flag();
  app.set_help_all_flag("--help");

  unsigned int width{256};
  app.add_option("-w,--width", width, "Image width (default 256)")->check(CLI::Number);

  unsigned int height{256};
  app.add_option("-h,--height", height, "Image height (default 256)")->check(CLI::Number);

  unsigned int samples{10};
  app.add_option("-s,--samples", samples, "Samples per pixel (default 10)")->check(CLI::Number);

  unsigned int depth{10};
  app.add_option("-d,--depth", depth, "Max depth (default 10)")->check(CLI::Number);

  std::string scene;
  app.add_option("scene", scene, "Scene file")->required();
  app.validate_positionals();

  CLI11_PARSE(app, argc, argv);

  std::cerr << "Scene = " << scene << '\n';

  auto aspect_ratio = (float)width / (float)height;

  // Calculate the image height, and ensure that it's at least 1.
  height = (unsigned int)(width / aspect_ratio);
  height = (height < 1) ? 1 : height;

  // Camera

  auto focal_length = 1.0;
  auto viewport_height = 2.0;
  auto viewport_width = viewport_height * (double(width)/height);
  auto camera_center = point3(0, 0, 0);

  // Calculate the vectors across the horizontal and down the vertical viewport edges.
  auto viewport_u = vec3(viewport_width, 0, 0);
  auto viewport_v = vec3(0, -viewport_height, 0);

  // Calculate the horizontal and vertical delta vectors from pixel to pixel.
  auto pixel_delta_u = viewport_u / width;
  auto pixel_delta_v = viewport_v / height;

  // Calculate the location of the upper left pixel.
  auto viewport_upper_left = camera_center
    - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
  auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  std::cout << "P3\n" << width << ' ' <<height << "\n255\n";

  for (unsigned int j = 0; j < height; j++) {
    std::clog << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;
    for (unsigned int i = 0; i < width; i++) {
      auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
      auto ray_direction = pixel_center - camera_center;
      ray r(camera_center, ray_direction);

      color pixel_color = ray_color(r);
      write_color(std::cout, pixel_color);
    }
  }

  std::clog << "\rDone.                 \n";

  return 0;
}
