#include <cstdlib>
#include <Vc/simd>

#include "defs.hpp"

using Tv = Vc::simd<T>;

std::unique_ptr<T[]> mandelbrot_vc(
  T re_min, T re_max, size_t re_size,
  T im_min, T im_max, size_t im_size,
  bool task_parallel
) {
  std::unique_ptr<T[]> result(reinterpret_cast<T*>(
    aligned_alloc(sizeof(Tv), re_size * im_size * sizeof(T))));

  // Compute distance between two pixels
  const T re_step = (re_max - re_min) / re_size;
  const T im_step = (im_max - im_min) / im_size;

  // Define vectorised constants
  const Tv re_step_v(re_step * Tv::size());
  const Tv max_len_sq_v(max_len_sq);
  const Tv inv_maxiter(1.f / maxiter);

  // Will work on Tv::size() columns in parallel, so compute position
  // of first Tv::size() points
  Tv re_min_v;
  {
    for (size_t i = 0; i < Tv::size(); i++)
      re_min_v[i] = static_cast<T>(re_min + i * re_step);
  }

  // foreach line ...
  #pragma omp parallel for if (task_parallel)
  for(size_t py=0; py < im_size; py++) {
    const Tv im_coord (im_min + py * im_step);

    // foreach column ...
    Tv re_coord = re_min_v;
    for(size_t px=0; px < re_size; px += Tv::size() ) {
      Tv re = re_coord;
      Tv im = im_coord;

      // compute iterations until divergence
      Tv iter_v(0.f);
      for(size_t iter = 0; iter < maxiter; iter++) {
        const Tv re_sq = re * re;
        const Tv im_sq = im * im;
        const Tv re_im = re * im;
        const Tv len_sq = re_sq + im_sq;

        Tv::mask_type not_diverged = len_sq < max_len_sq_v;
        if (Vc::none_of(not_diverged)) break;

        Vc::where(not_diverged, iter_v) += inv_maxiter;

        re = re_sq - im_sq + re_coord;
        im = re_im + re_im + im_coord;
      }

      // store result
      iter_v.copy_to(&result[py*re_size + px], Vc::flags::vector_aligned);

      // next column
      re_coord += re_step_v;
    }
  }

  return result;
}
