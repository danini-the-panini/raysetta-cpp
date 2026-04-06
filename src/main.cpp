#include "vendor/CLI11.hpp"

#include "util.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "camera.hpp"

color ray_color(const ray& r, int depth, const hittable& world) {
  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0)
    return color(0,0,0);

  hit_record rec;
  if (world.hit(r, interval(0.001, infinity), rec)) {
    vec3 direction = rec.normal + random_unit_vector();
    return 0.5 * ray_color(ray(rec.p, direction), depth - 1, world);
  }

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

  double pixel_samples_scale = 1.0 / samples;

  // World

  hittable_list world;

  world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
  world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

  camera cam(
    90.0,
    vec3(0.0, 0.0, 0.0),
    vec3(0.0, 0.0, -1.0),
    vec3(0.0, 1.0, 0.0),
    0.0,
    1.0
  );

  cam.viewport(width, height);

  std::cout << "P3\n" << width << ' ' <<height << "\n255\n";

  for (unsigned int j = 0; j < height; j++) {
    std::clog << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;
    for (unsigned int i = 0; i < width; i++) {
      color pixel_color(0,0,0);
      for (unsigned int sample = 0; sample < samples; sample++) {
        ray r = cam.get_ray(i, j);
        pixel_color += ray_color(r, depth, world);
      }
      write_color(std::cout, pixel_samples_scale * pixel_color);
    }
  }

  std::clog << "\rDone.                 \n";

  return 0;
}
