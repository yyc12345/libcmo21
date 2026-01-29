# Create executable
add_executable(Unvirt "")
# Setup sources
target_sources(Unvirt
PRIVATE
	Docstring.cpp
	StructFmt.cpp
	CmdHelper.cpp
	UnvirtContext.cpp
	Unvirt.cpp
)
# Setup headers
target_sources(Unvirt
PRIVATE
FILE_SET HEADERS
FILES
	Docstring.hpp
	StructFmt.hpp
	CmdHelper.hpp
	UnvirtContext.hpp
)
# Setup header infomation
target_include_directories(Unvirt
PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}"
)
# Setup linked library infomation
target_link_libraries(Unvirt
PRIVATE
	YYCC::YYCCommonplace
	LibCmo
)

# Install Unvirt
install(TARGETS Unvirt
    RUNTIME DESTINATION ${YYCC_INSTALL_BIN_PATH}
)
