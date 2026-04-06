#include <iostream>
#include <string>

#include "vendor/CLI11.hpp"

#include "vec3.hpp"
#include "color.hpp"

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

  std::cout << "P3\n" << width << ' ' <<height << "\n255\n";

  for (unsigned int j = 0; j < height; j++) {
    std::clog << "\rScanlines remaining: " << (height - j) << ' ' << std::flush;
    for (unsigned int i = 0; i < width; i++) {
      auto pixel_color = color(double(i)/(width-1), double(j)/(height-1), 0);
      write_color(std::cout, pixel_color);
    }
  }

  std::clog << "\rDone.                 \n";

  return 0;
}
