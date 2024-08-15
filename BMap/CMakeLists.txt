# Create shared library
add_library(BMap SHARED "")
# Setup sources
target_sources(BMap
PRIVATE
	BMap.cpp
	BMExports.cpp
)
# Setup headers
target_sources(BMap
PRIVATE
FILE_SET HEADERS
FILES
	BMap.hpp
	BMExports.hpp
)
# Setup header infomation
target_include_directories(BMap
PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}"
	YYCC::YYCCommonplace
	LibCmo
)
# Setup linked library infomation
target_link_libraries(BMap
PRIVATE
	YYCC::YYCCommonplace
	LibCmo
)
# Setup C++ standard
set_target_properties(BMap
PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED 20
    CXX_EXTENSION OFF
)
# Order Unicode charset for private using
target_compile_definitions(BMap
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:UNICODE>
    $<$<CXX_COMPILER_ID:MSVC>:_UNICODE>
)
# Order build as UTF-8 in MSVC
target_compile_options(BMap
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

# Install BMap only on Release mode
install(TARGETS BMap
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${YYCC_INSTALL_BIN_PATH}
)


# cmake_minimum_required(VERSION 3.12)
# project(BMap LANGUAGES CXX)

# # add libcmo if not existed
# if (NOT TARGET LibCmo)
# 	add_subdirectory("../LibCmo" "LibCmo.out")
# endif ()
# # add ironpad if not existed
# if (NOT TARGET IronPad)
# 	add_subdirectory("../IronPad" "IronPad.out")
# endif ()

# # hide all function in default
# # fix imported library PIC issue (i don't know why. just make it works)
# set_target_properties(LibCmo
# PROPERTIES
#     CXX_VISIBILITY_PRESET hidden
#     POSITION_INDEPENDENT_CODE ON
# )
# set_target_properties(IronPad
# PROPERTIES
#     CXX_VISIBILITY_PRESET hidden
#     POSITION_INDEPENDENT_CODE ON
# )

# # setup sources
# set(bmap_headers ".")
# set(bmap_sources
# 	BMap.cpp
#     BMExports.cpp
# )

# # generate shared library
# add_library(BMap
# SHARED
#     ${bmap_sources}
# )
# target_link_libraries(BMap
# PRIVATE
# 	LibCmo
# 	IronPad
# )
# target_include_directories(BMap
# PRIVATE
# 	${bmap_headers}
# )

# # set project standard
# set_target_properties(BMap
# PROPERTIES
# 	CXX_STANDARD 20
# 	CXX_STANDARD_REQUIRED 20
# 	CXX_EXTENSION OFF
# )
# # set default visibility to hidden
# set_target_properties(BMap
# PROPERTIES
#     CXX_VISIBILITY_PRESET hidden
# )
# # add export used macro flag
# target_compile_definitions(BMap
# PUBLIC
# 	LIBCMO_EXPORTING
# )
