# Configure version file
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/LibCmo/VTVersion.hpp.in
    ${CMAKE_CURRENT_LIST_DIR}/LibCmo/VTVersion.hpp
    @ONLY
)

# Create static library
add_library(LibCmo STATIC "")
# Setup static library sources
target_sources(LibCmo
PRIVATE
	# NeMo Shared
	LibCmo/VTEncoding.cpp
	LibCmo/VTImage.cpp
	# CK2
	LibCmo/CK2/CKBitmapData.cpp
	LibCmo/CK2/CKContext.cpp
	LibCmo/CK2/CKFileOthers.cpp
	LibCmo/CK2/CKFileReader.cpp
	LibCmo/CK2/CKFileWriter.cpp
	LibCmo/CK2/CKGlobals.cpp
	LibCmo/CK2/CKStateChunkOthers.cpp
	LibCmo/CK2/CKStateChunkReader.cpp
	LibCmo/CK2/CKStateChunkWriter.cpp
	# CK2 Data Handler
	LibCmo/CK2/DataHandlers/CKBitmapHandler.cpp
	# CK2 Manager
	LibCmo/CK2/MgrImpls/CKBaseManager.cpp
	LibCmo/CK2/MgrImpls/CKObjectManager.cpp
	LibCmo/CK2/MgrImpls/CKPathManager.cpp
	# CK2 Object
	LibCmo/CK2/ObjImpls/CK3dEntity.cpp
	LibCmo/CK2/ObjImpls/CKBeObject.cpp
	LibCmo/CK2/ObjImpls/CKGroup.cpp
	LibCmo/CK2/ObjImpls/CKMaterial.cpp
	LibCmo/CK2/ObjImpls/CKMesh.cpp
	LibCmo/CK2/ObjImpls/CKObject.cpp
	LibCmo/CK2/ObjImpls/CKTexture.cpp
	LibCmo/CK2/ObjImpls/CKLight.cpp
	LibCmo/CK2/ObjImpls/CKTargetLight.cpp
	LibCmo/CK2/ObjImpls/CKCamera.cpp
	LibCmo/CK2/ObjImpls/CKTargetCamera.cpp
	# VxMath
	LibCmo/VxMath/VxMemoryMappedFile.cpp
    LibCmo/VxMath/VxTypes.cpp
	LibCmo/VxMath/VxMath.cpp
	# X Container
	LibCmo/XContainer/XTypes.cpp
)
# Setup static library headers
target_sources(LibCmo
PUBLIC
FILE_SET HEADERS
FILES
	# NeMo Shared
	VTAll.hpp
	LibCmo/VTVersion.hpp
	LibCmo/VTInternal.hpp
	LibCmo/VTEncoding.hpp
	LibCmo/VTUtils.hpp
	# CK2
	LibCmo/CK2/CKDefines.hpp
	LibCmo/CK2/CKEnums.hpp
	LibCmo/CK2/CKGlobals.hpp
	LibCmo/CK2/CKIdentifiers.hpp
	LibCmo/CK2/CKTypes.hpp
	LibCmo/CK2/CKBitmapData.hpp
	LibCmo/CK2/CKContext.hpp
	LibCmo/CK2/CKFile.hpp
	LibCmo/CK2/CKStateChunk.hpp
	# CK2 Data Handler
	LibCmo/CK2/DataHandlers/CKBitmapHandler.hpp
	# CK2 Manager
	LibCmo/CK2/MgrImpls/CKBaseManager.hpp
	LibCmo/CK2/MgrImpls/CKObjectManager.hpp
	LibCmo/CK2/MgrImpls/CKPathManager.hpp
	# CK2 Object
	LibCmo/CK2/ObjImpls/CK3dEntity.hpp
	LibCmo/CK2/ObjImpls/CKBeObject.hpp
	LibCmo/CK2/ObjImpls/CKGroup.hpp
	LibCmo/CK2/ObjImpls/CKMaterial.hpp
	LibCmo/CK2/ObjImpls/CKMesh.hpp
	LibCmo/CK2/ObjImpls/CKObject.hpp
	LibCmo/CK2/ObjImpls/CKTexture.hpp
	LibCmo/CK2/ObjImpls/CK3dObject.hpp
	LibCmo/CK2/ObjImpls/CKRenderObject.hpp
	LibCmo/CK2/ObjImpls/CKSceneObject.hpp
	LibCmo/CK2/ObjImpls/CKLight.hpp
	LibCmo/CK2/ObjImpls/CKTargetLight.hpp
	LibCmo/CK2/ObjImpls/CKCamera.hpp
	LibCmo/CK2/ObjImpls/CKTargetCamera.hpp
	# VxMath
	LibCmo/VxMath/VxTypes.hpp
	LibCmo/VxMath/VxMath.hpp
	LibCmo/VxMath/VxEnums.hpp
	LibCmo/VxMath/VxMemoryMappedFile.hpp
	# X Container
	LibCmo/XContainer/XTypes.hpp
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
	STB::STB
)
target_compile_definitions(LibCmo
# Expose LibCmo build type
PUBLIC
	"$<$<CONFIG:Debug>:LIBCMO_BUILD_DEBUG>"
	"$<$<CONFIG:Release,RelWithDebInfo,MinSize>:LIBCMO_BUILD_RELEASE>"
)

# Install binary and headers
install(TARGETS LibCmo
    EXPORT LibCmoTargets
    LIBRARY DESTINATION ${NEMO_INSTALL_LIB_PATH}
    ARCHIVE DESTINATION ${NEMO_INSTALL_LIB_PATH}
    INCLUDES DESTINATION ${NEMO_INSTALL_INCLUDE_PATH}
    FILE_SET HEADERS DESTINATION ${NEMO_INSTALL_INCLUDE_PATH}
)
