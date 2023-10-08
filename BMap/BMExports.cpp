#include "BMExports.hpp"
#include <IronPad.hpp>
#include <set>
#include <type_traits>
#include <memory>

#pragma region Help & Save Functions

static constexpr const LibCmo::CK2::CK_ID INVALID_CKID = 0;
static LibCmo::CK2::CK_ID SafeGetID(LibCmo::CK2::ObjImpls::CKObject* obj) {
	if (obj == nullptr) return INVALID_CKID;
	else return obj->GetID();
}

static bool g_IsInited = false;
static std::set<BMap::BMFile*> g_AllBMFiles = std::set<BMap::BMFile*>();
static std::set<BMap::BMMeshTransition*> g_AllBMMeshTrans = std::set<BMap::BMMeshTransition*>();

bool CheckInited() {
	return g_IsInited;
}

bool CheckBMFile(BMap::BMFile* possible_bmfile) {
	return (g_IsInited && possible_bmfile != nullptr && g_AllBMFiles.contains(possible_bmfile));
}

bool CheckBMMeshTrans(BMap::BMMeshTransition* possible_trans) {
	return (g_IsInited && possible_trans != nullptr && g_AllBMMeshTrans.contains(possible_trans));
}

template<class _Ty, std::enable_if_t<std::is_pointer_v<_Ty>, int> = 0>
_Ty CheckGeneralObject(BMap::BMFile* possible_bmfile, LibCmo::CK2::CK_ID possible_id, LibCmo::CK2::CK_CLASSID expected_cid) {
	// check bm file self.
	if (!CheckBMFile(possible_bmfile)) return nullptr;
	// check id
	LibCmo::CK2::ObjImpls::CKObject* obj = possible_bmfile->GetObjectPtr(possible_id);
	// check id validation and class id
	if (obj == nullptr || LibCmo::CK2::CKIsChildClassOf(obj->GetClassID(), expected_cid)) return nullptr;

	return static_cast<_Ty>(obj);
}

#define CheckCKObject(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKObject*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_OBJECT)
#define CheckCKGroup(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKGroup*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_GROUP)
#define CheckCK3dObject(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CK3dObject*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_3DOBJECT)
#define CheckCKMesh(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKMesh*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_MESH)
#define CheckCKMaterial(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKMaterial*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL)
#define CheckCKTexture(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKTexture*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE)

#pragma endregion

#pragma region Module Init & Dispose

bool BMInit() {
	if (CheckInited()) return false;

	// register IronPad
	IronPad::IronPadRegister();
	// and startup CK environment
	LibCmo::CK2::CKStartUp();

	// set init
	g_IsInited = true;

	return true;
}

bool BMDispose() {
	if (!CheckInited()) return false;

	// free all existed file reader / writer
	for (auto ptr : g_AllBMFiles) {
		delete ptr;
	}
	g_AllBMFiles.clear();
	// free all mesh transition
	for (auto ptr : g_AllBMMeshTrans) {
		delete ptr;
	}
	g_AllBMMeshTrans.clear();

	// disable init
	g_IsInited = false;

	// shutdown CK environment
	LibCmo::CK2::CKShutdown();
	// unregister iron pad
	IronPad::IronPadUnregister();

	return true;
}

#pragma endregion

#pragma region BMFile

bool BMFile_Load(
	BMPARAM_IN(LibCmo::CKSTRING, file_name),
	BMPARAM_IN(LibCmo::CKSTRING, temp_folder),
	BMPARAM_IN(LibCmo::CKSTRING, texture_folder),
	BMPARAM_IN(LibCmo::CKDWORD, encoding_count),
	BMPARAM_IN(LibCmo::CKSTRING*, encodings),
	BMPARAM_OUT(BMap::BMFile*, out_file)) {
	if (!CheckInited()) return false;

	// create a now one and try to load data.
	std::unique_ptr<BMap::BMFile> file(new BMap::BMFile(temp_folder, texture_folder, encoding_count, encodings, false));
	if (file->IsFreezed()) return false;
	if (!file->Load(file_name)) return false;

	// add into list and return
	g_AllBMFiles.emplace(file.get());
	BMPARAM_OUT_ASSIGN(out_file, file.release());
	return true;
}

bool BMFile_Create(
	BMPARAM_IN(LibCmo::CKSTRING, temp_folder),
	BMPARAM_IN(LibCmo::CKSTRING, texture_folder),
	BMPARAM_IN(LibCmo::CKDWORD, encoding_count),
	BMPARAM_IN(LibCmo::CKSTRING*, encodings),
	BMPARAM_OUT(BMap::BMFile*, out_file)) {
	if (!CheckInited()) return false;

	// create a now one
	std::unique_ptr<BMap::BMFile> file(new BMap::BMFile(temp_folder, texture_folder, encoding_count, encodings, false));
	if (file->IsFreezed()) return false;

	// add into list and return if success
	g_AllBMFiles.emplace(file.get());
	BMPARAM_OUT_ASSIGN(out_file, file.release());
	return true;
}

bool BMFile_Save(
	BMPARAM_IN(BMap::BMFile*, map_file),
	BMPARAM_IN(LibCmo::CKSTRING, file_name),
	BMPARAM_IN(LibCmo::CKINT, compreess_level)) {
	if (!CheckBMFile(map_file)) return false;

	return map_file->Save(file_name, compreess_level);
}

bool BMFile_Free(BMPARAM_IN(BMap::BMFile*, map_file)) {
	if (!CheckBMFile(map_file)) return false;

	g_AllBMFiles.erase(map_file);
	delete map_file;
	return true;
}

bool BMFile_GetGroupCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_count, bmfile->GetGroupCount());
	return true;
}
bool BMFile_GetGroup(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->GetGroup(idx));
	return true;
}
bool BMFile_CreateGroup(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->CreateGroup());
	return true;
}
bool BMFile_Get3dObjectCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_count, bmfile->Get3dObjectCount());
	return true;
}
bool BMFile_Get3dObject(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->Get3dObject(idx));
	return true;
}
bool BMFile_Create3dObject(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->Create3dObject());
	return true;
}
bool BMFile_GetMeshCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_count, bmfile->GetMeshCount());
	return true;
}
bool BMFile_GetMesh(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->GetMesh(idx));
	return true;
}
bool BMFile_CreateMesh(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->CreateMesh());
	return true;
}
bool BMFile_GetMaterialCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_count, bmfile->GetMaterialCount());
	return true;
}
bool BMFile_GetMaterial(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->GetMaterial(idx));
	return true;
}
bool BMFile_CreateMaterial(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->CreateMaterial());
	return true;
}
bool BMFile_GetTextureCount(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_count, bmfile->GetTextureCount());
	return true;
}
bool BMFile_GetTexture(BMPARAM_FILE_DECL(bmfile), BMPARAM_IN(LibCmo::CKDWORD, idx), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->GetTexture(idx));
	return true;
}
bool BMFile_CreateTexture(BMPARAM_FILE_DECL(bmfile), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_id)) {
	if (!CheckBMFile(bmfile)) return false;
	BMPARAM_OUT_ASSIGN(out_id, bmfile->CreateTexture());
	return true;
}

#pragma endregion

#pragma region BMMeshTransition

bool BMMeshTrans_New(BMPARAM_OUT(BMap::BMMeshTransition*, out_trans)) {
	if (!CheckInited()) return false;

	// create new one, insert and return.
	auto meshtrans = new BMap::BMMeshTransition();
	g_AllBMMeshTrans.emplace(meshtrans);
	BMPARAM_OUT_ASSIGN(out_trans, meshtrans);
	return true;
}

bool BMMeshTrans_Delete(BMPARAM_IN(BMap::BMMeshTransition*, trans)) {
	if (!CheckBMMeshTrans(trans)) return false;

	g_AllBMMeshTrans.erase(trans);
	delete trans;
	return true;
}

bool BMMeshTrans_PrepareVertexCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count)) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareVertexCount(count);
}
bool BMMeshTrans_PrepareVertex(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem)) {
	if (!CheckBMMeshTrans(trans)) return false;
	BMPARAM_OUT_ASSIGN(out_mem, trans->PrepareVertex());
	return BMPARAM_OUT_VAL(out_mem) != nullptr;
}
bool BMMeshTrans_PrepareNormalCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count)) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareNormalCount(count);
}
bool BMMeshTrans_PrepareNormal(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem)) {
	if (!CheckBMMeshTrans(trans)) return false;
	BMPARAM_OUT_ASSIGN(out_mem, trans->PrepareNormal());
	return BMPARAM_OUT_VAL(out_mem) != nullptr;
}
bool BMMeshTrans_PrepareUVCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count)) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareUVCount(count);
}
bool BMMeshTrans_PrepareUV(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::VxMath::VxVector2*, out_mem)) {
	if (!CheckBMMeshTrans(trans)) return false;
	BMPARAM_OUT_ASSIGN(out_mem, trans->PrepareUV());
	return BMPARAM_OUT_VAL(out_mem) != nullptr;
}
bool BMMeshTrans_PrepareMtlSlotCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count)) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareMtlSlotCount(count);
}
bool BMMeshTrans_PrepareMtlSlot(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CK2::CK_ID*, out_mem)) {
	if (!CheckBMMeshTrans(trans)) return false;
	BMPARAM_OUT_ASSIGN(out_mem, trans->PrepareMtlSlot());
	return BMPARAM_OUT_VAL(out_mem) != nullptr;
}
bool BMMeshTrans_PrepareFaceCount(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CKDWORD, count)) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareFaceCount(count);
}
bool BMMeshTrans_PrepareFaceVertexIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem)) {
	if (!CheckBMMeshTrans(trans)) return false;
	BMPARAM_OUT_ASSIGN(out_mem, trans->PrepareFaceVertexIndices());
	return BMPARAM_OUT_VAL(out_mem) != nullptr;
}
bool BMMeshTrans_PrepareFaceNormalIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem)) {
	if (!CheckBMMeshTrans(trans)) return false;
	BMPARAM_OUT_ASSIGN(out_mem, trans->PrepareFaceNormalIndices());
	return BMPARAM_OUT_VAL(out_mem) != nullptr;
}
bool BMMeshTrans_PrepareFaceUVIndices(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem)) {
	if (!CheckBMMeshTrans(trans)) return false;
	BMPARAM_OUT_ASSIGN(out_mem, trans->PrepareFaceUVIndices());
	return BMPARAM_OUT_VAL(out_mem) != nullptr;
}
bool BMMeshTrans_PrepareFaceMtlSlot(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_OUT(LibCmo::CKDWORD*, out_mem)) {
	if (!CheckBMMeshTrans(trans)) return false;
	BMPARAM_OUT_ASSIGN(out_mem, trans->PrepareFaceMtlSlot());
	return BMPARAM_OUT_VAL(out_mem) != nullptr;
}
bool BMMeshTrans_Parse(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(LibCmo::CK2::ObjImpls::CKMesh*, write_into_mesh)) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->Parse(write_into_mesh);
}

#pragma endregion

#pragma region CKObject

bool BMObject_GetName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKSTRING, out_name)) {
	auto obj = CheckCKObject(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_name, obj->GetName());
	return true;
}

bool BMObject_SetName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, name)) {
	auto obj = CheckCKObject(bmfile, objid);
	if (obj == nullptr) return false;

	obj->SetName(name);
	return true;
}

#pragma endregion

#pragma region CKGroup

bool BMGroup_AddObject(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, memberid)) {
	auto obj = CheckCKGroup(bmfile, objid);
	auto memberobj = CheckCK3dObject(bmfile, memberid);
	if (obj == nullptr || memberobj == nullptr) return false;

	return obj->AddObject(memberobj) == LibCmo::CK2::CKERROR::CKERR_OK;
}

bool BMGroup_GetObjectCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	auto obj = CheckCKGroup(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_count, obj->GetObjectCount());
	return true;
}

bool BMGroup_GetObject(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, pos), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_objid)) {
	auto obj = CheckCKGroup(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_objid, SafeGetID(obj->GetObject(pos)));
	return true;
}

#pragma endregion

#pragma region CKTexture

bool BMTexture_GetFileName(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKSTRING, out_filename)) {
	auto obj = CheckCKTexture(bmfile, objid);
	if (obj == nullptr) return false;

	if (obj->GetUnderlyingData().GetSlotCount() == 0) return false;

	BMPARAM_OUT_ASSIGN(out_filename, obj->GetUnderlyingData().GetSlotFileName(0));
	return true;
}

bool BMTexture_LoadImage(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, filename)) {
	auto obj = CheckCKTexture(bmfile, objid);
	if (obj == nullptr) return false;

	// resize slot count if needed
	if (obj->GetUnderlyingData().GetSlotCount() == 0) {
		obj->GetUnderlyingData().SetSlotCount(1);
	}

	return obj->LoadImage(filename, 0);
}

bool BMTexture_SaveImage(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKSTRING, filename)) {
	auto obj = CheckCKTexture(bmfile, objid);
	if (obj == nullptr) return false;

	return obj->GetUnderlyingData().SaveImage(filename, 0);
}

bool BMTexture_GetSaveOptions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS, out_saveopt)) {
	auto obj = CheckCKTexture(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_saveopt, obj->GetUnderlyingData().GetSaveOptions());
	return true;
}

bool BMTexture_SetSaveOptions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS, saveopt)) {
	auto obj = CheckCKTexture(bmfile, objid);
	if (obj == nullptr) return false;

	obj->GetUnderlyingData().SetSaveOptions(saveopt);
	return true;
}

bool BMTexture_GetVideoFormat(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VX_PIXELFORMAT, out_vfmt)) {
	auto obj = CheckCKTexture(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_vfmt, obj->GetVideoFormat());
	return true;
}

bool BMTexture_SetVideoFormat(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VX_PIXELFORMAT, vfmt)) {
	auto obj = CheckCKTexture(bmfile, objid);
	if (obj == nullptr) return false;

	obj->SetVideoFormat(vfmt);
	return true;
}

#pragma endregion

#pragma region CKMaterial

#pragma endregion

#pragma region CKMesh

bool BMMesh_GetVertexCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_count, obj->GetVertexCount());
	return true;
}
bool BMMesh_SetVertexCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	obj->SetVertexCount(count);
	return true;
}
bool BMMesh_GetVertexPositions(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_mem, obj->GetVertexPositions());
	return true;
}
bool BMMesh_GetVertexNormals(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector3*, out_mem)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_mem, obj->GetVertexNormals());
	return true;
}
bool BMMesh_GetVertexUVs(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxVector2*, out_mem)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_mem, obj->GetVertexUVs());
	return true;
}

bool BMMesh_GetFaceCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_count, obj->GetFaceCount());
	return true;
}
bool BMMesh_SetFaceCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	obj->SetFaceCount(count);
	return true;
}
bool BMMesh_GetFaceIndices(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKWORD*, out_mem)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_mem, obj->GetFaceIndices());
	return true;
}
bool BMMesh_GetFaceMaterialSlotIndexs(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKWORD*, out_mem)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_mem, obj->GetFaceMaterialSlotIndexs());
	return true;
}

bool BMMesh_GetMaterialSlotCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_count)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_count, obj->GetMaterialSlotCount());
	return true;
}
bool BMMesh_SetMaterialSlotCount(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, count)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	obj->SetMaterialSlotCount(count);
	return true;
}
bool BMMesh_GetMaterialSlot(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, index), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_mtlid)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;
	if (index >= obj->GetMaterialSlotCount()) return false;

	BMPARAM_OUT_ASSIGN(out_mtlid, SafeGetID(obj->GetMaterialSlots()[index]));
	return true;
}
bool BMMesh_SetMaterialSlot(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, index), BMPARAM_IN(LibCmo::CK2::CK_ID, mtlid)) {
	auto obj = CheckCKMesh(bmfile, objid);
	auto mtlobj = CheckCKMaterial(bmfile, mtlid);
	if (obj == nullptr /*|| mtlobj == nullptr*/) return false;	// allow nullptr assign
	if (index >= obj->GetMaterialSlotCount()) return false;

	obj->GetMaterialSlots()[index] = mtlobj;
	return true;
}

#pragma endregion

#pragma region CK3dObject

bool BM3dEntity_GetWorldMatrix(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxMatrix, out_mat)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_mat, obj->GetWorldMatrix());
	return true;
}

bool BM3dEntity_SetWorldMatrix(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxMatrix, mat)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	obj->SetWorldMatrix(mat);
	return true;
}

bool BM3dEntity_GetCurrentMesh(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_meshid)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_meshid, SafeGetID(obj->GetCurrentMesh()));
	return true;
}

bool BM3dEntity_SetCurrentMesh(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, meshid)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	auto meshobj = CheckCKMesh(bmfile, meshid);
	if (obj == nullptr /*|| meshobj == nullptr*/) return false;	//allow nullptr assign

	obj->SetCurrentMesh(meshobj);
	return true;
}

bool BM3dEntity_GetVisibility(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_isVisible)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_isVisible, obj->IsVisible());
	return true;
}

bool BM3dEntity_SetVisibility(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, is_visible)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	obj->Show(is_visible ? LibCmo::CK2::CK_OBJECT_SHOWOPTION::CKSHOW : LibCmo::CK2::CK_OBJECT_SHOWOPTION::CKHIDE);
	return true;
}

#pragma endregion

