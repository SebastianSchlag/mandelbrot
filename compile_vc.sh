#!/bin/bash
export CXX="g++"
export CC="gcc"
ARCH=broadwell

git submodule update --init

CXX_VERSION=$($CXX -dumpversion | cut -f1 -d.)
if [ "$CXX_VERSION" -lt "5" ]; then
   echo "Only tested with g++5 (or higher); may work anyways"
   exit
fi

if ! grep -q "avx2" /proc/cpuinfo; then
   echo "It seems that your CPU does not support AVX2"
   echo "Please adopt the ARCH parameter in this file and comment"
   echo "out this check; you may experience a reduced performance"
   echo "see libs/Vc/cmake/OptimizeForArchitecture.cmake"
   exit
fi

CORES=$(cat /proc/cpuinfo | grep "processor" | wc -l)
echo "Use $CORES cores for building"

echo "Build libVc"
BDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
VCDIR="$BDIR/extern/Vc/"
VCDIRInst="$VCDIR/install_$CC"
export Vc_DIR=

# BUILD VC
cd $VCDIR
    rm -rf build_$CC $VCDIRInst
    mkdir build_$CC
    cd build_$CC
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$VCDIRInst -DTARGET_ARCHITECTURE=$ARCH -DBUILD_TESTING=OFF ..
    make -j $CORES
    make install

cd $BDIR

cat << EOF > extern/LocalVcConfig.cmake
set(TARGET_ARCHITECTURE "$ARCH")
set(ENV{Vc_DIR} "$VCDIRInst/lib/cmake/Vc/")
set(Vc_LIBRARIES ENV{Vc_DIR}) #bug-fix?


find_package(Vc REQUIRED)
add_compile_options(\${Vc_ALL_FLAGS})
message("VC Dir: \${Vc_INCLUDE_DIR}")
message("VC Flags: \${Vc_ALL_FLAGS}")
include_directories(\${Vc_INCLUDE_DIR})
link_directories("\${Vc_INCLUDE_DIR}/../lib/")

set(CMAKE_CXX_FLAGS "\${CMAKE_CXX_FLAGS} -DUSE_VC")


EOF