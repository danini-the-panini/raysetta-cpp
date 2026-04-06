#include "vendor/CLI11.hpp"

#include "util.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "lambertian.hpp"
#include "metal.hpp"
#include "dielectric.hpp"

color ray_color(const ray& r, int depth, const hittable& world) {
  // If we've exceeded the ray bounce limit, no more light is gathered.
  if (depth <= 0)
    return color(0,0,0);

  hit_record rec;
  if (world.hit(r, interval(0.001, infinity), rec)) {
    ray scattered;
    color attenuation;
    if (rec.mat->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_color(scattered, depth-1, world);
    return color(0,0,0);
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

  auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
  auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
  auto material_left   = make_shared<dielectric>(1.50);
  auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
  auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

  world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
  world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   0.5, material_center));
  world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
  world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.4, material_bubble));
  world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

  camera cam(
    20.0,
    point3(-2,2,1),
    point3(0,0,-1),
    vec3(0,1,0),
    10.0,
    3.4
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
