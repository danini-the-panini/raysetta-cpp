#ifndef SAMPLER_H
#define SAMPLER_H

// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#include <cstring>
#ifdef _MSC_VER
  #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "vendor/stb_image.hpp"

#include "vendor/base64.hpp"

#include <iostream>

class image_data {
  public:
    image_data() {}

    image_data(std::string fname_or_url) : image_data(fname_or_url.c_str()) {}

    image_data(const char* fname_or_url) {
      if (strncmp(fname_or_url, "data:", 5) == 0) {
        load_data_url(fname_or_url);
      } else {
        load_file(fname_or_url);
      }
      // Loads image data from the specified file. If the image was not loaded successfully,
      // width() and height() will return 0.

      // Loads the linear (gamma=1) image data from the given file name. Returns true if the
      // load succeeded. The resulting data buffer contains the three [0.0, 1.0]
      // floating-point values for the first pixel (red, then green, then blue). Pixels are
      // contiguous, going left to right for the width of the image, followed by the next row
      // below, for the full height of the image.


      bytes_per_scanline = image_width * bytes_per_pixel;
      convert_to_bytes();
    }

    ~image_data() {
      if (bdata != nullptr) delete[] bdata;
      if (fdata != nullptr) STBI_FREE(fdata);
    }

    int width()  const { return (fdata == nullptr) ? 0 : image_width; }
    int height() const { return (fdata == nullptr) ? 0 : image_height; }

    const unsigned char* pixel_data(int x, int y) const {
      // Return the address of the three RGB bytes of the pixel at x,y. If there is no image
      // data, returns magenta.
      static unsigned char magenta[] = { 255, 0, 255 };
      if (bdata == nullptr) return magenta;

      x = clamp(x, 0, image_width);
      y = clamp(y, 0, image_height);

      return bdata + y*bytes_per_scanline + x*bytes_per_pixel;
    }

  private:
    const int      bytes_per_pixel = 3;
    float         *fdata = nullptr;         // Linear floating point pixel data
    unsigned char *bdata = nullptr;         // Linear 8-bit pixel data
    int            image_width = 0;         // Loaded image width
    int            image_height = 0;        // Loaded image height
    int            bytes_per_scanline = 0;

    void load_file(const char* image_filename) {
      auto n = bytes_per_pixel; // Dummy out parameter: original components per pixel
      fdata = stbi_loadf(image_filename, &image_width, &image_height, &n, bytes_per_pixel);
      if (fdata == nullptr) {
        std::cerr << "ERROR: Could not load image file '" << image_filename << "'.\n";
        return;
      }
    }

    void load_data_url(const char* data_url) {
      auto data = strchr(data_url, ',');
      if (data != nullptr) {
        data++;
      }
      auto decoded = base64::from_base64(data);
      auto n = bytes_per_pixel; // Dummy out parameter: original components per pixel
      fdata = stbi_loadf_from_memory((const stbi_uc*)decoded.c_str(), decoded.length(), &image_width, &image_height, &n, bytes_per_pixel);
      if (fdata == nullptr) {
        char snip[21] = {0};
        strncpy(snip, data_url, 20);
        std::cerr << "ERROR: Could not load base64 image '" << snip << "'.\n";
        return;
      }
    }

    static int clamp(int x, int low, int high) {
      // Return the value clamped to the range [low, high).
      if (x < low) return low;
      if (x < high) return x;
      return high - 1;
    }

    static unsigned char float_to_byte(float value) {
      if (value <= 0.0)
        return 0;
      if (1.0 <= value)
        return 255;
      return static_cast<unsigned char>(256.0 * value);
    }

    void convert_to_bytes() {
      // Convert the linear floating point pixel data to bytes, storing the resulting byte
      // data in the `bdata` member.

      int total_bytes = image_width * image_height * bytes_per_pixel;
      bdata = new unsigned char[total_bytes];

      // Iterate through all pixel components, converting from [0.0, 1.0] float values to
      // unsigned [0, 255] byte values.

      auto *bptr = bdata;
      auto *fptr = fdata;
      for (auto i=0; i < total_bytes; i++, fptr++, bptr++)
        *bptr = float_to_byte(*fptr);
    }
};

// Restore MSVC compiler warnings
#ifdef _MSC_VER
  #pragma warning (pop)
#endif

#endif
