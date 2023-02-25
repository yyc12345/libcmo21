#include <CKAll.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <Windows.h>
#include <Psapi.h>

#define BUFFER_SIZE 1024

void Assert(bool cond, const char* desc) {
    if (!cond) {
		std::cout << desc << std::endl;
        ExitProcess(0);
    }
}

struct MyModuleInfo {
	intptr_t BaseOfDll;
	intptr_t EntryPoint;
	std::string ModuleName;
};

std::map<intptr_t, MyModuleInfo> ConstructModuleList() {
	std::map<intptr_t, MyModuleInfo> result;

	HANDLE hProc = GetCurrentProcess();
	HMODULE* hMods = new HMODULE[1024];
	TCHAR* szModName = new TCHAR[MAX_PATH];
	MODULEINFO infoMod;
	DWORD cbNeeded;
	DWORD nameLen;

	if (EnumProcessModules(hProc, hMods, sizeof(HMODULE) * 1024, &cbNeeded)) {
		for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); ++i) {
			MyModuleInfo data;

			nameLen = GetModuleBaseName(hProc, hMods[i], szModName, MAX_PATH);
			if (nameLen) {
				data.ModuleName.resize(nameLen);
				memcpy(data.ModuleName.data(), szModName, nameLen * sizeof(TCHAR));
			} else data.ModuleName = "";

			if (GetModuleInformation(hProc, hMods[i], &infoMod, sizeof(infoMod))) {
				data.BaseOfDll = reinterpret_cast<intptr_t>(infoMod.lpBaseOfDll);
				data.EntryPoint = reinterpret_cast<intptr_t>(infoMod.EntryPoint);
			} else {
				data.BaseOfDll = 0;
				data.EntryPoint = 0;
			}

			result.emplace(data.BaseOfDll, data);
		}
	}

	delete[] hMods;
	delete[] szModName;
	return result;
}

int main() {
	Assert(LoadLibrary("CK2.dll"), "Error loading CK2.dll");

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

	std::cout << "Press any key to run..." << std::endl;
	system("pause");

	static std::vector<CK_CLASSID> cls{
		CKCID_OBJECT,
		CKCID_PARAMETERIN,
		CKCID_PARAMETEROPERATION,
		CKCID_STATE,
		CKCID_BEHAVIORLINK,
		CKCID_BEHAVIOR,
		CKCID_BEHAVIORIO,
		CKCID_RENDERCONTEXT,
		CKCID_KINEMATICCHAIN,
		CKCID_SCENEOBJECT,
		CKCID_OBJECTANIMATION,
		CKCID_ANIMATION,
		CKCID_KEYEDANIMATION,
		CKCID_BEOBJECT,
		CKCID_DATAARRAY,
		CKCID_SCENE,
		CKCID_LEVEL,
		CKCID_PLACE,
		CKCID_GROUP,
		CKCID_SOUND,
		CKCID_WAVESOUND,
		CKCID_MIDISOUND,
		CKCID_MATERIAL,
		CKCID_TEXTURE,
		CKCID_MESH,
		CKCID_PATCHMESH,
		CKCID_RENDEROBJECT,
		CKCID_2DENTITY,
		CKCID_SPRITE,
		CKCID_SPRITETEXT,
		CKCID_3DENTITY,
		CKCID_GRID,
		CKCID_CURVEPOINT,
		CKCID_SPRITE3D,
		CKCID_CURVE,
		CKCID_CAMERA,
		CKCID_TARGETCAMERA,
		CKCID_LIGHT,
		CKCID_TARGETLIGHT,
		CKCID_CHARACTER,
		CKCID_3DOBJECT,
		CKCID_BODYPART,
		CKCID_PARAMETER,
		CKCID_PARAMETERLOCAL,
		CKCID_PARAMETERVARIABLE,
		CKCID_PARAMETEROUT,
		CKCID_INTERFACEOBJECTMANAGER,
		CKCID_CRITICALSECTION,
		CKCID_LAYER,
		CKCID_PROGRESSIVEMESH,
		CKCID_SYNCHRO
	};
	static std::vector<const char*> clsname{
		"CKCID_OBJECT",
		"CKCID_PARAMETERIN",
		"CKCID_PARAMETEROPERATION",
		"CKCID_STATE",
		"CKCID_BEHAVIORLINK",
		"CKCID_BEHAVIOR",
		"CKCID_BEHAVIORIO",
		"CKCID_RENDERCONTEXT",
		"CKCID_KINEMATICCHAIN",
		"CKCID_SCENEOBJECT",
		"CKCID_OBJECTANIMATION",
		"CKCID_ANIMATION",
		"CKCID_KEYEDANIMATION",
		"CKCID_BEOBJECT",
		"CKCID_DATAARRAY",
		"CKCID_SCENE",
		"CKCID_LEVEL",
		"CKCID_PLACE",
		"CKCID_GROUP",
		"CKCID_SOUND",
		"CKCID_WAVESOUND",
		"CKCID_MIDISOUND",
		"CKCID_MATERIAL",
		"CKCID_TEXTURE",
		"CKCID_MESH",
		"CKCID_PATCHMESH",
		"CKCID_RENDEROBJECT",
		"CKCID_2DENTITY",
		"CKCID_SPRITE",
		"CKCID_SPRITETEXT",
		"CKCID_3DENTITY",
		"CKCID_GRID",
		"CKCID_CURVEPOINT",
		"CKCID_SPRITE3D",
		"CKCID_CURVE",
		"CKCID_CAMERA",
		"CKCID_TARGETCAMERA",
		"CKCID_LIGHT",
		"CKCID_TARGETLIGHT",
		"CKCID_CHARACTER",
		"CKCID_3DOBJECT",
		"CKCID_BODYPART",
		"CKCID_PARAMETER",
		"CKCID_PARAMETERLOCAL",
		"CKCID_PARAMETERVARIABLE",
		"CKCID_PARAMETEROUT",
		"CKCID_INTERFACEOBJECTMANAGER",
		"CKCID_CRITICALSECTION",
		"CKCID_LAYER",
		"CKCID_PROGRESSIVEMESH",
		"CKCID_SYNCHRO"
	};
	
	auto moduleInfos = ConstructModuleList();

	fputs("Class Name,Class Id,Show(),IsHiddenByParent(),CanBeHide(),IsVisible(),~dtor(),GetClassID(),PreSave(),Save(),Load(),PostLoad(),PreDelete(),CheckPreDeletion(),CheckPostDeletion(),GetMemoryOccupation(),IsObjectUsed(),PrepareDependencies(),RemapDependencies(),Copy(),\n", stdout);
	for (size_t j = 0; j < cls.size(); ++j) {
		CK_CLASSID item = cls[j];
		const char* itemname = clsname[j];

		CKObject* obj = ctx->CreateObject(item, NULL, CK_OBJECTCREATION_NONAMECHECK, NULL);
		if (obj == nullptr) continue;
		CKDWORD* vtable = *(reinterpret_cast<CKDWORD**>(obj));

		fprintf(stdout, "%s,%d,", itemname, item);
		for (size_t i = 0; i < 18; ++i) {
			intptr_t addr = vtable[i];
			const auto& it = moduleInfos.lower_bound(addr);
			if (it != moduleInfos.end() && it != moduleInfos.begin()) {
				const auto& itt = std::prev(it);
				fprintf(stdout, "%s::", itt->second.ModuleName.c_str());
				addr = addr - itt->second.BaseOfDll;

				if (itt->second.ModuleName == "CK2_3D.dll") 
					addr += 0x10000000u;
				else if (itt->second.ModuleName == "CK2.dll") 
					addr += 0x24000000u;
			}
			fprintf(stdout, "%08X,", addr);
		}
		fputc('\n', stdout);
	}

	// call reader
	//CKObjectArray* array = CreateCKObjectArray();
	//Assert(!ctx->Load("Language.old.nmo", array, CK_LOAD_DEFAULT, NULL), "Fail to load CMO file");

	// call saver
	//CKObjectArray* array = CreateCKObjectArray();
	//CKObject* objs = ctx->CreateObject(CKCID_OBJECT, "fuck dassault", CK_OBJECTCREATION_NONAMECHECK, NULL);
	//array->AddIfNotHere(objs);

	//CKDependencies* dep = CKGetDefaultClassDependencies(CK_DEPENDENCIES_SAVE);
	//dep->m_Flags = CK_DEPENDENCIES_FULL;
	//Assert(!ctx->Save("result.cmo", array, 0xFFFFFFFF, dep, NULL), "Fail to save CMO file");

	//DeleteCKObjectArray(array);


	std::cout << "Press any key to exit..." << std::endl;
	system("pause");

	CKCloseContext(ctx);
	CKShutdown();

	return 0;
}