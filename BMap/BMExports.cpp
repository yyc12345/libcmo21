#include "BMExports.hpp"
#include <IronPad.hpp>
#include <set>

static bool g_IsInited = false;
static std::set<BMap::BMFile*> g_AllBMFiles = std::set<BMap::BMFile*>();
static std::set<BMap::BMMeshTransition*> g_AllBMMeshTrans = std::set<BMap::BMMeshTransition*>();

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
	if (!g_IsInited) return nullptr;
	auto file = new BMap::BMFile(temp_folder, texture_folder, encoding_count, encodings, false);
	if (file->IsFailed()) {
		delete file;
		file = nullptr;
	}
	if (!file->Load(file_name)) {
		delete file;
		file = nullptr;
	}

	g_AllBMFiles.emplace(file);
	return file;
}

BMap::BMFile* BMFile_Create(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[]) {
	if (!g_IsInited) return nullptr;
	auto file = new BMap::BMFile(temp_folder, texture_folder, encoding_count, encodings, false);
	if (file->IsFailed()) {
		delete file;
		file = nullptr;
	}

	g_AllBMFiles.emplace(file);
	return file;
}

bool BMFile_Save(BMap::BMFile* map_file, LibCmo::CKSTRING file_name, LibCmo::CKINT compreess_level) {
	if (!g_AllBMFiles.contains(map_file)) return false;
	return map_file->Save(file_name, compreess_level);
}

void BMFile_Free(BMap::BMFile* map_file) {
	// only free correct pointer
	if (map_file == nullptr) return;
	if (g_AllBMFiles.erase(map_file) != 0) {
		delete map_file;
	}
}

#define VISITOR_IMPL(namepart, cidpart) \
LibCmo::CKDWORD BMFile_Get ## namepart ## Count(BMap::BMFile* map_file) { \
	if (!g_AllBMFiles.contains(map_file)) return 0; \
	return map_file->Get ## namepart ## Count(); \
} \
LibCmo::CK2::ObjImpls::CK ## namepart * BMFile_Get ## namepart (BMap::BMFile* map_file, LibCmo::CKDWORD idx) { \
	if (!g_AllBMFiles.contains(map_file)) return nullptr; \
	return map_file->Get ## namepart (idx); \
} \
LibCmo::CK2::ObjImpls::CK ## namepart * BMFile_Create ## namepart (BMap::BMFile* map_file, LibCmo::CKSTRING name) { \
	if (!g_AllBMFiles.contains(map_file)) return nullptr; \
	return map_file->Create ## namepart (name); \
}

VISITOR_IMPL(Group, GROUP)
VISITOR_IMPL(3dObject, 3DOBJECT)
VISITOR_IMPL(Mesh, MESH)
VISITOR_IMPL(Material, MATERIAL)
VISITOR_IMPL(Texture, TEXTURE)

#undef VISITOR_IMPL

#pragma endregion

#pragma region BMMeshTransition

BMap::BMMeshTransition* BMMeshTrans_New() {
	if (!g_IsInited) return nullptr;
	auto meshtrans = new BMap::BMMeshTransition();
	g_AllBMMeshTrans.emplace(meshtrans);
	return meshtrans;
}

void BMMeshTrans_Delete(BMap::BMMeshTransition* trans) {
	// only free correct pointer
	if (trans == nullptr) return;
	if (g_AllBMMeshTrans.erase(trans) != 0) {
		delete trans;
	}
}

void BMMeshTrans_PrepareVertexCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!g_AllBMMeshTrans.contains(trans)) return;
	trans->PrepareVertexCount(count);
}
LibCmo::VxMath::VxVector3* BMMeshTrans_PrepareVertex(BMap::BMMeshTransition* trans) {
	if (!g_AllBMMeshTrans.contains(trans)) return nullptr;
	return trans->PrepareVertex();
}
void BMMeshTrans_PrepareNormalCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!g_AllBMMeshTrans.contains(trans)) return;
	trans->PrepareNormalCount(count);
}
LibCmo::VxMath::VxVector3* BMMeshTrans_PrepareNormal(BMap::BMMeshTransition* trans) {
	if (!g_AllBMMeshTrans.contains(trans)) return nullptr;
	return trans->PrepareNormal();
}
void BMMeshTrans_PrepareUVCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!g_AllBMMeshTrans.contains(trans)) return;
	trans->PrepareUVCount(count);
}
LibCmo::VxMath::VxVector2* BMMeshTrans_PrepareUV(BMap::BMMeshTransition* trans) {
	if (!g_AllBMMeshTrans.contains(trans)) return nullptr;
	return trans->PrepareUV();
}
void BMMeshTrans_PrepareMtlSlotCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!g_AllBMMeshTrans.contains(trans)) return;
	trans->PrepareMtlSlotCount(count);
}
LibCmo::CK2::ObjImpls::CKMaterial** BMMeshTrans_PrepareMtlSlot(BMap::BMMeshTransition* trans) {
	if (!g_AllBMMeshTrans.contains(trans)) return nullptr;
	return trans->PrepareMtlSlot();
}
void BMMeshTrans_PrepareFaceCount(BMap::BMMeshTransition* trans, LibCmo::CKDWORD count) {
	if (!g_AllBMMeshTrans.contains(trans)) return;
	trans->PrepareFaceCount(count);
}
LibCmo::CKDWORD* BMMeshTrans_PrepareFaceVertexIndices(BMap::BMMeshTransition* trans) {
	if (!g_AllBMMeshTrans.contains(trans)) return nullptr;
	return trans->PrepareFaceVertexIndices();
}
LibCmo::CKDWORD* BMMeshTrans_PrepareFaceNormalIndices(BMap::BMMeshTransition* trans) {
	if (!g_AllBMMeshTrans.contains(trans)) return nullptr;
	return trans->PrepareFaceNormalIndices();
}
LibCmo::CKDWORD* BMMeshTrans_PrepareFaceUVIndices(BMap::BMMeshTransition* trans) {
	if (!g_AllBMMeshTrans.contains(trans)) return nullptr;
	return trans->PrepareFaceUVIndices();
}
LibCmo::CKDWORD* BMMeshTrans_PrepareFaceMtlSlot(BMap::BMMeshTransition* trans) {
	if (!g_AllBMMeshTrans.contains(trans)) return nullptr;
	return trans->PrepareFaceMtlSlot();
}
bool BMMeshTrans_Parse(BMap::BMMeshTransition* trans, LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh) {
	if (!g_AllBMMeshTrans.contains(trans)) return false;
	return trans->Parse(write_into_mesh);
}

#pragma endregion

