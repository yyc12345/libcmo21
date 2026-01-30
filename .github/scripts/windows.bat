@ECHO OFF

:: Create build directory and enter it
MKDIR bin
CD bin
:: Create internal build and install directory, then enter it
MKDIR build
MKDIR install
CD build

:: Build with x64 architecture in Release mode
cmake -A x64 -DYYCC_BUILD_TEST=ON -DGTest_ROOT=%GTest_ROOT% -DYYCC_BUILD_BENCHMARK=ON -Dbenchmark_ROOT=%benchmark_ROOT% ../..
cmake --build . --config Release
cmake --install . --prefix=../install --config Release

:: Back to root directory
CD ..
CD ..
