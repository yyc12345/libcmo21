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
# Enable export macro
target_compile_definitions(BMap
PRIVATE
	BMAP_EXPORTING
)

# Install BMap
install(TARGETS BMap
    RUNTIME DESTINATION ${NEMO_INSTALL_BIN_PATH}
)
