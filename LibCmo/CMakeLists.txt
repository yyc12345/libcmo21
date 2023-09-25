cmake_minimum_required(VERSION 3.12)
project(LibCmo LANGUAGES CXX)

# find packages
find_package(ZLIB REQUIRED)
find_package(Iconv REQUIRED)

# manually check stb image path
if (NOT STB_IMAGE_PATH)
message(FATAL_ERROR "You must assign your stb_image library root path to STB_IMAGE_PATH when compiling this project.")
endif ()
if ((NOT EXISTS "${STB_IMAGE_PATH}/stb_image.h") OR (NOT EXISTS "${STB_IMAGE_PATH}/stb_image_resize.h") OR (NOT EXISTS "${STB_IMAGE_PATH}/stb_image_write.h"))
message(FATAL_ERROR "Invalid stb_image library path.")
endif ()

# set up file list
set(libcmo_headers ".")

set(libcmo_vt_src
	VTUtils.cpp
	VTEncoding.cpp
	VTImage.cpp
)
set(libcmo_ck2_src
	CK2/CKBitmapData.cpp
	CK2/CKContext.cpp
	CK2/CKFileOthers.cpp
	CK2/CKFileReader.cpp
	CK2/CKFileWriter.cpp
	CK2/CKGlobals.cpp
	CK2/CKStateChunkOthers.cpp
	CK2/CKStateChunkReader.cpp
	CK2/CKStateChunkWriter.cpp
)
set(libcmo_ck2_dh_src
	CK2/DataHandlers/CKBitmapHandler.cpp
)
set(libcmo_ck2_mgr_src
	CK2/MgrImpls/CKBaseManager.cpp
	CK2/MgrImpls/CKObjectManager.cpp
	CK2/MgrImpls/CKPathManager.cpp
)
set(libcmo_ck2_obj_src
	CK2/ObjImpls/CK3dEntity.cpp
	CK2/ObjImpls/CKBeObject.cpp
	CK2/ObjImpls/CKGroup.cpp
	CK2/ObjImpls/CKMaterial.cpp
	CK2/ObjImpls/CKMesh.cpp
	CK2/ObjImpls/CKObject.cpp
	CK2/ObjImpls/CKTexture.cpp
)
set(libcmo_vxmath_src
	VxMath/VxMemoryMappedFile.cpp
	VxMath/VxMath.cpp
)
set(libcmo_xcontainer_src
	XContainer/XTypes.cpp
)

# create static library
add_library(LibCmo
STATIC
	${libcmo_vt_src}
	${libcmo_ck2_src}
	${libcmo_ck2_dh_src}
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
	${STB_IMAGE_PATH}
	${ZLIB_INCLUDE_DIRS}
	${Iconv_INCLUDE_DIRS}
)

# set project standard
set_target_properties(LibCmo
PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED 20
	CXX_EXTENSION OFF
)
# add essential build macro and populate them
target_compile_definitions(LibCmo
PUBLIC
	$<$<CONFIG:Debug>:LIBCMO_BUILD_DEBUG>
	$<$<CONFIG:Release>:LIBCMO_BUILD_RELEASE>
	$<$<CONFIG:RelWithDebInfo>:LIBCMO_BUILD_RELEASE>
	$<$<CONFIG:MinSizeRel>:LIBCMO_BUILD_RELEASE>
)

