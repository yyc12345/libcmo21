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

You should clone YYCCommonplace and switch to the latest release tag (or specified commit hash provided with libcmo21 release infos if you are building for specific libcmo21 version). When configuring YYCCommonplace, you should notice following infos:

* Please make sure that you have specified C++ 20 explicitly by passing `-DCMAKE_CXX_STANDARD=20` in command line. This is crucial because libcmo21 use C++ 20, and YYCCommonplace's ABI is incompatible between C++ 17 version and C++ 20 version.
* If you need `BMap` in final libcmo21 built artifacts, and you are in non-Windows system now, please specify position independent code flag by passing `-DCMAKE_POSITION_INDEPENDENT_CODE=True` in command line. GCC and Clang will reject linking if you don't flag it.

After configuring, you can normally build YYCCommonplace like a common CMake project.

Please note if your final program or dynamic library is provided for end user, please choose `RelWithDebInfo` build type (`Release` is not suggested because it will strip all debug infos and it is not good for bug reporter, which is embedded in program, to report crash). If final program is served for programmer debugging, please choose `Debug` build type.

### stb

You should clone stb repository first, then switch to a specific commit hash `5736b15f7ea0ffb08dd38af21067c314d6a3aae9`. In ideally scenario, people like to choose the latest commit. However, I hardly update this dependency so the latest commit is not work.

### zlib

If you are in Windows, you should download zlib source code and build it with given Visual Studio solution files. The final file we needed is `zlibwapi.dll` with `ZlibDllRelease` build type.

If you are running on non-Windows system. You usually do not need to do anything. Because zlib development environment may be configured by your package manager correctly.

## Compile

### Directory Hierarchy

First, create subdirectory `Bin/build` and `Bin/install`.

### Configuration

Then enter subdirectory `Bin/build` and use following command to configure CMake:

- Windows (MSVC): `cmake -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BMAP=ON -DNEMO_BUILD_DOC=OFF -DSTB_IMAGE_PATH=<path-to-stb> -DYYCC_PATH=<path-to-yycc-install> -DZLIB_HEADER_PATH=<path-to-zlib-hdr> -DZLIB_BINARY_PATH=<path-to-zlib-bin> ../..`
- non-Windows: `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DNEMO_BUILD_UNVIRT=ON -DNEMO_BUILD_BMAP=ON -DNEMO_BUILD_DOC=OFF -DSTB_IMAGE_PATH=<path-to-stb> -DYYCC_PATH=<path-to-yycc-install> ../..`

The arguments in command should be replaced by:

* `<path-to-stb>`: The root directory of your cloned stb repository.
* `<path-to-yycc-install>`: The directory to installed CMake package you chosen when building YYCCommonplace.
* `<path-to-zlib-hdr>` (Windows only): The root directory of downloaded zlib source code.
* `<path-to-zlib-bin>` (Windows only): The directory where you can find built `zlibwapi.dll`.

The switches in command can be switched as you wish:

* `NEMO_BUILD_UNVIRT`: Build `Unvirt`, a command line application debugging Virtools files.
* `NEMO_BUILD_BMAP`: Build `BMap`, a dynamic library specific used for loading Ballance map file.
* `NEMO_BUILD_DOC`: Build the document of libcmo21.

### Build

Execute following command to build libcmo21.

* Windows: `cmake --build . --config RelWithDebInfo`
* non-Windows: `cmake --build .`

### Build Type

Like YYCCommonplace, we suggest `RelWithDebInfo` for end user. If you want to build for programmer debugging, please replace all `RelWithDebInfo` to `Debug`. The build type between YYCCommonplace and libcmo21 should keep same to reduce any possibility about ABI incompatible issue.

## Install

Currently the CMake install script still has some bugs and can not work as expected. So as the alternative, just go into build directory and find your final program please. It's pretty simple.

## Note

You may face issue when compiling this program on Linux or macOS because I develop this project on Windows frequently. The compatibility with Linux will only be checked just before releasing. And I don't have any Apple device to check the compatibility with macOS. So, for Linux issue, please report it directly and I will try to fix. For macOS bug, PR is welcomed.
