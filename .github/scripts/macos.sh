#!/bin/bash
set -euo pipefail

# Create build directory and enter it
mkdir bin
cd bin
# Create internal build and install directory
mkdir build
mkdir install

# Build in Release mode
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=23 -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BALLANCE=ON -DNEMO_BUILD_BMAP=ON -DNEMO_BUILD_BMAPINSPECTOR=ON -DYYCCommonplace_ROOT=$YYCCommonplace_ROOT -DSTB_ROOT=$STB_ROOT -DZLIB_ROOT=$ZLIB_ROOT ../..
cmake --build .
cmake --install . --prefix=../install
cd ..

# Back to root directory
cd ..
