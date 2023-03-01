cmake_minimum_required(VERSION 3.12)
project(LibCmo LANGUAGES CXX)

# find packages
find_package(ZLIB REQUIRED)
find_package(Iconv REQUIRED)

# set standard
set(CMAKE_CXX_STANDARD 20)

set(libcmo_headers ".")
set(libcmo_sources
	CKGlobals.cpp
	CKFile.cpp
	CKFileReader.cpp
	CKFileWriter.cpp
	CKMinContext.cpp
	CKStateChunk.cpp
	VxMemoryMappedFile.cpp
	VTEncoding.cpp
	VTUtils.cpp
)
set(libcmo_managers_sources
	CKManagerImplements/CKBaseManager.cpp
)
set(libcmo_objects_sources
	CKObjectImplements/CKObject.cpp
)

# create static library
add_library(LibCmo
STATIC
	${libcmo_sources}
	${libcmo_managers_sources}
	${libcmo_objects_sources}
)
target_link_libraries(LibCmo
PRIVATE
	${ZLIB_LIBRARIES}
	${Iconv_LIBRARIES}
)
target_include_directories(LibCmo
PUBLIC
	${libcmo_headers}
PRIVATE
	${ZLIB_INCLUDE_DIRS}
	${Iconv_INCLUDE_DIRS}
)

