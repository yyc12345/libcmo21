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
	Ruleset/Shared/Utility.cpp
	Ruleset/Shared/Name.cpp
	Ruleset/Shared/Sector.cpp
	Ruleset/Shared/DupCmp.cpp
	Ruleset/GpRules.cpp
	Ruleset/ChirsRules.cpp
	Ruleset/YYCRules.cpp
	Ruleset/ZZQRules.cpp
	Ruleset/BBugRules.cpp
	Ruleset/SOneRules.cpp
	Ruleset/SSBRules.cpp
	Ruleset/LXRules.cpp
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
	Ruleset/Shared/Utility.hpp
	Ruleset/Shared/Name.hpp
	Ruleset/Shared/Sector.hpp
	Ruleset/Shared/DupCmp.hpp
	Ruleset/GpRules.hpp
	Ruleset/ChirsRules.hpp
	Ruleset/YYCRules.hpp
	Ruleset/ZZQRules.hpp
	Ruleset/BBugRules.hpp
	Ruleset/SOneRules.hpp
	Ruleset/SSBRules.hpp
	Ruleset/LXRules.hpp
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
