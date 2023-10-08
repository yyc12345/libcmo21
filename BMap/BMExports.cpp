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
//#define CheckCKBeObject(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKBeObject*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_BEOBJECT)
#define CheckCKGroup(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKGroup*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_GROUP)
#define CheckCK3dObject(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CK3dObject*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_3DOBJECT)
#define CheckCKMesh(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKMesh*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_MESH)
#define CheckCKMaterial(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKMaterial*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL)
#define CheckCKTexture(bmfile, objid) CheckGeneralObject<LibCmo::CK2::ObjImpls::CKTexture*>(bmfile, objid, LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE)

#pragma endregion

void BMInit() {
	// register IronPad
	IronPad::IronPadRegister();
	// and startup CK environment
	LibCmo::CK2::CKStartUp();

	// set init
	g_IsInited = true;
}

void BMDispose() {
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
}

#pragma region BMFile

BMap::BMFile* BMFile_Load(LibCmo::CKSTRING file_name, LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[]) {
	if (!CheckInited()) return nullptr;

	// create a now one and try to load data.
	std::unique_ptr<BMap::BMFile> file(new BMap::BMFile(temp_folder, texture_folder, encoding_count, encodings, false));
	if (file->IsFreezed()) return nullptr;
	if (!file->Load(file_name)) return nullptr;

	// add into list and return
	g_AllBMFiles.emplace(file.get());
	return file.release();
}

BMap::BMFile* BMFile_Create(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[]) {
	if (!CheckInited()) return nullptr;

	// create a now one
	std::unique_ptr<BMap::BMFile> file(new BMap::BMFile(temp_folder, texture_folder, encoding_count, encodings, false));
	if (file->IsFreezed()) return nullptr;

	// add into list and return if success
	g_AllBMFiles.emplace(file.get());
	return file.release();
}

bool BMFile_Save(BMap::BMFile* map_file, LibCmo::CKSTRING file_name, LibCmo::CKINT compreess_level) {
	if (!CheckBMFile(map_file)) return false;

	return map_file->Save(file_name, compreess_level);
}

bool BMFile_Free(BMap::BMFile* map_file) {
	if (!CheckBMFile(map_file)) return false;

	g_AllBMFiles.erase(map_file);
	delete map_file;
	return true;
}

LibCmo::CKDWORD BMFile_GetGroupCount(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->GetGroupCount();
}
LibCmo::CK2::CK_ID BMFile_GetGroup(BMap::BMFile* bmfile, LibCmo::CKDWORD idx) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->GetGroup(idx);
}
LibCmo::CK2::CK_ID BMFile_CreateGroup(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->CreateGroup();
}
LibCmo::CKDWORD BMFile_Get3dObjectCount(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->Get3dObjectCount();
}
LibCmo::CK2::CK_ID BMFile_Get3dObject(BMap::BMFile* bmfile, LibCmo::CKDWORD idx) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->Get3dObject(idx);
}
LibCmo::CK2::CK_ID BMFile_Create3dObject(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->Create3dObject();
}
LibCmo::CKDWORD BMFile_GetMeshCount(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->GetMeshCount();
}
LibCmo::CK2::CK_ID BMFile_GetMesh(BMap::BMFile* bmfile, LibCmo::CKDWORD idx) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->GetMesh(idx);
}
LibCmo::CK2::CK_ID BMFile_CreateMesh(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->CreateMesh();
}
LibCmo::CKDWORD BMFile_GetMaterialCount(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->GetMaterialCount();
}
LibCmo::CK2::CK_ID BMFile_GetMaterial(BMap::BMFile* bmfile, LibCmo::CKDWORD idx) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->GetMaterial(idx);
}
LibCmo::CK2::CK_ID BMFile_CreateMaterial(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->CreateMaterial();
}
LibCmo::CKDWORD BMFile_GetTextureCount(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->GetTextureCount();
}
LibCmo::CK2::CK_ID BMFile_GetTexture(BMap::BMFile* bmfile, LibCmo::CKDWORD idx) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->GetTexture(idx);
}
LibCmo::CK2::CK_ID BMFile_CreateTexture(BMap::BMFile* bmfile) {
	if (!CheckBMFile(bmfile)) return 0;
	return bmfile->CreateTexture();
}

#pragma endregion

#pragma region BMMeshTransition

BMap::BMMeshTransition* BMMeshTrans_New() {
	if (!CheckInited()) return nullptr;

	// create new one, insert and return.
	auto meshtrans = new BMap::BMMeshTransition();
	g_AllBMMeshTrans.emplace(meshtrans);
	return meshtrans;
}

bool BMMeshTrans_Delete(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return false;

	g_AllBMMeshTrans.erase(trans);
	delete trans;
	return true;
}

bool BMMeshTrans_PrepareVertexCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareVertexCount(count);
}
LibCmo::VxMath::VxVector3* BMMeshTrans_PrepareVertex(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return nullptr;
	return trans->PrepareVertex();
}
bool BMMeshTrans_PrepareNormalCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareNormalCount(count);
}
LibCmo::VxMath::VxVector3* BMMeshTrans_PrepareNormal(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return nullptr;
	return trans->PrepareNormal();
}
bool BMMeshTrans_PrepareUVCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareUVCount(count);
}
LibCmo::VxMath::VxVector2* BMMeshTrans_PrepareUV(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return nullptr;
	return trans->PrepareUV();
}
bool BMMeshTrans_PrepareMtlSlotCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareMtlSlotCount(count);
}
LibCmo::CK2::CK_ID* BMMeshTrans_PrepareMtlSlot(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return nullptr;
	return trans->PrepareMtlSlot();
}
bool BMMeshTrans_PrepareFaceCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->PrepareFaceCount(count);
}
LibCmo::CKDWORD* BMMeshTrans_PrepareFaceVertexIndices(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return nullptr;
	return trans->PrepareFaceVertexIndices();
}
LibCmo::CKDWORD* BMMeshTrans_PrepareFaceNormalIndices(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return nullptr;
	return trans->PrepareFaceNormalIndices();
}
LibCmo::CKDWORD* BMMeshTrans_PrepareFaceUVIndices(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return nullptr;
	return trans->PrepareFaceUVIndices();
}
LibCmo::CKDWORD* BMMeshTrans_PrepareFaceMtlSlot(BMap::BMMeshTransition* trans) {
	if (!CheckBMMeshTrans(trans)) return nullptr;
	return trans->PrepareFaceMtlSlot();
}
bool BMMeshTrans_Parse(BMap::BMMeshTransition* trans, LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh) {
	if (!CheckBMMeshTrans(trans)) return false;
	return trans->Parse(write_into_mesh);
}

#pragma endregion

#pragma region CKObject

LibCmo::CKSTRING BMObject_GetName(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid) {
	auto obj = CheckCKObject(bmfile, objid);
	if (obj == nullptr) return nullptr;
	return obj->GetName();
}

bool BMObject_SetName(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, LibCmo::CKSTRING name) {
	auto obj = CheckCKObject(bmfile, objid);
	if (obj == nullptr) return false;
	obj->SetName(name);
	return true;
}

#pragma endregion

#pragma region CKGroup

bool BMGroup_AddObject(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, LibCmo::CK2::CK_ID memberid) {
	auto obj = CheckCKGroup(bmfile, objid);
	auto memberobj = CheckCK3dObject(bmfile, memberid);
	if (obj == nullptr || memberobj == nullptr) return false;
	
	return obj->AddObject(memberobj) == LibCmo::CK2::CKERROR::CKERR_OK;
}

LibCmo::CKDWORD BMGroup_GetObjectCount(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid) {
	auto obj = CheckCKGroup(bmfile, objid);
	if (obj == nullptr) return false;
	return obj->GetObjectCount();
}

LibCmo::CK2::CK_ID BMGroup_GetObject(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, LibCmo::CKDWORD pos) {
	auto obj = CheckCKGroup(bmfile, objid);
	if (obj == nullptr) return 0;
	
	return SafeGetID(obj->GetObject(pos));
}

#pragma endregion

#pragma region CKTexture

#pragma endregion

#pragma region CKMaterial

#pragma endregion

#pragma region CKMesh

#pragma endregion

#pragma region CK3dObject

CStyleVxMatrix BM3dEntity_GetWorldMatrix(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid) {
	CStyleVxMatrix result;
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) {
		result.FromVxMatrix(LibCmo::VxMath::VxMatrix());
	} else {
		result.FromVxMatrix(obj->GetWorldMatrix());
	}
	
	return result;
}

bool BM3dEntity_SetWorldMatrix(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, CStyleVxMatrix mat) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	LibCmo::VxMath::VxMatrix cppmat;
	mat.ToVxMatrix(cppmat);
	obj->SetWorldMatrix(cppmat);
	return true;
}

LibCmo::CK2::CK_ID BM3dEntity_GetCurrentMesh(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return 0;

	return SafeGetID(obj->GetCurrentMesh());
}

bool BM3dEntity_SetCurrentMesh(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, LibCmo::CK2::CK_ID meshid) {
	auto obj = CheckCK3dObject(bmfile, objid);
	auto meshobj = CheckCKMesh(bmfile, meshid);
	if (obj == nullptr || meshobj == nullptr) return false;

	obj->SetCurrentMesh(meshobj);
	return true;
}

bool BM3dEntity_GetVisibility(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	return obj->IsVisible();
}

bool BM3dEntity_SetVisibility(BMap::BMFile* bmfile, LibCmo::CK2::CK_ID objid, bool is_visible) {
	auto obj = CheckCK3dObject(bmfile, objid);
	if (obj == nullptr) return false;

	obj->Show(is_visible ? LibCmo::CK2::CK_OBJECT_SHOWOPTION::CKSHOW : LibCmo::CK2::CK_OBJECT_SHOWOPTION::CKHIDE);
	return true;
}

#pragma endregion

