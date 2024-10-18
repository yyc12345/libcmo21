# Compile Manual

## Requirements

This project require:

* CMake 3.23 at least.
* The common compiler supporting C++ 20 (GCC / Clang / MSVC).
* Littile-endian architecture system.
* zlib.
* iconv (non-Windows system required).
* [stb](https://github.com/nothings/stb): For image read and write.
* [YYCCommonplace](https://github.com/yyc12345/YYCCommonplace): My personal invented library concentrating some common functions for mine.

Since libcmo21 0.2.0, we only provide it in CMake build system. So Windows user should install CMake first or use Visual Studio directly (Visual Studio provides CMake build system supports).

## Preparations

### YYCCommonplace

You should clone YYCCommonplace and switch to the latest release tag (or specified commit hash provided with libcmo21 release infos if you are building for specific libcmo21 version). Then compile it **in C++ 20 mode** (this is crucial because libcmo21 use C++ 20, and YYCCommonplace's ABI is incompatible between C++ 17 version and C++ 20 version). Finally install it as CMake package.

Please note if your final program or dynamic library is provided for end user, please choose `RelWithDebInfo` build type (`Release` is not suggested because it will strip all debug infos and it is not good for bug reporter, which is embedded in program, to report crash). If final program is served for programmer debugging, please choose `Debug` build type.

### stb

You should clone stb repository first. In ideally scenario, we suggest you switch to the latest commit. However, all builds are actually only run on a specific commit hash `5736b15f7ea0ffb08dd38af21067c314d6a3aae9`. So if the latest commit is not work, please switch to this commit hash instead.

### zlib

If you are in Windows, you should download zlib source code and build it with given Visual Studio solution files. The final file we needed is `zlibwapi.dll` with `ZlibDllRelease` build type.

If you are running on non-Windows system. You usually do not need to do anything. Because zlib development environment may be configured by your package manager correctly.

## Compile

### Directory Hierarchy

First, create subdirectory `Bin/build` and `Bin/install`.

### Configuration

Then enter subdirectory `Bin/build` and use following command to configure CMake:

- Windows: `cmake -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BMAP=ON -DNEMO_BUILD_DOC=OFF -DSTB_IMAGE_PATH=<path-to-stb> -DYYCC_PATH=<path-to-yycc-install> -DZLIB_HEADER_PATH=<path-to-zlib-hdr> -DZLIB_BINARY_PATH=<path-to-zlib-bin> ../..`
- non-Windows: `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BMAP=ON -DNEMO_BUILD_DOC=OFF -DSTB_IMAGE_PATH=<path-to-stb> -DYYCC_PATH=<path-to-yycc-install> ../..`

The arguments in command should be replaced by:

* `<path-to-stb>`: The root directory of your cloned stb repository.
* `<path-to-yycc-install>`: The directory to installed CMake package you chosen when building YYCCommonplace.
* `<path-to-zlib-hdr>` (Windows only): The root directory of downloaded zlib source code.
* `<path-to-zlib-bin>` (Windows only): The directory where you can find built `zlibwapi.dll`.

The switches in command can be switched as you wish:

* `NEMO_BUILD_UNVIRT`: Build `unvirt`, a command line application debugging Virtools files.
* `NEMO_BUILD_BMAP`: Build `BMap`, a dynamic library specific used for loading Ballance map file.
* `NEMO_BUILD_DOC`: Build the document of libcmo21.

### Build

Execute following command to build libcmo21.

* Windows：`cmake --build . --config RelWithDebInfo`
* non-Windows：`cmake --build .`

### Build Type

Like YYCCommonplace, we suggest `RelWithDebInfo` for end user. If you want to build for programmer debugging, please replace all `RelWithDebInfo` to `Debug`. The build type between YYCCommonplace and libcmo21 should keep same to reduce any possibility about ABI incompatible issue.

## Install

Currently the CMake install script still has some bugs and can not work as expected. So as the alternative, just go into build directory and find your final program please. It's pretty simple.

## Note

You may face issue when compiling this program on Linux or macOS because I develop this project on Windows frequently. The compatibility with Linux will only be checked just before releasing. And I don't have any Apple device to check the compatibility with macOS. So, for Linux issue, please report it directly and I will try to fix. For macOS bug, PR is welcomed.



It can be compiled on Windows via sln file. You should set up `LibRef.props` when using sln file to build this project on Windows.  
You also can use CMake file to compile this project on Linux or anything else platform. However CMake may not be updated in time because I develop this project on Windows frequently.  
You may need use this command to configure CMake: `cmake .. -DSTB_IMAGE_PATH="/path/to/stb-image" -DCMAKE_BUILD_TYPE=Release`

