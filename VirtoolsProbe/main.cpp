#include <CKAll.h>
#include <iostream>
#include <filesystem>
#include <string>

#define BUFFER_SIZE 1024

void Assert(bool cond, const char* desc) {
    if (!cond) {
		std::cout << desc << std::endl;
        ExitProcess(0);
    }
}

int main() {
	Assert(LoadLibrary("CK2.dll"), "Error loading CK2.dll");

	std::cout << "Press any key to run..." << std::endl;
	system("pause");

	Assert(!CKStartUp(), "CKStartUp Error");
	std::string sharedStorage;
	sharedStorage.resize(BUFFER_SIZE);
	std::filesystem::path rootPath, rePath, magPath, plgPath, bbPath;
	GetModuleFileName(NULL, sharedStorage.data(), BUFFER_SIZE);
	rootPath = sharedStorage;
	rootPath = rootPath.parent_path();
	rePath = rootPath / "RenderEngines";
	magPath = rootPath / "Managers";
	plgPath = rootPath / "Plugins";
	bbPath = rootPath / "BuildingBlocks";

	CKPluginManager* pluginManager = CKGetPluginManager();
	int k = 0;
	k += pluginManager->ParsePlugins((char*)rePath.string().c_str());
	k += pluginManager->ParsePlugins((char*)magPath.string().c_str());
	k += pluginManager->ParsePlugins((char*)plgPath.string().c_str());
	k += pluginManager->ParsePlugins((char*)bbPath.string().c_str());
	std::cout << "Total loaded modules count: " << k << std::endl;

	CKContext* ctx = NULL;
	Assert(!CKCreateContext(&ctx, NULL, 0), "Fail to execute CKCreateContext()");

	// call saver
	CKObjectArray* array = CreateCKObjectArray();
	CKObject* objs = ctx->CreateObject(CKCID_OBJECT, "fuck dassault", CK_OBJECTCREATION_NONAMECHECK, NULL);
	array->AddIfNotHere(objs);

	CKDependencies* dep = CKGetDefaultClassDependencies(CK_DEPENDENCIES_SAVE);
	dep->m_Flags = CK_DEPENDENCIES_FULL;
	Assert(!ctx->Save("result.cmo", array, 0xFFFFFFFF, dep, NULL), "Fail to save CMO file");

	DeleteCKObjectArray(array);
	CKCloseContext(ctx);
	CKShutdown();

	return 0;
}