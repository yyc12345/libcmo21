#!/bin/bash
set -euo pipefail

# Create build and install directory
mkdir build
mkdir install

# Record install directory first because build step require it
cd install
export ZLIB_ROOT=$(pwd)
cd ..

# Build in Release mode
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=23 -DZLIB_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=$ZLIB_ROOT ..
cmake --build .
cmake --install .
cd ..
