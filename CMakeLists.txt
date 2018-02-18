cmake_minimum_required(VERSION 3.9)
project(vector)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -mavx -mavx2 -fopenmp")

set(SRC_FILES
        "mandelbrot_baseline.cpp"
        "mandelbrot_intrinsics.cpp"
)

add_executable(mandelbrot main.cpp
    ${SRC_FILES}
)

target_link_libraries(mandelbrot png)