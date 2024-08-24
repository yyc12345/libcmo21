# Create executable
add_executable(Unvirt "")
# Setup sources
target_sources(Unvirt
PRIVATE
	AccessibleValue.cpp
	CmdHelper.cpp
	StructFormatter.cpp
	UnvirtContext.cpp
	Unvirt.cpp
)
# Setup headers
target_sources(Unvirt
PRIVATE
FILE_SET HEADERS
FILES
	AccessibleValue.hpp
	CmdHelper.hpp
	StructFormatter.hpp
	UnvirtContext.hpp
)
# Setup header infomation
target_include_directories(Unvirt
PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}"
	YYCC::YYCCommonplace
	LibCmo
)
# Setup linked library infomation
target_link_libraries(Unvirt
PRIVATE
	YYCC::YYCCommonplace
	LibCmo
)
# Setup C++ standard
set_target_properties(Unvirt
PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED 20
    CXX_EXTENSION OFF
)
# Order Unicode charset for private using
target_compile_definitions(Unvirt
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:UNICODE>
    $<$<CXX_COMPILER_ID:MSVC>:_UNICODE>
)
# Order build as UTF-8 in MSVC
target_compile_options(Unvirt
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

# Install Unvirt only on Release mode
install(TARGETS Unvirt
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${YYCC_INSTALL_BIN_PATH}
)
