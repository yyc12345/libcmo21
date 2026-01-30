# Create executable
add_executable(BMapInspector "")
# Setup sources
target_sources(BMapInspector
PRIVATE
	BMapInspector.cpp
	Utils.cpp
)
# Setup headers
target_sources(BMapInspector
PRIVATE
FILE_SET HEADERS
FILES
	Utils.hpp
	Rule.hpp
)
# Setup header infomation
target_include_directories(BMapInspector
PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}"
)
# Setup linked library infomation
target_link_libraries(BMapInspector
PRIVATE
	YYCC::YYCCommonplace
	LibCmo
)

# Install BMapInspector
install(TARGETS BMapInspector
    RUNTIME DESTINATION ${YYCC_INSTALL_BIN_PATH}
)
