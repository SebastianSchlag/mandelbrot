cmake_minimum_required(VERSION 3.5)
project(vector)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -O3 -mavx -mavx2 -fopenmp")

set(SRC_FILES
        mandelbrot_baseline.cpp
        mandelbrot_intrinsics.cpp
        mandelbrot_intrinsics2.cpp
)

if(1)
    include(extern/LocalVcConfig.cmake REQUIRED)
    list(APPEND SRC_FILES "mandelbrot_vc.cpp")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWITH_VC")
endif(USE_VC)


add_executable(mandelbrot main.cpp
    ${SRC_FILES}
)

target_link_libraries(mandelbrot png)