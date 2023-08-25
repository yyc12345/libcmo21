#include "CKFile.hpp"
#include "CKStateChunk.hpp"
#include <cstdarg>

namespace LibCmo::CK2 {

#pragma region CKFileObject

	// CKObject is managed by CKMinContext, 
	// so we do not considering its memory leak
	// however, we need process CKStateChunk.

	CKFileObject::CKFileObject() :
		ObjectId(0u), CreatedObjectId(0u), ObjectCid(CK_CLASSID::CKCID_OBJECT),
		ObjPtr(nullptr), Name(), Data(nullptr), Options(CK_FO_OPTIONS::CK_FO_DEFAULT),
		FileIndex(0u), SaveFlags(0u) {}

	CKFileObject::CKFileObject(const CKFileObject& rhs) :
		ObjectId(rhs.ObjectId), CreatedObjectId(rhs.CreatedObjectId), ObjectCid(rhs.ObjectCid),
		ObjPtr(rhs.ObjPtr), Name(rhs.Name), Data(rhs.Data), Options(rhs.Options),
		FileIndex(rhs.FileIndex), SaveFlags(0u) {
		if (this->Data != nullptr) {
			this->Data = new CKStateChunk(*(rhs.Data));
		}
	}

	CKFileObject::CKFileObject(CKFileObject&& rhs) :
		ObjectId(rhs.ObjectId), CreatedObjectId(rhs.CreatedObjectId), ObjectCid(rhs.ObjectCid),
		ObjPtr(rhs.ObjPtr), Name(rhs.Name), Data(rhs.Data), Options(rhs.Options),
		FileIndex(rhs.FileIndex), SaveFlags(0u) {
		rhs.Data = nullptr;
	}

	CKFileObject& CKFileObject::operator=(const CKFileObject& rhs) {
		this->ObjectId = rhs.ObjectId;
		this->CreatedObjectId = rhs.CreatedObjectId;
		this->ObjectCid = rhs.ObjectCid;
		this->ObjPtr = rhs.ObjPtr;
		this->Name = rhs.Name;

		this->Data = rhs.Data;
		if (this->Data != nullptr) {
			this->Data = new CKStateChunk(*(rhs.Data));
		}

		this->Options = rhs.Options;
		this->FileIndex = rhs.FileIndex;
		this->SaveFlags = rhs.SaveFlags;

		return *this;
	}

	CKFileObject& CKFileObject::operator=(CKFileObject&& rhs) {
		this->ObjectId = rhs.ObjectId;
		this->CreatedObjectId = rhs.CreatedObjectId;
		this->ObjectCid = rhs.ObjectCid;
		this->ObjPtr = rhs.ObjPtr;
		this->Name = rhs.Name;

		this->Data = rhs.Data;
		rhs.Data = nullptr;

		this->Options = rhs.Options;
		this->FileIndex = rhs.FileIndex;
		this->SaveFlags = rhs.SaveFlags;

	}

	CKFileObject::~CKFileObject() {
		if (Data != nullptr) delete Data;
	}

#pragma endregion

#pragma region CKFileManagerData

	CKFileManagerData::CKFileManagerData() :
		Data(nullptr), Manager(0u, 0u) {}

	CKFileManagerData::CKFileManagerData(const CKFileManagerData& rhs) :
		Data(rhs.Data), Manager(rhs.Manager) {

		if (this->Data != nullptr) {
			this->Data = new CKStateChunk(*(rhs.Data));
		}

	}

	CKFileManagerData::CKFileManagerData(CKFileManagerData&& rhs) :
		Data(rhs.Data), Manager(rhs.Manager) {
		rhs.Data = nullptr;
	}

	CKFileManagerData& CKFileManagerData::operator=(const CKFileManagerData& rhs) {
		this->Manager = rhs.Manager;

		this->Data = rhs.Data;
		if (this->Data != nullptr) {
			this->Data = new CKStateChunk(*(rhs.Data));
		}

		return *this;
	}

	CKFileManagerData& CKFileManagerData::operator=(CKFileManagerData&& rhs) {
		this->Manager = rhs.Manager;

		this->Data = rhs.Data;
		rhs.Data = nullptr;
	}

	CKFileManagerData::~CKFileManagerData() {
		if (Data != nullptr) delete Data;
	}

#pragma endregion

#pragma region CKFilePluginDependencies

	CKFilePluginDependencies::CKFilePluginDependencies() :
		m_PluginCategory(CK_PLUGIN_TYPE::CKPLUGIN_MANAGER_DLL), m_Guids() {}

	CKFilePluginDependencies::CKFilePluginDependencies(const CKFilePluginDependencies& rhs) :
		m_PluginCategory(rhs.m_PluginCategory), m_Guids(rhs.m_Guids) {}

	CKFilePluginDependencies& CKFilePluginDependencies::operator=(const CKFilePluginDependencies& rhs) {
		this->m_PluginCategory = rhs.m_PluginCategory;
		this->m_Guids = rhs.m_Guids;

		return *this;
	}

	CKFilePluginDependencies::~CKFilePluginDependencies() {}

#pragma endregion

#pragma region CKFile Misc

	CKFile::CKFile(CKContext* ctx) :
		m_Ctx(ctx) {
		;
	}

	CKFile::~CKFile() {}


#pragma endregion


}
