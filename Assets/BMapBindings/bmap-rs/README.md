# bmap-rs

## Layout

This project follows standard Rust project layout.
Source code are located in `src` directory and test code are in `tests` directory.

Please note that the raw FFI functions and the wrapper of it are placed in the single project,
not like other Rust project that raw bindings and wrapper are put into 2 different project, 
one of them is ended with `-sys` and another one is not.
This is a considerable decision due to the following reasons:

- I want this project has same pattern with other bindings. In other bindings, FFI binding code and wrapper are put together.
- They (FFI binding and wrapper) can not be easily splitted due to logic reasons. Putting them together can avoid some extra work.

## Native BMap Library Location

This project uses native way (native compiler and linker) to link the BMap library.
This is different with other bindings.

> [!IMPORTANT]
> We highly suggest that you make sure that the toolchain building your BMap is same as your configured Rust toolchain.
> Although BMap is a dynamic library and its function is exposed as C function, the link issue may still occurs due to the different toolchain.

According to this, you should set `LibCmo_ROOT` environment variable pointing to the CMake install directory of LibCmo with built BMap before configuring this Rust project.
This project will find it in `build.rs` script and tell Rust compiler how to link it.

> [!IMPORTANT]
> For Linux and macOS user, you may need manually to rename the final built BMap artifact.
> Because in these platforms, CMake produced BMap dynamic library may have `lib` prefix in its name.
> This can not be recognized by our build script.
> 
> You should rename it to `BMap.so` or `BMap.dylib` depending on your platform.
> You also may need rename some contents of other files involving this rename change.

Also due to this, when distributing your Rust project, please do not forget copy the built BMap library with your Rust artifacts.
