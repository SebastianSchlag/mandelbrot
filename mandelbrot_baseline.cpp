#include "defs.hpp"

std::unique_ptr<T[]> mandelbrot_baseline(
  T re_min, T re_max, size_t re_size,
  T im_min, T im_max, size_t im_size)
{

  std::unique_ptr<T[]> result(new T[re_size * im_size]);

// Compute difference between two pixels
  const T re_step = (re_max - re_min) / re_size;
  const T im_step = (im_max - im_min) / im_size;

  for(size_t py=0; py < im_size; py++) {
    const T im_coord = im_min + py * im_step;

    T re_coord = re_min;

    for(size_t px=0; px < re_size; px++) {
      re_coord += re_step;

      T re = re_coord;
      T im = im_coord;

      unsigned iter;
      for(iter = 0; iter < maxiter; iter++) {
        T re_sq = re * re;
        T im_sq = im * im;
        T re_im = re * im;
        T len_sq = re_sq + im_sq;

        if (len_sq > max_len_sq)
          break;

        re = re_sq - im_sq + re_coord;
        im = re_im + re_im + im_coord;
      }

      result[py * re_size + px] = static_cast<T>(iter - 1) / maxiter;
    }
  }

  return result;
}
