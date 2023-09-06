# libcmo21

The Library for CMO File Read/Write. Also the Minimalist Virtools Environment.

## Status

This is a long time project.  
This project welcome everyone's contribution, except the employee of Dassault, which created Virtools.  

## Introduction

The aim of this project is creating a universal library which can read / write CMO files or any other Virtools files without any Virtools dependencies.  
This project will not link any original Virtools dynamic library. So this project can be ported to any platform if the compiler supports.  
This project only involving specific Virtools version, 2.1. Other Virtools versions are not considered by this project.  
This project is based on reverse work of CK2.dll, VxMath.dll and CK2_3D.dll. The program [unvirt](https://aluigi.altervista.org/papers.htm#unvirt) created by Luigi Auriemma, which is licensed by GPL-v2, also help my work.

**The difference between this project and other Virtools libraries, is that we are not focusing on re-creating the whole Virtools engine. We only focus on the Virtools files RW, and we only just implement a minimalist Virtools environment for achieving this.**

## Goals

The ultimate goals of this project are:

* Create a library which can read Virtools file and do not limited on x86 platform by original Virtools implement.
* Create a user-firendly Python binding. And allow user can export Ballance Map from Blender on Linux platform natively.
* Create a Godot Extension which can load Ballance Map natively.

There is still a long way to go. But I will try my best.

## Contributions

However, not all contribution will be accepted. Just like I said, we create **Minimalist** Virtools Environment. The basic RW functions is enough. We do not accept complex function implementations.  
There are 3 lists which indicate our accept guideline.

### Wanted Features

These features will be accepted as soon as possible.

* The bug fix of Virtools file reader.
* Class layout, `Load()` functions of following `CKObject` based classes.
  - `CKBeObject`
  - `CKGroup`
  - `CKMaterial`
  - `CKTexture`
  - `CKMesh`
  - `CKRenderObject`
  - `CK3dEntity`
  - `CK3dObject`
* Class layout, and `LoadData()` functions of following `CKBaseManager` based classes.
  - `CKAttributeManager`
  - `CKBehaviorManager`

### Not Urgent Features

These features are in plan, but not urge to merge.

* The `CK_ID` remap system of Reader & Writer.
* Any Save functions.
* The save steps of Virtools file.
* Other CK classes implementations.
* Non-Virtools 2.1 implementations.

### Don't Care Features

These features explicitly will not be merged.

* Run Virtools file.
* Plugin system.

## Compile

This project require:

* The compiler supporting C++20
* Littile-endian architecture system.
* zlib
* FFmpeg (In Windows, download dev package in [there](https://github.com/yt-dlp/FFmpeg-Builds).)
* iconv (non-Windows system required)

It can be compiled on Windows via sln file, or on Linux platform via CMake file.  
You should set up `LibRef.props` when using sln file.  
However CMake may not be updated in time because I develop this project on Windows frequently.
