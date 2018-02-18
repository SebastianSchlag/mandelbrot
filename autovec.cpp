#define BLACK_MAGIC \
    a = reinterpret_cast<float*>(__builtin_assume_aligned(a, 64)); \
    b = reinterpret_cast<float*>(__builtin_assume_aligned(b, 64));




#include <cstddef>

constexpr size_t padding = 8;

void vector_add(
  float* a, // stored in rdi
  float* b, // stored in rdi
  size_t n  // stored in rsi
) {
  BLACK_MAGIC

#pragma omp simd
  for(size_t i=0; i<padding*n; i++)
    a[i] += b[i];
}  