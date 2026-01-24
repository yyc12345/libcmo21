# LibCmo21 Redist

This folder is served for LibCmo21 distribution and this page will introduce how to distribute a LibCmo21.

In this article, I assume:
* This distribution is served for Windows user.
* All Linux will use this project by compiling it on themselves.
* You are using Visual Studio under Windows, not CMake.
* User will only need x64 architecture, not Win32 (x86).

## Common

1. Copy project `LICENSE` into folder.

## Unvirt

1. Compile project with `x64 | Release` profile.
1. Create folder `Unvirt` and enter it.
1. Copy generated `Unvirt.exe` and `Unvirt.pdb` into folder.
1. Copy zlib binary `zlibwapi.dll` into folder.

## BMap

1. Compile project with `x64 | Release` profile.
1. Create folder `BMap` and enter it.
1. Copy generated `BMap.dll` and `BMap.pdb` into folder.
1. Copy zlib binary `zlibwapi.dll` into folder.

## PyBMap

1. Compile project with `x64 | Release` profile.
1. Create folder `PyBMap` and enter it.
1. Copy all files ending with  `.py` and located in folder `BMapBindings/PyBMap/PyBMap` into folder.
1. Copy generated `BMap.dll` and `BMap.pdb` into folder.
1. Copy zlib binary `zlibwapi.dll` into folder.

## BMapSharp

This project is not ready for release.

## Ending

1. Pack all files and folders except `.gitignore` and `README.md` in this folder.
