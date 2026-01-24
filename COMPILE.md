# Compile Manual

## Choose Version

We suggest that you only use stable version (tagged commit).
The latest commit always present current works.
It means that it is not stable and work in progress.

## Requirements

* CMake 3.23 at least.
* The common compiler supporting C++ 23 (GCC / Clang / MSVC).
* Littile-endian architecture system.
* zlib.
* [stb](https://github.com/nothings/stb).
* [YYCCommonplace](https://github.com/yyc12345/YYCCommonplace).
* Doxygen (Required if you build documentation).

## Preparations

### YYCCommonplace

Following these steps to prepare YYCCommonplace:

1. Clone YYCCommonplace first.
1. Switch to the latest **release** tag, or **specified commit hash** provided with libcmo21 release infos if you are building for specific libcmo21 version.
1. Following compile manual provided by YYCCommonplace to configure, compile and install it.

> [!IMPORTANT]  
> If you need `BMap` component in final libcmo21 built artifacts, and you are in non-Windows system now, please specify position independent code flag by passing `-DCMAKE_POSITION_INDEPENDENT_CODE=True` in command line when configuring YYCCommonplace. Otherwise GCC and Clang will reject linking.

### stb

Following these steps to prepare stb:

1. Clone stb repository first.
1. Switch to a **specific commit hash** `2e2bef463a5b53ddf8bb788e25da6b8506314c08`. I do not frequently update this dependency so using the latest commit is inviable.

> [!NOTE]  
> std is a header-only C project. So we don't need compile it. libcmo21 will use homemade CMake script to find it.

### zlib

If you are in Windows, you should download zlib source code and build it with given Visual Studio solution files. The final file we needed is `zlibwapi.dll` with `ZlibDllRelease` build type.

If you are running on non-Windows system. You usually do not need to do anything. Because zlib development environment may be configured by your package manager correctly.

If you are in Windows, or in Linux but want to use specific zlib version due to various reasons, following these steps to prepare zlib:

1. Download zlib source code from its official.
1. Extract it into a directory.
1. Enter this directory and create 2 subdirectory `build` and `install` for CMake build and install respectively.
1. Enter `build` directory and configure CMake with extra `-DCMAKE_CXX_STANDARD=23 -DZLIB_BUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=<path-to-install>` parameters. And `<path-to-install>` is the absolute path to your created `install` directory in previous step (Idk why `--prefix` argument is not works for zlib).
1. Use CMake to build zlib
1. Use CMake to install zlib into previous we created `install` directory.

> [!NOTE]  
> We use CMake, rather than any other to compile zlib because zlib provide it, and we also use CMake as our build system, so that we do not need to write any extra files for adaption.

> [!CAUTION]  
> Windows developer should highly notice that please do NOT use Visual Studio file located in `contrib/vstudio` directory to produce binary zlib on Windows. That project will produce `zlibwapi.dll` which is not our expected `zlib.dll`. Please use Visual Studio embedded CMake to configure zlib and compile it directly.

### Doxygen

Doxygen is required only if you need to build documentation.
If you don't need this please skip this chapter.

We use Doxygen 1.9.7.
It would be okey use other versions but I have not test on them.

YYCCommonplace use Doxygen as its documentation system.
So before compiling, you must make sure `doxygen` are presented in your environment.

## Build and Install

Using CMake is the only viable way to build and install this repository.

### Configurable Variables

First, there is a list listing all variables you may configure during compiling.

* `NEMO_BUILD_UNVIRT`: Set it to `ON` to build `Unvirt`. `ON` in default.
This is an interactive tool for loading, saving Virtools file.
It is extremely useful for debugging this project.
* `NEMO_BUILD_BMAP`: Set it to `ON` to build `BMap`. `OFF` in default.
It is a dynamic library specific used for loading Ballance map file.
If you are coming from my another project [BallanceBlenderPlugin](https://github.com/yyc12345/BallanceBlenderHelper), this is what you need.
* `NEMO_BUILD_DOC`: Set it to `ON` to build documentation. `OFF` in default.
It may be useful for the developer who firstly use this project in their own projects.
Please note that generated documentation is different in different platforms.
* `YYCCommonplace_ROOT`: Set to the install path of YYCCommonplace.
* `stb_ROOT`: Set to the root directory of stb.
* `ZLIB_ROOT`: Set to the install path of zlib.
If you are using zlib which is not build by your own, you usually do not need specify this variable.

### Configure CMake

When configure CMake, you may use different options on different platforms.
Following list may help you.

- On Windows:
    * `-A Win32` or `-A x64` to specify architecture.
- On Linux or other POSIX systems:
    * `-DCMAKE_BUILD_TYPE=Debug` or `-DCMAKE_BUILD_TYPE=Release` to specify build type.

Additionally, you can attach any variables introduced above with `-D` option during CMake configurations.

> [!NOTE]  
> Position independent code flag is automatically added if you enable `BMap` so you don't need manually specify it. You just need to make sure that all dependencies enable this.

### Build with CMake

After configuration, you can use `cmake --build .` to build project,
with additional options on different platforms.
Following list may help you.

- On Windows:
    * `--config Debug` or `--config Release` to specify build type.
- On Linux or other POSIX systems:
    * None

### Install with CMake

After building, you can use `cmake --install . --prefix <path-to-prefix>`
to install project into given path, with additional options on different platforms.
Following list may help you.

- On Windows:
    * `--config Debug` or `--config Release` to specify build type.
- On Linux or other POSIX systems:
    * None

## Compatibility Notes

You may face issue when compiling this program on Linux or macOS because I develop this project on Windows frequently.
The compatibility with Linux and macOS will only be checked just before releasing. 
And, I don't have any Apple devices, so the compatibility with macOS is checked by GitHub Action.
