cmake_minimum_required(VERSION 3.12)
project(Unvirt LANGUAGES CXX)

# add libcmo
add_subdirectory("../LibCmo" "LibCmo.out")

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

# set languages
set(CMAKE_CXX_STANDARD 20)

# generate program
add_executable(Unvirt ${unvirt_sources})
target_link_libraries(Unvirt
PRIVATE
	LibCmo
)

