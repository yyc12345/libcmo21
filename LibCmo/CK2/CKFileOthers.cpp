#include "CKFile.hpp"
#include "CKStateChunk.hpp"
#include "ObjImpls/CKObject.hpp"
#include <cstdarg>

namespace LibCmo::CK2 {

#pragma region CKFileObject

	// CKObject is managed by CKMinContext, 
	// so we do not considering its memory leak
	// however, we need process CKStateChunk.

	CKFileObject::CKFileObject() :
		ObjectId(0u), CreatedObjectId(0u), ObjectCid(CK_CLASSID::CKCID_OBJECT),
		ObjPtr(nullptr), Name(), Data(nullptr), Options(CK_FO_OPTIONS::CK_FO_DEFAULT),
		FileIndex(0u), SaveFlags(CK_STATESAVE_ALL), PackSize(0u) {}

	CKFileObject::CKFileObject(const CKFileObject& rhs) :
		ObjectId(rhs.ObjectId), CreatedObjectId(rhs.CreatedObjectId), ObjectCid(rhs.ObjectCid),
		ObjPtr(rhs.ObjPtr), Name(rhs.Name), Data(rhs.Data), Options(rhs.Options),
		FileIndex(rhs.FileIndex), SaveFlags(rhs.SaveFlags), PackSize(rhs.PackSize) {
		if (this->Data != nullptr) {
			this->Data = new CKStateChunk(*(rhs.Data));
		}
	}

	CKFileObject::CKFileObject(CKFileObject&& rhs) :
		ObjectId(rhs.ObjectId), CreatedObjectId(rhs.CreatedObjectId), ObjectCid(rhs.ObjectCid),
		ObjPtr(rhs.ObjPtr), Name(rhs.Name), Data(rhs.Data), Options(rhs.Options),
		FileIndex(rhs.FileIndex), SaveFlags(rhs.SaveFlags), PackSize(rhs.PackSize) {
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

		this->PackSize = rhs.PackSize;
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
		
		this->PackSize = rhs.PackSize;
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
		m_Ctx(ctx), m_Visitor(this),
		m_Done(false), m_IsCopyFromReader(false),
		m_SaveIDMax(0),
		m_FileObjects(), m_ManagersData(), m_PluginsDep(), m_IncludedFiles(),
		m_FileInfo()
	{}

	CKFileWriter::CKFileWriter(CKContext* ctx, CKFileReader* reader) :
		m_Ctx(ctx), m_Visitor(this),
		m_Done(false), m_IsCopyFromReader(true),
		m_SaveIDMax(0),
		m_FileObjects(), m_ManagersData(), m_PluginsDep(), m_IncludedFiles(),
		m_FileInfo()
	{
		// sync save id max
		this->m_SaveIDMax = reader->GetSaveIdMax();

		// copy objects
		for (const auto& item : reader->GetFileObjects()) {
			CKFileObject obj;
			// copy CKObject pointer
			obj.ObjPtr = item.ObjPtr;
			// and use ctor to copy CKStateChunk
			if (item.Data == nullptr) {
				obj.Data = nullptr;
			} else {
				obj.Data = new CKStateChunk(*item.Data);
			}

			// copy other data
			obj.ObjectId = item.ObjectId;
			obj.ObjectCid = item.ObjectCid;
			obj.SaveFlags = item.SaveFlags;
			obj.Name = item.Name;

			// insert
			m_FileObjects.emplace_back(std::move(obj));
		}

		// copy managers
		for (const auto& item : reader->GetManagersData()) {
			CKFileManagerData mgr;
			// copy guid
			mgr.Manager = item.Manager;
			// copy chunk
			if (item.Data == nullptr) {
				mgr.Data = nullptr;
			} else {
				mgr.Data = new CKStateChunk(*item.Data);
			}

			// insert
			m_ManagersData.emplace_back(std::move(mgr));
		}

		// copy plugin dep
		for (const auto& item : reader->GetPluginsDep()) {
			// direct copy
			m_PluginsDep.emplace_back(item);
		}

		// copy included file
		for (const auto& item : reader->GetIncludedFiles()) {
			// direct copy
			m_IncludedFiles.emplace_back(item);
		}

	}

	CKFileWriter::~CKFileWriter() {}

	bool CKFileWriter::AddSavedObject(ObjImpls::CKObject* obj, CKDWORD flags) {
		if (m_Done || m_IsCopyFromReader) return false;
		if (obj == nullptr) return false;

		// check whether is saved.
		CK_ID objid = obj->GetID();
		if (XContainer::NSXBitArray::IsSet(m_AlreadySavedMask, static_cast<CKDWORD>(objid))) return false;

		// ok, insert this value
		m_ObjectsHashTable.try_emplace(objid, static_cast<CKDWORD>(m_FileObjects.size()));

		XContainer::NSXBitArray::Set(m_AlreadySavedMask, static_cast<CKDWORD>(objid));

		CKFileObject fobj;
		fobj.ObjectId = objid;
		fobj.ObjPtr = obj;
		fobj.ObjectCid = obj->GetClassID();
		fobj.SaveFlags = flags;
		XContainer::NSXString::FromCKSTRING(fobj.Name, obj->GetName());
		m_FileObjects.emplace_back(std::move(fobj));
		
		return true;
	}

	bool CKFileWriter::AddSavedObjects(const XContainer::XObjectPointerArray& objarray, CKDWORD flags) {
		if (m_Done || m_IsCopyFromReader) return false;
		
		bool ret = true;
		for (auto obj : objarray) {
			if (!AddSavedObject(obj, flags)) {
				ret = false;
			}
		}

		return ret;
	}

	bool CKFileWriter::AddSavedFile(CKSTRING u8FileName) {
		if (m_Done || m_IsCopyFromReader) return false;
		if (u8FileName == nullptr) return false;

		m_IncludedFiles.emplace_back(u8FileName);
		return true;
	}

#pragma endregion


#pragma region CKFileVisitor

	CKFileVisitor::CKFileVisitor(CKFileReader* reader) :
		m_IsReader(true), m_Reader(reader), m_Writer(nullptr), m_Ctx(reader->m_Ctx) {
		if (reader == nullptr) LIBPANIC("Reader is nullptr.");
	}

	CKFileVisitor::CKFileVisitor(CKFileWriter* writer) :
		m_IsReader(false), m_Reader(nullptr), m_Writer(writer), m_Ctx(writer->m_Ctx) {
		if (writer == nullptr) LIBPANIC("Writer is nullptr.");
	}

	CKFileVisitor::CKFileVisitor(const CKFileVisitor& rhs) :
		m_IsReader(rhs.m_IsReader), m_Reader(rhs.m_Reader), m_Writer(rhs.m_Writer), m_Ctx(rhs.m_Ctx) {}

	CKFileVisitor::CKFileVisitor(CKFileVisitor&& rhs) :
		m_IsReader(rhs.m_IsReader), m_Reader(rhs.m_Reader), m_Writer(rhs.m_Writer), m_Ctx(rhs.m_Ctx) {}

	CKFileVisitor& CKFileVisitor::operator=(const CKFileVisitor& rhs) {
		this->m_IsReader = rhs.m_IsReader;
		this->m_Reader = rhs.m_Reader;
		this->m_Writer = rhs.m_Writer;
		this->m_Ctx = rhs.m_Ctx;

		return *this;
	}

	CKFileVisitor& CKFileVisitor::operator=(CKFileVisitor&& rhs) {
		this->m_IsReader = rhs.m_IsReader;
		this->m_Reader = rhs.m_Reader;
		this->m_Writer = rhs.m_Writer;
		this->m_Ctx = rhs.m_Ctx;

		return *this;
	}

	const CKFileObject* CKFileVisitor::GetFileObjectByIndex(size_t index) {
		if (m_IsReader) {
			return &m_Reader->m_FileObjects[index];
		} else {
			return nullptr;
		}
	}

	CKDWORD CKFileVisitor::GetIndexByObjectID(CK_ID objid) {
		// see CKFile::SaveFindObjectIndex in IDA
		CKDWORD idx = static_cast<CKDWORD>(-1);
		if (m_IsReader) return idx;

		auto finder = m_Writer->m_ObjectsHashTable.find(objid);
		if (finder == m_Writer->m_ObjectsHashTable.end()) return idx;
		return finder->second;
	}

#pragma endregion

}
