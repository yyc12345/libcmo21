@ECHO OFF

:: Create build and install directory
MKDIR build
MKDIR install

:: Record install directory first because build step require it
CD install
SET ZLIB_ROOT=%CD%
CD ..

:: Build with x64 architecture in Release mode
CD build
cmake -A x64 -DCMAKE_CXX_STANDARD=23 -DZLIB_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=%ZLIB_ROOT% ..
cmake --build . --config Release
cmake --install . --config Release
CD ..
