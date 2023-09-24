cmake_minimum_required(VERSION 3.12)
project(IronPad LANGUAGES CXX)

# set standard
set(CMAKE_CXX_STANDARD 20)

# add libcmo
add_subdirectory("../LibCmo" "LibCmo.out")

# setup sources
set(ironpad_headers ".")
set(ironpad_src IronPad.cpp)

# create static library
add_library(IronPad
STATIC
	${ironpad_src}
)
target_link_libraries(IronPad
PRIVATE
	LibCmo
)
target_include_directories(IronPad
PUBLIC
	${ironpad_headers}
)
