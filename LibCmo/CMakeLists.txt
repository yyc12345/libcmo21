# Create static library
add_library(LibCmo STATIC "")
# Setup static library sources
target_sources(LibCmo
PRIVATE
	# Assistant source files
	VTEncoding.cpp
	VTImage.cpp
	# CK2
	CK2/CKBitmapData.cpp
	CK2/CKContext.cpp
	CK2/CKFileOthers.cpp
	CK2/CKFileReader.cpp
	CK2/CKFileWriter.cpp
	CK2/CKGlobals.cpp
	CK2/CKStateChunkOthers.cpp
	CK2/CKStateChunkReader.cpp
	CK2/CKStateChunkWriter.cpp
	# CK2 Data Handler
	CK2/DataHandlers/CKBitmapHandler.cpp
	# CK2 Manager
	CK2/MgrImpls/CKBaseManager.cpp
	CK2/MgrImpls/CKObjectManager.cpp
	CK2/MgrImpls/CKPathManager.cpp
	# CK2 Object
	CK2/ObjImpls/CK3dEntity.cpp
	CK2/ObjImpls/CKBeObject.cpp
	CK2/ObjImpls/CKGroup.cpp
	CK2/ObjImpls/CKMaterial.cpp
	CK2/ObjImpls/CKMesh.cpp
	CK2/ObjImpls/CKObject.cpp
	CK2/ObjImpls/CKTexture.cpp
	CK2/ObjImpls/CKLight.cpp
	CK2/ObjImpls/CKTargetLight.cpp
	CK2/ObjImpls/CKCamera.cpp
	CK2/ObjImpls/CKTargetCamera.cpp
	# VxMath
	VxMath/VxMemoryMappedFile.cpp
	VxMath/VxMath.cpp
	# X Container
	XContainer/XTypes.cpp
)
# Setup static library headers
target_sources(LibCmo
PUBLIC
FILE_SET HEADERS
FILES
    # Asststant header files
	VTInternal.hpp
	VTEncoding.hpp
	VTUtils.hpp
	VTAll.hpp
	# CK2
	CK2/CKDefines.hpp
	CK2/CKEnums.hpp
	CK2/CKGlobals.hpp
	CK2/CKIdentifiers.hpp
	CK2/CKTypes.hpp
	CK2/CKBitmapData.hpp
	CK2/CKContext.hpp
	CK2/CKFile.hpp
	CK2/CKStateChunk.hpp
	# CK2 Data Handler
	CK2/DataHandlers/CKBitmapHandler.hpp
	# CK2 Manager
	CK2/MgrImpls/CKBaseManager.hpp
	CK2/MgrImpls/CKObjectManager.hpp
	CK2/MgrImpls/CKPathManager.hpp
	# CK2 Object
	CK2/ObjImpls/CK3dEntity.hpp
	CK2/ObjImpls/CKBeObject.hpp
	CK2/ObjImpls/CKGroup.hpp
	CK2/ObjImpls/CKMaterial.hpp
	CK2/ObjImpls/CKMesh.hpp
	CK2/ObjImpls/CKObject.hpp
	CK2/ObjImpls/CKTexture.hpp
	CK2/ObjImpls/CK3dObject.hpp
	CK2/ObjImpls/CKRenderObject.hpp
	CK2/ObjImpls/CKSceneObject.hpp
	CK2/ObjImpls/CKLight.hpp
	CK2/ObjImpls/CKTargetLight.hpp
	CK2/ObjImpls/CKCamera.hpp
	CK2/ObjImpls/CKTargetCamera.hpp
	# VxMath
	VxMath/VxTypes.hpp
	VxMath/VxMath.hpp
	VxMath/VxEnums.hpp
	VxMath/VxMemoryMappedFile.hpp
	# X Container
	XContainer/XTypes.hpp
)
# Setup include and linked library infomation
target_include_directories(LibCmo
PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_link_libraries(LibCmo
PUBLIC
	YYCC::YYCCommonplace
PRIVATE
	ZLIB::ZLIB
	stb::stb-image
)
if (NOT WIN32)
	target_link_libraries(LibCmo PRIVATE Iconv::Iconv)
endif ()
# Setup C++ standard
set_target_properties(LibCmo
PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED 20
    CXX_EXTENSION OFF
)
target_compile_definitions(LibCmo
# Expose LibCmo build type
PUBLIC
	"$<$<CONFIG:Debug>:LIBCMO_BUILD_DEBUG>"
	"$<$<CONFIG:Release,RelWithDebInfo,MinSize>:LIBCMO_BUILD_RELEASE>"
# Unicode charset for private using
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:UNICODE>
    $<$<CXX_COMPILER_ID:MSVC>:_UNICODE>
)
target_compile_options(LibCmo
# Order build as UTF-8 in MSVC
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

# Install binary and headers
install(TARGETS LibCmo
    EXPORT LibCmoTargets
    LIBRARY DESTINATION ${NEMO_INSTALL_LIB_PATH}
    ARCHIVE DESTINATION ${NEMO_INSTALL_LIB_PATH}
    INCLUDES DESTINATION ${NEMO_INSTALL_INCLUDE_PATH}
    FILE_SET HEADERS DESTINATION ${NEMO_INSTALL_INCLUDE_PATH}
)
