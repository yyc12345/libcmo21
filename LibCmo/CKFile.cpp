#include "CKFile.hpp"
#include "CKStateChunk.hpp"
#include "CKMinContext.hpp"
#include <cstdarg>

namespace LibCmo::CK2 {

#pragma region CKBufferParser

	CKBufferParser::CKBufferParser(void* ptr, size_t rsize, bool need_manual_free) :
		m_MemBegin(static_cast<char*>(ptr)),
		m_MemPos(0u), m_MemSize(rsize),
		m_NeedManualFree(need_manual_free) {
		;
	}
	CKBufferParser::~CKBufferParser() {
		if (this->m_NeedManualFree) delete[](this->m_MemBegin);
	}

#pragma endregion

#pragma region CKFileInfo

	CKFileInfo::CKFileInfo() :
		ProductVersion(0u), ProductBuild(0x01010000u), FileWriteMode(CK_FILE_WRITEMODE::CKFILE_UNCOMPRESSED),
		FileVersion(8u), CKVersion(CKVERSION), FileSize(0u),
		ObjectCount(0u), ManagerCount(0u), MaxIDSaved(0u), Crc(0u),
		Hdr1PackSize(0u), Hdr1UnPackSize(0u), DataPackSize(0u), DataUnPackSize(0u) {
	}

	CKFileInfo::CKFileInfo(const CKFileInfo& rhs) :
		ProductVersion(rhs.ProductVersion), ProductBuild(rhs.ProductBuild), FileWriteMode(rhs.FileWriteMode),
		FileVersion(rhs.FileVersion), CKVersion(rhs.CKVersion), FileSize(rhs.FileSize),
		ObjectCount(rhs.ObjectCount), ManagerCount(rhs.ManagerCount), MaxIDSaved(rhs.MaxIDSaved), Crc(rhs.Crc),
		Hdr1PackSize(rhs.Hdr1PackSize), Hdr1UnPackSize(rhs.Hdr1UnPackSize),
		DataPackSize(rhs.DataPackSize), DataUnPackSize(rhs.DataUnPackSize) {
	}

	CKFileInfo& CKFileInfo::operator=(const CKFileInfo& rhs) {
		this->ProductVersion = rhs.ProductVersion;
		this->ProductBuild = rhs.ProductBuild;
		this->FileWriteMode = rhs.FileWriteMode;
		this->FileVersion = rhs.FileVersion;
		this->CKVersion = rhs.CKVersion;
		this->FileSize = rhs.FileSize;
		this->ObjectCount = rhs.ObjectCount;
		this->ManagerCount = rhs.ManagerCount;
		this->MaxIDSaved = rhs.MaxIDSaved;
		this->Crc = rhs.Crc;
		this->Hdr1PackSize = rhs.Hdr1PackSize;
		this->Hdr1UnPackSize = rhs.Hdr1UnPackSize;
		this->DataPackSize = rhs.DataPackSize;
		this->DataUnPackSize = rhs.DataUnPackSize;

		return *this;
	}

	CKFileInfo::~CKFileInfo() {
	}

#pragma endregion

#pragma region CKFileObject

	CKFileObject::CKFileObject() :
		ObjectId(0u), ObjectCid(CK_CLASSID::CKCID_OBJECT), Name(),
		Data(nullptr), FileIndex(0u) {
	}

	CKFileObject::CKFileObject(const CKFileObject& rhs) :
		ObjectId(rhs.ObjectId), ObjectCid(rhs.ObjectCid), Name(rhs.Name),
		Data(rhs.Data), FileIndex(rhs.FileIndex) {

		if (this->Data != nullptr) {
			this->Data = new(std::nothrow) CKStateChunk(*(rhs.Data));
		}

	}

	CKFileObject& CKFileObject::operator=(const CKFileObject& rhs) {
		this->ObjectId = rhs.ObjectId;
		this->ObjectCid = rhs.ObjectCid;
		this->Name = rhs.Name;
		this->FileIndex = rhs.FileIndex;

		this->Data = rhs.Data;
		if (this->Data != nullptr) {
			this->Data = new(std::nothrow) CKStateChunk(*(rhs.Data));
		}

		return *this;
	}

	CKFileObject::~CKFileObject() {
		if (Data != nullptr) delete Data;
	}

#pragma endregion

#pragma region CKFileManagerData

	CKFileManagerData::CKFileManagerData() :
		Data(nullptr), Manager(0u, 0u) {
	}

	CKFileManagerData::CKFileManagerData(const CKFileManagerData& rhs) :
		Data(rhs.Data), Manager(rhs.Manager) {

		if (this->Data != nullptr) {
			this->Data = new(std::nothrow) CKStateChunk(*(rhs.Data));
		}

	}

	CKFileManagerData& CKFileManagerData::operator=(const CKFileManagerData& rhs) {
		this->Manager = rhs.Manager;

		this->Data = rhs.Data;
		if (this->Data != nullptr) {
			this->Data = new(std::nothrow) CKStateChunk(*(rhs.Data));
		}

		return *this;
	}

	CKFileManagerData::~CKFileManagerData() {
		if (Data != nullptr) delete Data;
	}

#pragma endregion

#pragma region CKFilePluginDependencies

	CKFilePluginDependencies::CKFilePluginDependencies() :
		m_PluginCategory(CK_PLUGIN_TYPE::CKPLUGIN_MANAGER_DLL), m_Guids() {
	}

	CKFilePluginDependencies::CKFilePluginDependencies(const CKFilePluginDependencies& rhs) :
		m_PluginCategory(rhs.m_PluginCategory), m_Guids(rhs.m_Guids) {
	}

	CKFilePluginDependencies& CKFilePluginDependencies::operator=(const CKFilePluginDependencies& rhs) {
		this->m_PluginCategory = rhs.m_PluginCategory;
		this->m_Guids = rhs.m_Guids;

		return *this;
	}

	CKFilePluginDependencies::~CKFilePluginDependencies() {
	}

#pragma endregion

	namespace CKFileData {

#pragma region ShallowDocument

		ShallowDocument::ShallowDocument() {
			/*this->m_IndexByClassId.resize(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));*/
		}

		ShallowDocument::~ShallowDocument() {

		}

#pragma endregion

#pragma region DeepDocument

		DeepDocument::DeepDocument() {
			/*this->m_IndexByClassId.resize(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));*/
		}

		DeepDocument::~DeepDocument() {

		}

#pragma endregion

	}

#pragma region CKFile Misc

	CKFile::CKFile(CKMinContext* ctx) :
		m_FileName(), m_MinCtx(ctx) {
		;
	}

	CKFile::~CKFile() {
	}


#pragma endregion


}
