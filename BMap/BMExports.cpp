#include "BMExports.hpp"
#include <IronPad.hpp>
#include <set>

static std::set<BMap::BMFile*> g_AllBMFiles = std::set<BMap::BMFile*>();

void BMInit() {
	// register IronPad
	IronPad::IronPadRegister();
	// and startup CK environment
	LibCmo::CK2::CKStartUp();
}

void BMDispose() {
	// free all existed file reader / writer
	for (auto ptr : g_AllBMFiles) {
		delete ptr;
	}
	g_AllBMFiles.clear();
	// shutdown CK environment
	LibCmo::CK2::CKShutdown();
	// unregister iron pad
	IronPad::IronPadUnregister();
}

BMap::BMFile* BMFile_Load(LibCmo::CKSTRING file_name, LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[]) {
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
