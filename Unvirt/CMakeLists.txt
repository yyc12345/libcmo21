cmake_minimum_required(VERSION 3.12)
project(Unvirt LANGUAGES CXX)

# set languages
set(CMAKE_CXX_STANDARD 20)

# add libcmo
add_subdirectory("../LibCmo" "LibCmo.out")
# add ironpad
#add_subdirectory("../IronPad" "IronPad.out")

# setup sources
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

