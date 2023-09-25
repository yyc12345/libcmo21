cmake_minimum_required(VERSION 3.12)
project(Unvirt LANGUAGES CXX)

# add libcmo if not existed
if (NOT TARGET LibCmo)
	add_subdirectory("../LibCmo" "LibCmo.out")
endif ()
# add ironpad if not existed
if (NOT TARGET IronPad)
	add_subdirectory("../IronPad" "IronPad.out")
endif ()

# setup sources
set(unvirt_headers ".")
set(unvirt_sources
	AccessibleValue.cpp
	CmdHelper.cpp
	StringHelper.cpp
	StructFormatter.cpp
	TerminalHelper.cpp
	UnvirtContext.cpp
	Unvirt.cpp
)

# generate program
add_executable(Unvirt ${unvirt_sources})
target_link_libraries(Unvirt
PRIVATE
	LibCmo
	IronPad
)
target_include_directories(Unvirt
PRIVATE
	${unvirt_headers}
)

# set project standard
set_target_properties(Unvirt
PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED 20
	CXX_EXTENSION OFF
)


