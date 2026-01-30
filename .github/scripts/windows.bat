@ECHO OFF

:: Create build directory and enter it
MKDIR bin
CD bin
:: Create internal build and install directory
MKDIR build
MKDIR install

:: Build with x64 architecture in Release mode
CD build
cmake -A x64 -DCMAKE_CXX_STANDARD=23 -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BALLANCE=ON -DNEMO_BUILD_BMAP=ON -DNEMO_BUILD_BMAPINSPECTOR=ON -DYYCCommonplace_ROOT=%YYCCommonplace_ROOT% -DSTB_ROOT=%STB_ROOT% -DZLIB_ROOT=%ZLIB_ROOT% ../..
cmake --build . --config Release
cmake --install . --prefix=../install --config Release
CD ..

:: Back to root directory
CD ..
