#ifndef PARSER_H
#define PARSER_H

#include <exception>
#include <fstream>
#include <format>
#include <unordered_map>

#include "bvh_node.hpp"
#include "camera.hpp"
#include "checker_texture.hpp"
#include "cube_map.hpp"
#include "dielectric.hpp"
#include "diffuse_light.hpp"
#include "gradient.hpp"
#include "image_data.hpp"
#include "image_texture.hpp"
#include "lambertian.hpp"
#include "material.hpp"
#include "metal.hpp"
#include "noise_texture.hpp"
#include "perlin.hpp"
#include "quad.hpp"
#include "scene.hpp"
#include "solid_color.hpp"
#include "sphere.hpp"
#include "texture.hpp"
#include "tri.hpp"
#include "vendor/json.hpp"

using json = nlohmann::json;

class parse_exception : std::exception {
  private:
    std::string message;
  public:
    parse_exception(const char* msg) : message(msg) {}
    parse_exception(const std::string msg) : message(msg) {}

    const char* what() const noexcept {
      return message.c_str();
    }
};

class parser {
  public:
    parser(std::string filename) : data(json::parse(std::ifstream(filename))) {}

    scene parse() {
      parse_noises();
      parse_images();
      parse_textures();
      parse_materials();

      return scene(parse_world(), parse_bg(), parse_cam());
    }

  private:
    json data;
    std::unordered_map<std::string, shared_ptr<perlin> > noises;
    std::unordered_map<std::string, shared_ptr<image_data> > images;
    std::unordered_map<std::string, shared_ptr<texture> > textures;
    std::unordered_map<std::string, shared_ptr<material> > materials;

    void parse_noises() {
      auto obj = data["noises"];
      if (!obj.is_object() || obj.empty()) return;
      for (auto it = obj.begin(); it != obj.end(); ++it) {
        noises[it.key()] = parse_noise(it.value());
      }
    }

    void parse_images() {
      auto obj = data["images"];
      if (!obj.is_object() || obj.empty()) return;
      for (auto it = obj.begin(); it != obj.end(); ++it) {
        images[it.key()] = parse_image(it.value());
      }
    }

    void parse_textures() {
      auto obj = data["textures"];
      if (!obj.is_object() || obj.empty()) return;
      for (auto it = obj.begin(); it != obj.end(); ++it) {
        textures[it.key()] = parse_texture(it.value());
      }
    }

    void parse_materials() {
      auto obj = data["materials"];
      if (!obj.is_object() || obj.empty()) return;
      for (auto it = obj.begin(); it != obj.end(); ++it) {
        materials[it.key()] = parse_material(it.value());
      }
    }

    shared_ptr<perlin> parse_noise(json obj) {
      std::string type = obj["type"];
      if (type.compare("Perlin") == 0) {
        return make_shared<perlin>(
          parse_vec3_array(obj["randec"]),
          obj["permX"],
          obj["permY"],
          obj["permZ"]
        );
      } else {
        throw parse_exception(std::format("Unknown noise type `{}`", type));
      }
    }

    shared_ptr<image_data> parse_image(json obj) {
      std::string type = obj["type"];
      if (type.compare("Image") == 0) {
        return make_shared<image_data>(obj["data"]);
      } else {
        throw parse_exception(std::format("Unknown image type `{}`", type));
      }
    }

    shared_ptr<texture> parse_texture(json obj) {
      std::string type = obj["type"];
      if (type.compare("SolidColor") == 0) {
        return make_shared<solid_color>(parse_vec3(obj["albedo"]));
      } else if (type.compare("Checker") == 0) {
        return make_shared<checker_texture>(obj["scale"], parse_texture(obj["even"]), parse_texture(obj["odd"]));
      } else if (type.compare("Image") == 0) {
        return make_shared<image_texture>(images.at(obj["image"]));
      } else if (type.compare("Noise") == 0) {
        return make_shared<noise_texture>(obj["scale"], obj["depth"], parse_axis(obj["marble_axis"]), noises.at(obj["noise"]));
      } else {
        throw parse_exception(std::format("Unknown texture type `{}`", type));
      }
    }

    shared_ptr<material> parse_material(json obj) {
      std::string type = obj["type"];
      if (type.compare("Lambertian") == 0) {
        return make_shared<lambertian>(textures.at(obj["texture"]));
      } else if (type.compare("Metal") == 0) {
        return make_shared<metal>(textures.at(obj["texture"]), obj["fuzz"]);
      } else if (type.compare("Dielectric") == 0) {
        return make_shared<dielectric>(obj["refraction_index"]);
      } else if (type.compare("DiffuseLight") == 0) {
        return make_shared<diffuse_light>(textures.at(obj["texture"]));
      } else {
        throw parse_exception(std::format("Unknown material type `{}`", type));
      }
    }

    shared_ptr<hittable> parse_world() {
      json obj = data["world"];
      std::vector<shared_ptr<hittable>> objs;
      objs.reserve(obj.size());
      for (auto it = obj.begin(); it != obj.end(); ++it) {
        objs.push_back(parse_object(it.value()));
      }
      return make_shared<bvh_node>(objs);
    }

    shared_ptr<hittable> parse_object(json obj) {
      std::string type = obj["type"];
      if (type.compare("Sphere") == 0) {
        return make_shared<sphere>(parse_vec3(obj["center"]), obj["radius"], materials.at(obj["material"]));
      } else if (type.compare("MovingSphere") == 0) {
        return make_shared<sphere>(parse_vec3(obj["center1"]), parse_vec3(obj["center2"]), obj["radius"], materials.at(obj["material"]));
      } else if (type.compare("Quad") == 0) {
        return make_shared<quad>(parse_vec3(obj["q"]), parse_vec3(obj["u"]), parse_vec3(obj["u"]), materials.at(obj["material"]));
      } else if (type.compare("Tri") == 0) {
        return make_shared<tri>(parse_vec3(obj["a"]), parse_vec3(obj["b"]), parse_vec3(obj["c"]), materials.at(obj["material"]));
      } else if (type.compare("Box") == 0) {
        return box(parse_vec3(obj["a"]), parse_vec3(obj["b"]), materials.at(obj["material"]));
      } else {
        throw parse_exception(std::format("Unknown object type `{}`", type));
      }
    }

    shared_ptr<background> parse_bg() {
      json obj = data["background"];
      std::string type = obj["type"];
      if (type.compare("Solid") == 0) {
        return make_shared<solid_color>(parse_vec3(obj["albedo"]));
      } else if (type.compare("Gradient") == 0) {
        return make_shared<gradient>(parse_vec3(obj["top"]), parse_vec3(obj["bottom"]));
      } else if (type.compare("SphereMap") == 0) {
        return textures.at(obj["texture"]);
      } else if (type.compare("CubeMap") == 0) {
        return make_shared<cube_map>(
          textures.at(obj["textures"][0]),
          textures.at(obj["textures"][1]),
          textures.at(obj["textures"][2]),
          textures.at(obj["textures"][3]),
          textures.at(obj["textures"][4]),
          textures.at(obj["textures"][5])
        );
      } else {
        throw parse_exception(std::format("Unknown background type `{}`", type));
      }
    }

    camera_opts parse_cam() {
      json obj = data["camera"];
      return camera_opts {
        .vfov = parse_or_default(obj["vfov"], 90.0),
        .lookfrom = parse_vec3(obj["lookfrom"], vec3(0, 0, -1)),
        .lookat = parse_vec3(obj["lookat"], vec3(0, 0, -1)),
        .vup = parse_vec3(obj["vup"], vec3(0, 0, -1)),
        .defocus_angle = parse_or_default(obj["defocus_angle"], 90.0),
        .focus_dist = parse_or_default(obj["focus_dist"], 90.0)
      };
    }

    static vec3 parse_vec3(json arr) {
      if (arr.size() != 3)
        throw parse_exception(std::format("expected length {} but got {}", 3, arr.size()));
      return vec3(arr[0], arr[1], arr[2]);
    }

    static vec3 parse_vec3(json arr, vec3 def) {
      if (arr.is_null() || arr.empty()) return def;
      return parse_vec3(arr);
    }

    static int parse_axis(std::string ax) {
      if (ax.compare("x") == 0) return 0;
      if (ax.compare("y") == 0) return 1;
      if (ax.compare("z") == 0) return 2;
      throw parse_exception(std::format("Unknown axis `{}`", ax));
    }

    static std::vector<vec3> parse_vec3_array(json arr) {
      std::vector<vec3> vec3s;
      vec3s.reserve(arr.size());
      for (auto it: arr.get<std::vector<std::vector<double>>>()) {
        if (it.size() != 3)
          throw parse_exception(std::format("expected length {} but got {}", 3, it.size()));
        vec3s.push_back(vec3(it[0], it[1], it[2]));
      }
      return vec3s;
    }

    template <typename T>
    static T parse_or_default(json v, T def) {
      if (v.is_null()) return def;
      return v;
    }
};

#endif
