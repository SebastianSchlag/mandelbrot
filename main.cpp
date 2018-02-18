#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <cmath>

#include <numeric>
#include <algorithm>
#include <memory>

#include <iostream>
#include <png.h>

#include "defs.hpp"
#include "ScopedTimer.hpp"

void render_image(
  const std::string& fn,
  size_t width, size_t height,
  T* iters) {

  auto fp = fopen(fn.c_str(), "wb");
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop info_ptr = png_create_info_struct(png_ptr);

  png_init_io(png_ptr, fp);
  // Write header (8 bit colour depth)
  png_set_IHDR(png_ptr, info_ptr, width, height, 8,
               PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  std::unique_ptr<png_byte[]> row ( new png_byte[ 3*width ] );

  auto set_rgb = [&row] (size_t x, T value) {
      int v = static_cast<int>(767 * value);
      if (v < 0) v = 0;
      if (v > 767) v = 767;
      const int o = v % 256;

      png_byte* ptr = &row[3*x];

      if (v<256)      {ptr[0] = 0; ptr[1] = 0;     ptr[2] = o;}
      else if (v<512) {ptr[0] = 0; ptr[1] = o;     ptr[2] = 255-o;}
      else            {ptr[0] = o; ptr[1] = 255-o; ptr[2] = 0;}
  };

  for (size_t y=0; y < height; y++) {
    for (size_t x=0; x<width; x++) {
      set_rgb(x, iters[y*width + x]);
    }

    png_write_row(png_ptr, row.get());
  }

  png_write_end(png_ptr, nullptr);
  fclose(fp);
  png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
  png_destroy_write_struct(&png_ptr, nullptr);
}


int main() {
  const size_t size_y = 1 << 10;
  const size_t size_x = size_y / 2 * 3;

/*
  auto* iters = reinterpret_cast<T*>(
    aligned_alloc(64, sizeof(T)* size_x * size_y));
*/
/* Hint compiler that image size is a multiple of 8
  constexpr size_t padding = 8;
  re_size = ((re_size + padding - 1) / padding) * padding;
  im_size = ((im_size + padding - 1) / padding) * padding;
*/

#ifdef WITH_VC
  if (true) {
    ScopedTimer timer("Computation VC");
    auto mandel = mandelbrot_vc(-2.0, 1.0, size_x, -1.0, 1.0, size_y);
    render_image("mandel_vc.png", size_x, size_y, mandel.get());
  }
#endif

#ifdef WITH_PIK
  if (true) {
    ScopedTimer timer("Computation PIK");
    auto mandel = mandelbrot_pik(-2.0, 1.0, size_x, -1.0, 1.0, size_y);
    render_image("mandel_pik.png", size_x, size_y, mandel.get());
  }
#endif

  double time_base;
  {
    ScopedTimer timer(time_base, "Computation Base");
    auto mandel = mandelbrot_baseline(-2.0, 1.0, size_x, -1.0, 1.0, size_y);
    render_image("mandel_base.png", size_x, size_y, mandel.get());
  }

  {
    ScopedTimer timer("Computation Intrinsics", time_base);
    auto mandel = mandelbrot_intrinsics(-2.0, 1.0, size_x, -1.0, 1.0, size_y);
    render_image("mandel_intrinsics.png", size_x, size_y, mandel.get());
  }

  {
    ScopedTimer timer("Computation Intrinsics2", time_base);
    auto mandel = mandelbrot_intrinsics2(-2.0, 1.0, size_x, -1.0, 1.0, size_y);
    render_image("mandel_intrinsics2.png", size_x, size_y, mandel.get());
  }

  return 0;
}