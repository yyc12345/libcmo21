# BMap Bindings

This directory contains bindings for C++ written BMap for interacting with Ballance map.
We now provide following bindings due to different reasons:

* pybmap: The Python bindings for BMap. [Ballance Blender Plugin](https://github.com/yyc12345/BallanceBlenderHelper) require this.
* BMapSharp: The C# bindings for BMap. This can be used in C#-available game engine (Unity, Godot, etc.) for implementing next generation of Ballance with legacy map format support.
* bmap-rs: The Rust bindings for BMap. ZZQ request it for his Ballance launcher.

## Package Principle

In theory, these BMap bindings can be distributed as the package in their corresponding package manager.
However, considering these bindings is specific for a minor territory, Ballance mapping,
and the complexity of building the essential artificate, dynamic library BMap written in C++,
I give up the idea of distributing them as package.
For all users who want to utilize these bindings, you should build them on their own,
according to the build manual provided in respective directory.

## Test

These BMap bindings all have test for testing whether they work correctly.
And these test program following the same pattern for fetching test parameters
indluding where to find the map and etc.
More preciously, you should set environment variables list following before running test programs.

* `BMAP_FILE_NAME`: The path to the map for loading.
* `BMAP_BALLANCE_DIR`:The path to the Ballance directory for finding textures
* `BMAP_ENCODINGS`: The name of encodings used by BMap for loading map. Multiple encodings is supported by separating them with commas, for example: `cp1252,gbk`.
