if (WIN32)
    # In Windows, we use custom way to import ZLib.
    # Before using this CMake file in Windows, you should do following steps first.
    # 1. Get ZLib repository: https://github.com/madler/zlib
    # 2. Navigate to `contrib/vstudio` and choose a proper Visual Studio project according to your environment.
    # 3. Open project file and build. Then you will get the built binary.
    # 4. The directory binary located is the argument you should passed to ZLIB_BINARY_PATH, for example: `contrib/vstudio/vc14/x64/ZlibDllRelease`

    # Check ZLib path variable
    if (NOT DEFINED ZLIB_HEADER_PATH)
        message(FATAL_ERROR "You must set ZLIB_HEADER_PATH to the root directory of ZLib repository.")
    endif()
    if (NOT DEFINED ZLIB_BINARY_PATH)
        message(FATAL_ERROR "You must set ZLIB_BINARY_PATH to the directory where include binary built by contributed Visual Studio project.")
    endif()
    # Add imported library
    add_library(ZLIB INTERFACE IMPORTED)
    # Add alias for it to let it has the same behavior with CMake imported ZLib.
    add_library(ZLIB::ZLIB ALIAS ZLIB)
    # Setup header files
    set_target_properties(ZLIB PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
        "${ZLIB_HEADER_PATH}"
    )
    # Setup lib files
    set_target_properties(ZLIB PROPERTIES
    INTERFACE_LINK_LIBRARIES
        "${ZLIB_BINARY_PATH}/zlibwapi.lib"
    )
else ()
    # In non-Windows, we simply import ZLib from CMake preset.
    # It will produce ZLIB::ZLIB target for including and linking.
    find_package(ZLIB REQUIRED)
endif ()
