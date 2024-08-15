# Create executable
add_executable(Unvirt "")
# Setup sources
target_sources(Unvirt
PRIVATE
	AccessibleValue.cpp
	CmdHelper.cpp
	StringHelper.cpp
	StructFormatter.cpp
	TerminalHelper.cpp
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
	StringHelper.hpp
	StructFormatter.hpp
	TerminalHelper.hpp
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


# cmake_minimum_required(VERSION 3.12)
# project(Unvirt LANGUAGES CXX)

# # add libcmo if not existed
# if (NOT TARGET LibCmo)
# 	add_subdirectory("../LibCmo" "LibCmo.out")
# endif ()
# # add ironpad if not existed
# if (NOT TARGET IronPad)
# 	add_subdirectory("../IronPad" "IronPad.out")
# endif ()

# # setup sources
# set(unvirt_headers ".")
# set(unvirt_sources
# 	AccessibleValue.cpp
# 	CmdHelper.cpp
# 	StringHelper.cpp
# 	StructFormatter.cpp
# 	TerminalHelper.cpp
# 	UnvirtContext.cpp
# 	Unvirt.cpp
# )

# # generate program
# add_executable(Unvirt ${unvirt_sources})
# target_link_libraries(Unvirt
# PRIVATE
# 	LibCmo
# 	IronPad
# )
# target_include_directories(Unvirt
# PRIVATE
# 	${unvirt_headers}
# )

# # set project standard
# set_target_properties(Unvirt
# PROPERTIES
# 	CXX_STANDARD 20
# 	CXX_STANDARD_REQUIRED 20
# 	CXX_EXTENSION OFF
# )
