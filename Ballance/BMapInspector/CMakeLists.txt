# Create executable
add_executable(BMapInspector "")
# Setup sources
target_sources(BMapInspector
PRIVATE
	# Kernel
	BMapInspector.cpp
	Utils.cpp
	Reporter.cpp
	Cli.cpp
	Map.cpp
	Rule.cpp
	# Rules
	Rule/GpRules.cpp
	Rule/ChirsRules.cpp
	Rule/YYCRules.cpp
	Rule/ZZQRules.cpp
	Rule/BBugRules.cpp
)
# Setup headers
target_sources(BMapInspector
PRIVATE
FILE_SET HEADERS
FILES
	# Kernel
	Utils.hpp
	Reporter.hpp
	Cli.hpp
	Map.hpp
	Rule.hpp
	# Rules
	Rule/GpRules.hpp
	Rule/ChirsRules.hpp
	Rule/YYCRules.hpp
	Rule/ZZQRules.hpp
	Rule/BBugRules.hpp
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
