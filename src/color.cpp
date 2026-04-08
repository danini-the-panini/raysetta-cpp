#include "color.hpp"
#include "interval.hpp"

inline double linear_to_gamma(double linear_component)
{
  if (linear_component > 0)
    return std::sqrt(linear_component);

  return 0;
}

pixel to_pixel(const color& c) {
  auto r = c.x();
  auto g = c.y();
  auto b = c.z();

  // Apply a linear to gamma transform for gamma 2
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  // Translate the [0,1] component values to the byte range [0,255].
  static const interval intensity(0.000, 0.999);
  unsigned char rbyte = (unsigned char)(256 * intensity.clamp(r));
  unsigned char gbyte = (unsigned char)(256 * intensity.clamp(g));
  unsigned char bbyte = (unsigned char)(256 * intensity.clamp(b));

  return pixel { .r = rbyte, .g = gbyte, .b = bbyte };
}

void write_pixel(std::ostream& out, const pixel& px) {
  // Write out the pixel color components.
  out << (short)px.r << ' ' << (short)px.g << ' ' << (short)px.b << '\n';
}
