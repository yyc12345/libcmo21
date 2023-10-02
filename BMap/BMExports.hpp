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
LIBCMO_EXPORT void BMFile_Free(BMap::BMFile* map_file);

#define VISITOR_DECL(namepart) \
LIBCMO_EXPORT LibCmo::CKDWORD BMFile_Get ## namepart ## Count(BMap::BMFile* map_file); \
LIBCMO_EXPORT LibCmo::CK2::ObjImpls::CK ## namepart * BMFile_Get ## namepart (BMap::BMFile* map_file, LibCmo::CKDWORD idx); \
LIBCMO_EXPORT LibCmo::CK2::ObjImpls::CK ## namepart * BMFile_Create ## namepart (BMap::BMFile* map_file, LibCmo::CKSTRING name);

VISITOR_DECL(Group)
VISITOR_DECL(3dObject)
VISITOR_DECL(Mesh)
VISITOR_DECL(Material)
VISITOR_DECL(Texture)

#undef VISITOR_DECL

#pragma endregion

#pragma region BMMeshTransition

LIBCMO_EXPORT BMap::BMMeshTransition* BMMeshTrans_New();
LIBCMO_EXPORT void BMMeshTrans_Delete(BMap::BMMeshTransition* trans);

LIBCMO_EXPORT void BMMeshTrans_PrepareVertexCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::VxMath::VxVector3* BMMeshTrans_PrepareVertex(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT void BMMeshTrans_PrepareNormalCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::VxMath::VxVector3* BMMeshTrans_PrepareNormal(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT void BMMeshTrans_PrepareUVCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::VxMath::VxVector2* BMMeshTrans_PrepareUV(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT void BMMeshTrans_PrepareMtlSlotCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::CK2::ObjImpls::CKMaterial** BMMeshTrans_PrepareMtlSlot(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT void BMMeshTrans_PrepareFaceCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceVertexIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceNormalIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceUVIndices(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT LibCmo::CKDWORD* BMMeshTrans_PrepareFaceMtlSlot(BMap::BMMeshTransition* trans);
LIBCMO_EXPORT bool BMMeshTrans_Parse(BMap::BMMeshTransition* trans, LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh);

#pragma endregion


