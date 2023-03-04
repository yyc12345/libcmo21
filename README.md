# libcmo21

The Library for CMO File Read/Write. Also the Minimalist Virtools Environment.

## Introduction

The aim of this project is creating a universal library which can read / write CMO files or any other Virtools files without any Virtools dependencies.  
This project will not link any original Virtools dynamic library. So this project can be ported to any platform if the compiler supports.  
This project only involving specific Virtools version, 2.1. Other Virtools versions are not considered by this project.  
The difference between this project and doyaGu's Virtools libraries, is that we are not focusing on re-creating the whole Virtools engine. We only focus on the Virtools files RW, and we only just implement a minimalist Virtools environment for achieving this.  
This project is based on reverse work of CK2.dll, VxMath.dll and CK2_3D.dll. The program [unvirt](https://aluigi.altervista.org/papers.htm#unvirt) created by Luigi Auriemma, which is licensed by GPL-v2, also help my work.  

## Goals

The ultimate goals of this project are:

* Create a library which can read Virtools file and do not limited on x86 platform by original Virtools implement.
* Create a user-firendly Python binding. And allow user can export Ballance Map from Blender on Linux platform natively.
* Create a Godot Extension which can load Ballance Map natively.

There is still a long way need to go. But I will try my best.

## Compile

This project require:

* The compiler supporting C++20
* Littile-endian architecture system.
* zlib
* iconv (non-Windows system required)

It can be compiled on Windows via sln file, or on Linux platform via CMake file.
