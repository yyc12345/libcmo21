@ECHO OFF

:: Create build directory and enter it
MKDIR bin
CD bin
:: Create internal build and install directory
MKDIR build
MKDIR install

:: Build with x64 architecture in Release mode
CD build
cmake -A x64 -DCMAKE_CXX_STANDARD=23 ../..
cmake --build . --config Release
cmake --install . --prefix=../install --config Release
CD ..

:: Record install directory
CD install
SET YYCCommonplace_ROOT=%CD%
CD ..

:: Back to root directory
CD ..
