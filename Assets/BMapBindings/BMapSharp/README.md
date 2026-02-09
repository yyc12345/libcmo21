# BMapSharp

## Layout

This directory contains 2 project.
`BMapSharp` is the binding to BMap and `BMapSharpTest` is the test for `BMapSharp`.

> [!NOTE]
> `BMapSharpTest` project do not utilize any existing test framework.
> The test should be executed manually by compiling it and running it.

## Native BMap Library Location

The native BMap library should be placed together with managed `BMapSharp` dynamic library.

The native BMap library must be named as `BMap.dll` (in Windows), `BMap.so` (in Linux or BSD), `BMap.dylib` (in macOS), or `BMap.bin` (in any other platforms).

If you are building final distributed package file (I have not build them yet), you may need manually put native BMap library file into package file because I have not write this behavior in any place.
