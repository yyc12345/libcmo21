# libcmo21

The library for CMO (also accept NMO, VMO and NMS) file RW (read and write). It is also a Minimalist Virtools Environment.

> Write with one library. Load and save Virtools file everywhere.

## Status

This project is now suspended and may be still suspended eternally due to following reasons:

* All ultimate goals are achieved.
* The complexity of this project and legacy bugs.
* The lost interest and the lack of time of mine.

In following life time of this project, only easy-to-be-resolved critical bugs will be fixed.

## Introduction

The aim of this project is creating a universal library which can RW CMO files or any other Virtools files without any Virtools dependencies.

This project will not link any original Virtools dynamic library. So this project can be ported to any platform if the compiler and runtime libraries support.

This project only involving specific Virtools version, 2.1. Other too higher Virtools versions are not considered by this project.

This project is barely based on the reverse work of doyaGu who decompile `CK2.dll`, `VxMath.dll` and `CK2_3D.dll`. The program [unvirt](https://aluigi.altervista.org/papers.htm#unvirt) created by Luigi Auriemma, which is licensed by GPL-v2, also help my work.

**The difference between this project and other Virtools libraries (e.g. [doyaGu/Ballanced](https://github.com/doyaGu/Ballanced)), is that this project is not focusing on re-creating the whole Virtools engine. This project only focus on the Virtools files RW, and it only just implement a minimalist Virtools environment for achieving this.**

## Goals

The ultimate goals of this project are:

* Create a library which can read Virtools file and do not limited on x86 platform by original Virtools implement.
* Create a universal dynamic library for Ballance Map file loading.
  - Create a Python binding for the loader. And allow user can export Ballance Map from Blender on Linux platform natively.
  - Create a C# binding for the loader to enable that I can load Ballance Map in Godot on Linux / Android platform natively.

These goals are now all achieved.

## Project Layout

* LibCmo: Core library. It is a static library because I don't want to process export table things. Link to your program freely.
* Unvirt: Interactive Virtools file viewer. Can provide more detail than Luigi Auriemma's unvirt.
* Ballance: Ballance related stuffs which utilize LibCmo.
  - BMap: A dynamic library which is specific for Ballance Map loading and saving.
  - BMapInspector: A inspector for checking Ballance Map.

## Contributions

However, not all contribution will be accepted.

For the limitation of contributor, please see [Contribution Guideline](./CONTRIBUTING.md).

For the content of contribution, just like I said, we create **Minimalist** Virtools Environment. The basic RW functions is enough. We do not accept complex function implementations. There are 3 lists which indicate our accept guideline.

### Wanted Features

These features will be accepted as soon as possible.

* The bug fix of any existing code.

### Not Urgent Features

These features are in plan, but not urge to merge.

* The `CK_ID` remap system of Reader.
* CK3dEntity hierarchy system.
* Other CK classes implementations.
* Non-Virtools 2.1 implementations.

### Don't Care Features

These features explicitly will not be merged.

* Run Virtools file.
* Plugin system.

## Compile

See [Compile Manual](./COMPILE.md)
