@ECHO OFF

:: Create build directory and enter it
MKDIR bin
CD bin
:: Create internal build and install directory
MKDIR build
MKDIR install

:: Build with x64 architecture in Release mode
CD build
:: We set this to revert the incompatible ABI for MSVC STL.
:: See: https://github.com/microsoft/STL/wiki/VS-2022-Changelog#vs-2022-1710
:: 
:: Ideally, I can install new VCRedist to resolve this issue.
:: However, Blender embeds its own VCRedist when distribution which has lower VCRedist version.
:: And at the same time, Blender will load our BMap.dll built by new VCRedist, so it trigger the incompatible ABI issue.
:: 
:: Currently, the VCRedist distributed by Blender 4.5 LTS is incompatible with our BMap.dll.
:: So I write it in script, rather than in CMake file, to temporaryly fix this issue.
:: 
:: This fix also is written in dependency build scripts, please remove them together if you remove this,
:: when Blender migrate to the new VCRedist.
set CXXFLAGS=/D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR=1
cmake -A x64 -DCMAKE_CXX_STANDARD=23 -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BALLANCE=ON -DNEMO_BUILD_BMAP=ON -DNEMO_BUILD_BMAPINSPECTOR=ON -DYYCCommonplace_ROOT=%YYCCommonplace_ROOT% -DSTB_ROOT=%STB_ROOT% -DZLIB_ROOT=%ZLIB_ROOT% ../..
cmake --build . --config Release
cmake --install . --prefix=../install --config Release
CD ..

:: Back to root directory
CD ..
