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
