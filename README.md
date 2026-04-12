# Raysetta C++

A raytracer written in C++. Part of the [Raysetta project](https://github.com/danini-the-panini/raysetta). Based on [Ray Tracing in One Weekend](https://raytracing.github.io/).

## Building and Running

Make sure you have a C++20 compiler available (like GCC or clang)

```sh
make
bin/raysetta path/to/scene.json --width 400 --height 240 --samples 100 --depth 50 > output.ppm
```
