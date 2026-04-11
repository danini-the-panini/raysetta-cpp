#include "vendor/CLI11.hpp"

#include "color.hpp"
#include "parser.hpp"
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

  scene scn = parser(scene_path).parse();
  tracer tr = tracer(width, height, samples, depth, scn);

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
