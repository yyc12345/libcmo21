cmake_minimum_required(VERSION 3.12)
project(LibCmo LANGUAGES CXX)

# find packages
find_package(ZLIB REQUIRED)
find_package(Iconv REQUIRED)

# set standard
set(CMAKE_CXX_STANDARD 20)

set(libcmo_headers ".")

set(libcmo_vt_src
	VTUtils.cpp
	VTEncoding.cpp
)
set(libcmo_ck2_src
	CK2/CKContext.cpp
	CK2/CKFileOthers.cpp
	CK2/CKFileReader.cpp
	CK2/CKFileWriter.cpp
	CK2/CKGlobals.cpp
	CK2/CKStateChunk.cpp
)
set(libcmo_ck2_mgr_src
	CK2/MgrImpls/CKBaseManager.cpp
)
set(libcmo_ck2_obj_src
	CK2/ObjImpls/CKObject.cpp
)
set(libcmo_vxmath_src
	VxMath/VxMemoryMappedFile.cpp
)
set(libcmo_xcontainer_src
)

# create static library
add_library(LibCmo
STATIC
	${libcmo_vt_src}
	${libcmo_ck2_src}
	${libcmo_ck2_mgr_src}
	${libcmo_ck2_obj_src}
	${libcmo_vxmath_src}
	${libcmo_xcontainer_src}
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

