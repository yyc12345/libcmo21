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
	if (obj == nullptr || !LibCmo::CK2::CKIsChildClassOf(obj->GetClassID(), expected_cid)) return nullptr;

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
	BMPARAM_IN(BMap::NakedOutputCallback, raw_callback),
	BMPARAM_IN(LibCmo::CKDWORD, encoding_count),
	BMPARAM_IN(LibCmo::CKSTRING*, encodings),
	BMPARAM_OUT(BMap::BMFile*, out_file)) {
	if (!CheckInited()) return false;

	// create a now one and try to load data.
	std::unique_ptr<BMap::BMFile> file(new BMap::BMFile(temp_folder, texture_folder, raw_callback, encoding_count, encodings, true));
	if (file->IsInitError()) return false;
	if (!file->Load(file_name)) return false;

	// add into list and return
	g_AllBMFiles.emplace(file.get());
	BMPARAM_OUT_ASSIGN(out_file, file.release());
	return true;
}

bool BMFile_Create(
	BMPARAM_IN(LibCmo::CKSTRING, temp_folder),
	BMPARAM_IN(LibCmo::CKSTRING, texture_folder),
	BMPARAM_IN(BMap::NakedOutputCallback, raw_callback),
	BMPARAM_IN(LibCmo::CKDWORD, encoding_count),
	BMPARAM_IN(LibCmo::CKSTRING*, encodings),
	BMPARAM_OUT(BMap::BMFile*, out_file)) {
	if (!CheckInited()) return false;

	// create a now one
	std::unique_ptr<BMap::BMFile> file(new BMap::BMFile(temp_folder, texture_folder, raw_callback, encoding_count, encodings, false));
	if (file->IsInitError()) return false;

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
bool BMMeshTrans_Parse(BMPARAM_MESHTRANS_DECL(trans), BMPARAM_IN(BMap::BMFile*, bmfile), BMPARAM_IN(LibCmo::CK2::CK_ID, objid)) {
	if (!CheckBMMeshTrans(trans)) return false;
	if (!CheckBMFile(bmfile)) return false;
	return trans->Parse(bmfile, objid);
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

bool BMMaterial_GetDiffuse(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetDiffuse());
	return true;
}
bool BMMaterial_SetDiffuse(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetDiffuse(col);
	return true;
}
bool BMMaterial_GetAmbient(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetAmbient());
	return true;
}
bool BMMaterial_SetAmbient(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetAmbient(col);
	return true;
}
bool BMMaterial_GetSpecular(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetSpecular());
	return true;
}
bool BMMaterial_SetSpecular(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetSpecular(col);
	return true;
}
bool BMMaterial_GetEmissive(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxColor, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetEmissive());
	return true;
}
bool BMMaterial_SetEmissive(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxColor, col)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetEmissive(col);
	return true;
}
bool BMMaterial_GetSpecularPower(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKFLOAT, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetSpecularPower());
	return true;
}
bool BMMaterial_SetSpecularPower(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKFLOAT, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetSpecularPower(val);
	return true;
}

bool BMMaterial_GetTexture(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_texid)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_texid, SafeGetID(obj->GetTexture(0)));
	return true;
}
bool BMMaterial_SetTexture(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, texid)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	auto texobj = CheckCKTexture(bmfile, texid);
	if (obj == nullptr /*|| texobj == nullptr*/) return false;	// allow nullptr assign

	obj->SetTexture(texobj, 0);
	return true;
}
bool BMMaterial_GetTextureBorderColor(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKDWORD, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetTextureBorderColor());
	return true;
}
bool BMMaterial_SetTextureBorderColor(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKDWORD, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetTextureBorderColor(val);
	return true;
}

bool BMMaterial_GetTextureBlendMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_BLENDMODE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetTextureBlendMode());
	return true;
}
bool BMMaterial_SetTextureBlendMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_BLENDMODE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetTextureBlendMode(val);
	return true;
}
bool BMMaterial_GetTextureMinMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_FILTERMODE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetTextureMinMode());
	return true;
}
bool BMMaterial_SetTextureMinMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_FILTERMODE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetTextureMinMode(val);
	return true;
}
bool BMMaterial_GetTextureMagMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_FILTERMODE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetTextureMagMode());
	return true;
}
bool BMMaterial_SetTextureMagMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_FILTERMODE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetTextureMagMode(val);
	return true;
}
bool BMMaterial_GetTextureAddressMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXTEXTURE_ADDRESSMODE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetTextureAddressMode());
	return true;
}
bool BMMaterial_SetTextureAddressMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXTEXTURE_ADDRESSMODE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetTextureAddressMode(val);
	return true;
}

bool BMMaterial_GetSourceBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXBLEND_MODE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetSourceBlend());
	return true;
}
bool BMMaterial_SetSourceBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXBLEND_MODE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetSourceBlend(val);
	return true;
}
bool BMMaterial_GetDestBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXBLEND_MODE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetDestBlend());
	return true;
}
bool BMMaterial_SetDestBlend(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXBLEND_MODE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetDestBlend(val);
	return true;
}
bool BMMaterial_GetFillMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXFILL_MODE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetFillMode());
	return true;
}
bool BMMaterial_SetFillMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXFILL_MODE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetFillMode(val);
	return true;
}
bool BMMaterial_GetShadeMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXSHADE_MODE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetShadeMode());
	return true;
}
bool BMMaterial_SetShadeMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXSHADE_MODE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetShadeMode(val);
	return true;
}

bool BMMaterial_GetAlphaTestEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetAlphaTestEnabled());
	return true;
}
bool BMMaterial_SetAlphaTestEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetAlphaTestEnabled(enabled);
	return true;
}
bool BMMaterial_GetAlphaBlendEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetAlphaBlendEnabled());
	return true;
}
bool BMMaterial_SetAlphaBlendEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetAlphaBlendEnabled(enabled);
	return true;
}
bool BMMaterial_GetPerspectiveCorrectionEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetPerspectiveCorrectionEnabled());
	return true;
}
bool BMMaterial_SetPerspectiveCorrectionEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetPerspectiveCorrectionEnabled(enabled);
	return true;
}
bool BMMaterial_GetZWriteEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetZWriteEnabled());
	return true;
}
bool BMMaterial_SetZWriteEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetZWriteEnabled(enabled);
	return true;
}
bool BMMaterial_GetTwoSidedEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetTwoSidedEnabled());
	return true;
}
bool BMMaterial_SetTwoSidedEnabled(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, enabled)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetTwoSidedEnabled(enabled);
	return true;
}

bool BMMaterial_GetAlphaRef(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CKBYTE, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetAlphaRef());
	return true;
}
bool BMMaterial_SetAlphaRef(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CKBYTE, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetAlphaRef(val);
	return true;
}
bool BMMaterial_GetAlphaFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXCMPFUNC, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetAlphaFunc());
	return true;
}
bool BMMaterial_SetAlphaFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXCMPFUNC, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetAlphaFunc(val);
	return true;
}
bool BMMaterial_GetZFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXCMPFUNC, out_val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	BMPARAM_OUT_ASSIGN(out_val, obj->GetZFunc());
	return true;
}
bool BMMaterial_SetZFunc(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXCMPFUNC, val)) {
	auto obj = CheckCKMaterial(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetZFunc(val);
	return true;
}

#pragma endregion

#pragma region CKMesh

LIBCMO_EXPORT bool BMMesh_GetLitMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VXMESH_LITMODE, out_mode)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_mode, obj->GetLitMode());
	return true;
}
LIBCMO_EXPORT bool BMMesh_SetLitMode(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VXMESH_LITMODE, mode)) {
	auto obj = CheckCKMesh(bmfile, objid);
	if (obj == nullptr) return false;

	obj->SetLitMode(mode);
	return true;
}

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

bool BM3dObject_GetWorldMatrix(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::VxMath::VxMatrix, out_mat)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_mat, obj->GetWorldMatrix());
	return true;
}

bool BM3dObject_SetWorldMatrix(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::VxMath::VxMatrix, mat)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	obj->SetWorldMatrix(mat);
	return true;
}

bool BM3dObject_GetCurrentMesh(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(LibCmo::CK2::CK_ID, out_meshid)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_meshid, SafeGetID(obj->GetCurrentMesh()));
	return true;
}

bool BM3dObject_SetCurrentMesh(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(LibCmo::CK2::CK_ID, meshid)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	auto meshobj = CheckCKMesh(bmfile, meshid);
	if (obj == nullptr /*|| meshobj == nullptr*/) return false;	//allow nullptr assign

	obj->SetCurrentMesh(meshobj);
	return true;
}

bool BM3dObject_GetVisibility(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_OUT(bool, out_isVisible)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	BMPARAM_OUT_ASSIGN(out_isVisible, obj->IsVisible());
	return true;
}

bool BM3dObject_SetVisibility(BMPARAM_OBJECT_DECL(bmfile, objid), BMPARAM_IN(bool, is_visible)) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	obj->Show(is_visible ? LibCmo::CK2::CK_OBJECT_SHOWOPTION::CKSHOW : LibCmo::CK2::CK_OBJECT_SHOWOPTION::CKHIDE);
	return true;
}

#pragma endregion

