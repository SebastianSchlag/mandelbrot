#include "defs.hpp"
#include <immintrin.h>

std::unique_ptr<T[]> mandelbrot_intrinsics(
  const T re_min, const T re_max, size_t re_size,
  const T im_min, const T im_max, size_t im_size)
{
  constexpr size_t packing = 8;

  std::unique_ptr<T[]> result(reinterpret_cast<T*>(
    aligned_alloc(sizeof(T) * packing, re_size * im_size * sizeof(T))));

// Obtain a vector containing the max_length in vector format
  const __m256 max_len_sq_v = _mm256_set1_ps(max_len_sq);
  const __m256 all_increment = _mm256_set1_ps(1.f / maxiter);

// Compute difference between two pixels
  const T re_step = (re_max - re_min) / re_size;
  const __m256 re_step_v = _mm256_set1_ps(re_step * packing);

  const T im_step = (im_max - im_min) / im_size;

  __m256 re_min_v;
  {
    __m256 shift = _mm256_set_ps(
      7*re_step, 6*re_step, 5*re_step, 4*re_step,
      3*re_step, 2*re_step, 1*re_step, 0*re_step
    );

    re_min_v = _mm256_add_ps(_mm256_set1_ps(re_min), shift);
  }

  for(size_t py=0; py < im_size; py++) {
    const __m256 im_coord = _mm256_set1_ps(im_min + py * im_step);

    __m256 re_coord = re_min_v;
    for(size_t px=0; px < re_size; px += packing) {
      __m256 re = re_coord;
      __m256 im = im_coord;

      __m256 counts = _mm256_setzero_ps();
      for(size_t iter = 0; iter < maxiter; iter++) {
        __m256 re_sq = _mm256_mul_ps(re, re);
        __m256 im_sq = _mm256_mul_ps(im, im);
        __m256 re_im = _mm256_mul_ps(re, im);
        __m256 len_sq = _mm256_add_ps(re_sq, im_sq);

        // masking
        __m256 not_diverged = _mm256_cmp_ps(len_sq, max_len_sq_v, _CMP_LT_OS);
        __m256 tmp = _mm256_and_ps(all_increment, not_diverged);
        counts = _mm256_add_ps(counts, tmp);

        // check if all diverged
        int mask = _mm256_movemask_ps(not_diverged);
        if (!mask) break;

        re = re_sq - im_sq + re_coord;
        im = re_im + re_im + im_coord;
      }

      _mm256_store_ps(&result[py*re_size + px], counts);
      re_coord = _mm256_add_ps(re_coord, re_step_v);
    }
  }

  return result;
}
