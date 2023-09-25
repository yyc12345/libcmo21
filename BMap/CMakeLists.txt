cmake_minimum_required(VERSION 3.12)
project(BMap LANGUAGES CXX)

# add libcmo if not existed
if (NOT TARGET LibCmo)
	add_subdirectory("../LibCmo" "LibCmo.out")
endif ()
# add ironpad if not existed
if (NOT TARGET IronPad)
	add_subdirectory("../IronPad" "IronPad.out")
endif ()

# hide all function in default
# fix imported library PIC issue (i don't know why. just make it works)
set_target_properties(LibCmo
PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON
)
set_target_properties(IronPad
PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    POSITION_INDEPENDENT_CODE ON
)

# setup sources
set(bmap_headers ".")
set(bmap_sources
	BMap.cpp
    BMExports.cpp
)

# generate shared library
add_library(BMap
SHARED
    ${bmap_sources}
)
target_link_libraries(BMap
PRIVATE
	LibCmo
	IronPad
)
target_include_directories(BMap
PRIVATE
	${bmap_headers}
)

# set project standard
set_target_properties(BMap
PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED 20
	CXX_EXTENSION OFF
)
# set default visibility to hidden
set_target_properties(BMap
PROPERTIES
    CXX_VISIBILITY_PRESET hidden
)
# add export used macro flag
target_compile_definitions(BMap
PUBLIC
	LIBCMO_EXPORTING
)

