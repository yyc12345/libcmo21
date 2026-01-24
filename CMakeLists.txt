# Minimum required CMake version
cmake_minimum_required(VERSION 3.23)
# Project definition
project(NeMo
    VERSION 0.4.0
    LANGUAGES CXX
)

# Provide options
option(NEMO_BUILD_UNVIRT "Build Unvirt, the console interface operator of LibCmo." ON)
option(NEMO_BUILD_BMAP "Build BMap, the example use of LibCmo which can read and write Ballance game map." OFF)
option(NEMO_BUILD_DOC "Build document of LibCmo and all related stuff." OFF)

# Set C++ standards
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Setup install path from CMake provided install path for convenient use.
include(GNUInstallDirs)
set(NEMO_INSTALL_INCLUDE_PATH ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH
"Public header install path relative to CMAKE_INSTALL_PREFIX unless set to an absolute path.")
set(NEMO_INSTALL_LIB_PATH ${CMAKE_INSTALL_LIBDIR} CACHE PATH
"Library install path relative to CMAKE_INSTALL_PREFIX unless set to an absolute path.")
set(NEMO_INSTALL_BIN_PATH ${CMAKE_INSTALL_BINDIR} CACHE PATH
"Binary install path relative to CMAKE_INSTALL_PREFIX unless set to an absolute path.")
set(NEMO_INSTALL_DOC_PATH ${CMAKE_INSTALL_DOCDIR} CACHE PATH
"Non-arch doc install path relative to CMAKE_INSTALL_PREFIX unless set to an absolute path.")

# Add our CMake in module found path
set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/CMake"
)
# Find required packages
find_package(ZLIB REQUIRED)
find_package(YYCCommonplace REQUIRED)
find_package(STB REQUIRED)

# If we are not in Windows environment, and we need to build shared library BMap,
# we should enable PIC (position independent code), otherwise build process will fail.
# Also we should let all symbols in final dll be hidden (not exported) in default.
# Because we only want export functions we ordered.
if ((NOT WIN32) AND NEMO_BUILD_BMAP)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_POSITION_INDEPENDENT_CODE True)
endif ()

# Include build targets by options
add_subdirectory(LibCmo)
if (NEMO_BUILD_UNVIRT)
    add_subdirectory(Unvirt)
endif ()
if (NEMO_BUILD_BMAP)
    add_subdirectory(BMap)
endif ()
if (NEMO_BUILD_DOC)
    add_subdirectory(Documents)
endif ()

# Install target and package
# Install target
install(EXPORT LibCmoTargets
    FILE LibCmoTargets.cmake
    NAMESPACE NeMo::
    DESTINATION ${NEMO_INSTALL_LIB_PATH}/cmake/LibCmo
)
# Package configuration file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    LibCmoConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY SameMinorVersion
)
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/CMake/LibCmoConfig.cmake.in 
    "${CMAKE_CURRENT_BINARY_DIR}/LibCmoConfig.cmake"
    INSTALL_DESTINATION ${NEMO_INSTALL_LIB_PATH}/cmake/LibCmo
)
# Copy package files to install destination
install(
FILES
    "${CMAKE_CURRENT_BINARY_DIR}/LibCmoConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/LibCmoConfigVersion.cmake"
DESTINATION
    ${NEMO_INSTALL_LIB_PATH}/cmake/LibCmo
)
