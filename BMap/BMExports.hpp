#pragma once

#include "BMap.hpp"
#include <algorithm>

/*
Design Note:

+ We use raw pointer for BMFile and BMMeshTransition
	- These 2 class do no involve class inheritance so we can safely pass them to this DLL.
	- These 2 class is not a part of CK core. It is more easy to check them validation. It is enough checking them in a std::set.
+ We use CK_ID as CKObject visitor to ensure the visiting is safe.
	- We choose CK_ID because checking a valid CK_ID is more easy and cost lower performance than checking a valid CKObject*.
	- Another reason is that CKObject* relate to class inheritance and need cast. Use CK_ID can leyt them get safe cast in C++, not in other binding languages.
	- Each CK_ID also should be used with its corresponding BMFile*, because each BMfile* will create a unique CKContext*. CK_ID between different BMFile* is not shared.

*/

#pragma region Init / Dispose

LIBCMO_EXPORT void BMInit();
LIBCMO_EXPORT void BMDispose();

#pragma endregion

#pragma region BMFile

LIBCMO_EXPORT BMap::BMFile* BMFile_Load(LibCmo::CKSTRING file_name, LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[]);
LIBCMO_EXPORT BMap::BMFile* BMFile_Create(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[]);
LIBCMO_EXPORT bool BMFile_Save(BMap::BMFile* map_file, LibCmo::CKSTRING file_name, LibCmo::CKINT compreess_level);
LIBCMO_EXPORT bool BMFile_Free(BMap::BMFile* map_file);

LIBCMO_EXPORT LibCmo::CKDWORD BMFile_GetGroupCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_GetGroup(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_CreateGroup(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_Get3dObjectCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_Get3dObject(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_Create3dObject(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_GetMeshCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_GetMesh(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_CreateMesh(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_GetMaterialCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_GetMaterial(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_CreateMaterial(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_GetTextureCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_GetTexture(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_CreateTexture(BMap::BMFile* bmfile);

#pragma endregion

#pragma region BMMeshTransition

LIBCMO_EXPORT BMap::BMMeshTransition* BMMeshTrans_New();
LIBCMO_EXPORT bool BMMeshTrans_Delete(BMap::BMMeshTransition* trans);

LIBCMO_EXPORT bool BMMeshTrans_PrepareVertexCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::VxMath::VxVector3* BMMeshTrans_PrepareVertex(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT bool BMMeshTrans_PrepareNormalCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::VxMath::VxVector3* BMMeshTrans_PrepareNormal(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT bool BMMeshTrans_PrepareUVCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::VxMath::VxVector2* BMMeshTrans_PrepareUV(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT bool BMMeshTrans_PrepareMtlSlotCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::CK2::CK_ID* BMMeshTrans_PrepareMtlSlot(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT bool BMMeshTrans_PrepareFaceCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceVertexIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceNormalIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceUVIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceMtlSlot(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT bool BMMeshTrans_Parse(BMap::BMMeshTransition* trans, LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh);

#pragma endregion

#pragma region CKObject

LIBCMO_EXPORT LibCmo::CKSTRING BMObject_GetName(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid);
LIBCMO_EXPORT bool BMObject_SetName(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, LibCmo::CKSTRING name);

#pragma endregion

#pragma region CKGroup

LIBCMO_EXPORT bool BMGroup_AddObject(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, LibCmo::CK2::CK_ID memberid);
LIBCMO_EXPORT LibCmo::CKDWORD BMGroup_GetObjectCount(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMGroup_GetObject(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, LibCmo::CKDWORD pos);

#pragma endregion

#pragma region CKTexture

#pragma endregion

#pragma region CKMaterial

#pragma endregion

#pragma region CKMesh

#pragma endregion

#pragma region CK3dObject

// This struct is designed to prevent C4190 warning
struct CStyleVxMatrix {
	LibCmo::CKBYTE placeholder[sizeof(LibCmo::VxMath::VxMatrix)];
	void FromVxMatrix(const LibCmo::VxMath::VxMatrix& mat) {
		std::memcpy(this, &mat, std::min(sizeof(LibCmo::VxMath::VxMatrix), sizeof(CStyleVxMatrix)));
	}
	void ToVxMatrix(LibCmo::VxMath::VxMatrix& mat) {
		std::memcpy(&mat, this, std::min(sizeof(LibCmo::VxMath::VxMatrix), sizeof(CStyleVxMatrix)));
	}
};

LIBCMO_EXPORT CStyleVxMatrix BM3dEntity_GetWorldMatrix(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid);
LIBCMO_EXPORT bool BM3dEntity_SetWorldMatrix(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, CStyleVxMatrix mat);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BM3dEntity_GetCurrentMesh(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid);
LIBCMO_EXPORT bool BM3dEntity_SetCurrentMesh(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, LibCmo::CK2::CK_ID meshid);
LIBCMO_EXPORT bool BM3dEntity_GetVisivility(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid);
LIBCMO_EXPORT bool BM3dEntity_SetVisivility(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, bool is_visible);

#pragma endregion
