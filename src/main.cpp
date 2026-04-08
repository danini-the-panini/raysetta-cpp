#include "vendor/CLI11.hpp"

#include "util.hpp"
#include "vec3.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"
#include "bvh_node.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "lambertian.hpp"
#include "metal.hpp"
#include "dielectric.hpp"
#include "checker_texture.hpp"
#include "image_texture.hpp"
#include "noise_texture.hpp"
#include "quad.hpp"
#include "diffuse_light.hpp"
#include "solid_color.hpp"
#include "gradient.hpp"
#include "scene.hpp"
#include "tracer.hpp"

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

  std::string scene_path;
  app.add_option("scene", scene_path, "Scene file")->required();
  app.validate_positionals();

  CLI11_PARSE(app, argc, argv);

  std::cerr << "Scene = " << scene_path << '\n';

  // World

  hittable_list objects;

  auto red   = make_shared<lambertian>(color(.65, .05, .05));
  auto white = make_shared<lambertian>(color(.73, .73, .73));
  auto green = make_shared<lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color(15, 15, 15));

  objects.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
  objects.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
  objects.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
  objects.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
  objects.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
  objects.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

  shared_ptr<hittable> world = make_shared<bvh_node>(objects);

  // shared_ptr<background> bg = make_shared<gradient>(color(0.5, 0.7, 1.0), color(1.0, 1.0, 1.0));
  shared_ptr<background> bg = make_shared<solid_color>(0.0, 0.0, 0.0);

  camera_opts cam;

  cam.vfov          = 40;
  cam.lookfrom      = point3(278, 278, -800);
  cam.lookat        = point3(278, 278, 0);
  cam.vup           = vec3(0,1,0);
  cam.focus_dist    = 10.0;
  cam.defocus_angle = 0;

  scene scn(world, bg, cam);

  tracer tr = tracer(
    width,
    height,
    samples,
    depth,
    scn
  );

  std::cout << "P3\n" << width << ' ' <<height << "\n255\n";

  for (unsigned int y = 0; y < height; y++) {
    std::clog << "\rScanlines remaining: " << (height - y) << ' ' << std::flush;
    for (unsigned int x = 0; x < width; x++) {
      write_pixel(std::cout, tr.trace(x, y));
    }
  }

  std::clog << "\rDone.                 \n";

  return 0;
}
