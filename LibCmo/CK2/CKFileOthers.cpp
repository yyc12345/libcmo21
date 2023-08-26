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

		return *this;
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

		return *this;
	}

	CKFileManagerData::~CKFileManagerData() {
		if (Data != nullptr) delete Data;
	}

#pragma endregion

#pragma region CKFileReader

	CKFileReader::CKFileReader(CKContext* ctx) :
		m_Ctx(ctx), m_Visitor(this),
		m_Done(false),
		m_SaveIDMax(0),
		m_FileObjects(), m_ManagersData(), m_PluginsDep(), m_IncludedFiles(),
		m_FileInfo() {}

	CKFileReader::~CKFileReader() {}

	CKINT CKFileReader::GetSaveIdMax() {
		return m_SaveIDMax;
	}

	const XContainer::XArray<CKFileObject>& CKFileReader::GetFileObjects() {
		return m_FileObjects;
	}

	const XContainer::XArray<CKFileManagerData>& CKFileReader::GetManagersData() {
		return m_ManagersData;
	}

	const XContainer::XArray<CKFilePluginDependencies>& CKFileReader::GetPluginsDep() {
		return m_PluginsDep;
	}

	const XContainer::XArray<XContainer::XString>& CKFileReader::GetIncludedFiles() {
		return m_IncludedFiles;
	}

	const CKFileInfo CKFileReader::GetFileInfo() {
		return m_FileInfo;
	}

#pragma endregion

#pragma region CKFileWriter

	CKFileWriter::CKFileWriter(CKContext* ctx) :
		m_Ctx(ctx), m_Visitor(this) {}

	CKFileWriter::CKFileWriter(CKContext* ctx, CKFileReader* reader) :
		m_Ctx(ctx), m_Visitor(this) {}

	CKFileWriter::~CKFileWriter() {}

	CKBOOL CKFileWriter::AddSavedObject(ObjImpls::CKObject* obj, CKDWORD flags) {
		return CKFALSE;
	}

	CKBOOL CKFileWriter::AddSavedObjects(CKObjectArray* objarray, CKDWORD flags) {
		return CKFALSE;
	}

	CKBOOL CKFileWriter::AddSavedFile(CKSTRING u8FileName) {
		return CKFALSE;
	}

#pragma endregion


#pragma region CKFileVisitor

	CKFileVisitor::CKFileVisitor(CKFileReader* reader) :
		m_IsReader(CKTRUE), m_Reader(reader), m_Writer(nullptr) {
		if (reader == nullptr) LIBPANIC("Reader is nullptr.");
	}

	CKFileVisitor::CKFileVisitor(CKFileWriter* writer) :
		m_IsReader(false), m_Reader(nullptr), m_Writer(nullptr) {
		if (writer == nullptr) LIBPANIC("Writer is nullptr.");
	}

	CKFileVisitor::CKFileVisitor(const CKFileVisitor& rhs) :
		m_IsReader(rhs.m_IsReader), m_Reader(rhs.m_Reader), m_Writer(rhs.m_Writer) {}

	CKFileVisitor::CKFileVisitor(CKFileVisitor&& rhs) :
		m_IsReader(rhs.m_IsReader), m_Reader(rhs.m_Reader), m_Writer(rhs.m_Writer) {}

	CKFileVisitor& CKFileVisitor::operator=(const CKFileVisitor& rhs) {
		this->m_IsReader = rhs.m_IsReader;
		this->m_Reader = rhs.m_Reader;
		this->m_Writer = rhs.m_Writer;

		return *this;
	}

	CKFileVisitor& CKFileVisitor::operator=(CKFileVisitor&& rhs) {
		this->m_IsReader = rhs.m_IsReader;
		this->m_Reader = rhs.m_Reader;
		this->m_Writer = rhs.m_Writer;

		return *this;
	}

	const CKFileObject* CKFileVisitor::GetFileObjectByIndex(size_t index) {
		if (m_IsReader) {
			return &m_Reader->m_FileObjects[index];
		} else {
			return nullptr;
		}
	}

#pragma endregion

}
