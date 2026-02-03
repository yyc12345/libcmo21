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
cmake -DCMAKE_CXX_STANDARD=23 -DCMAKE_BUILD_TYPE=Release ../..
cmake --build .
cmake --install . --prefix=../install
cd ..

# Record install directory
cd install
export YYCCommonplace_ROOT=$(pwd)
cd ..

# Back to root directory
cd ..
