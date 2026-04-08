#ifndef SCENE_H
#define SCENE_H

#include "background.hpp"
#include "camera.hpp"
#include "hittable.hpp"

class scene {
  public:
    shared_ptr<hittable> world;
    shared_ptr<background> bg;
    camera_opts cam;

    scene(shared_ptr<hittable> world, shared_ptr<background> bg, const camera_opts& cam)
      : world(world), bg(bg), cam(cam) {}
};

#endif
