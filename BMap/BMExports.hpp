#pragma once

#include "BMap.hpp"

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
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_CreateGroup(BMap::BMFile* bmfile, LibCmo::CKSTRING name);
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_Get3dObjectCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_Get3dObject(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_Create3dObject(BMap::BMFile* bmfile, LibCmo::CKSTRING name);
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_GetMeshCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_GetMesh(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_CreateMesh(BMap::BMFile* bmfile, LibCmo::CKSTRING name);
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_GetMaterialCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_GetMaterial(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_CreateMaterial(BMap::BMFile* bmfile, LibCmo::CKSTRING name);
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_GetTextureCount(BMap::BMFile* bmfile);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_GetTexture(BMap::BMFile* bmfile, LibCmo::CKDWORD idx);
LIBCMO_EXPORT LibCmo::CK2::CK_ID BMFile_CreateTexture(BMap::BMFile* bmfile, LibCmo::CKSTRING name);

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
LIBCMO_EXPORT LibCmo::CK2::ObjImpls::CKMaterial** BMMeshTrans_PrepareMtlSlot(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT bool BMMeshTrans_PrepareFaceCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceVertexIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceNormalIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceUVIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceMtlSlot(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT bool BMMeshTrans_Parse(BMap::BMMeshTransition* trans, LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh);
#pragma endregion


