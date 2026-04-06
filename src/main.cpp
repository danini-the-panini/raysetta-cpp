#include "vendor/CLI11.hpp"

#include <iostream>
#include <string>

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
      for (unsigned int i = 0; i < width; i++) {
          auto r = double(i) / (width-1);
          auto g = double(j) / (height-1);
          auto b = 0.0;

          int ir = int(255.999 * r);
          int ig = int(255.999 * g);
          int ib = int(255.999 * b);

          std::cout << ir << ' ' << ig << ' ' << ib << '\n';
      }
  }

  return 0;
}
