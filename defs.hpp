#include <cstddef>
#include <memory>

using T = float;

constexpr T max_len_sq = 4;
constexpr unsigned maxiter = 512;

std::unique_ptr<T[]> mandelbrot_baseline(
  T re_min, T re_max, size_t re_size,
  T im_min, T im_max, size_t im_size);

std::unique_ptr<T[]> mandelbrot_intrinsics(
  T re_min, T re_max, size_t re_size,
  T im_min, T im_max, size_t im_size);

std::unique_ptr<T[]> mandelbrot_intrinsics2(
  T re_min, T re_max, size_t re_size,
  T im_min, T im_max, size_t im_size);

std::unique_ptr<T[]> mandelbrot_vc(
  T re_min, T re_max, size_t re_size,
  T im_min, T im_max, size_t im_size);