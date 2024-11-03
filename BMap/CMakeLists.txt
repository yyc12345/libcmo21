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
# Setup project macros
target_compile_definitions(BMap
# Enable export macro
PRIVATE
	BMAP_EXPORTING
# Order Unicode charset for private using
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
    CONFIGURATIONS Release RelWithDebInfo MinSizeRel
    RUNTIME DESTINATION ${YYCC_INSTALL_BIN_PATH}
)
