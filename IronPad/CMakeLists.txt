cmake_minimum_required(VERSION 3.12)
project(IronPad LANGUAGES CXX)

# add libcmo if not existed
if (NOT TARGET LibCmo)
	add_subdirectory("../LibCmo" "LibCmo.out")
endif ()

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

# set project standard
set_target_properties(IronPad
PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED 20
	CXX_EXTENSION OFF
)
