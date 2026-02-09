# pybmap

## Layout

This project follow `src` and `test` layout and is managed by Astral UV.
The source code of pybmap is located inside `src`.
And the files located in `test` is used for testing.

> [!NOTE]
> The files located in `test` are not prepared for `pytest` framework.
> The test should be executed manually by `uv run test/main.py`.

## Native BMap Library Location

The native BMap library file should be placed with `bmap.py` file.

The native BMap library must be named as `BMap.dll` (in Windows), `BMap.so` (in Linux or BSD), `BMap.dylib` (in macOS), or `BMap.bin` (in any other platforms).

If you are building final distributed package file, such as `.whl` file, you may need manually put native BMap library file into package file because I have not write this behavior in any script.
