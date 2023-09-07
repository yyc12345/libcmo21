#include "CKContext.hpp"
#include "ObjImpls/CKObject.hpp"
#include "MgrImpls/CKBaseManager.hpp"
#include "MgrImpls/CKObjectManager.hpp"
#include "MgrImpls/CKPathManager.hpp"
#include "../XContainer/XBitArray.hpp"
#include <cstdarg>

namespace LibCmo::CK2 {

#pragma region Ctor Dtor

	CKContext::CKContext() :
		// setup manager
		m_ManagerList(),
		m_ObjectManager(nullptr), m_PathManager(nullptr),
		// setup object cache
		m_ObjectCache(), m_ObjectPointerCache(),
		// setup file save/load options
		m_CompressionLevel(5),
		m_FileWriteMode(CK_FILE_WRITEMODE::CKFILE_UNCOMPRESSED),
		m_GlobalImagesSaveOptions(CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_RAWDATA),
		m_GlobalSoundsSaveOptions(CK_SOUND_SAVEOPTIONS::CKSOUND_EXTERNAL),
		m_GlobalImagesSaveFormat(nullptr),	// todo: setup save format
		// misc init
		m_NameEncoding(),
		m_OutputCallback(nullptr) {

		// setup managers
		m_ObjectManager = new MgrImpls::CKObjectManager(this);
		m_ManagerList.emplace_back(m_ObjectManager);
		m_PathManager = new MgrImpls::CKPathManager(this);
		m_ManagerList.emplace_back(m_PathManager);
	}

	CKContext::~CKContext() {
		// reset context
		ClearAll();
		// free all manager
		for (auto& mgrptr : m_ManagerList) {
			delete mgrptr;
		}
	}

#pragma endregion

#pragma region Engine runtime

	void CKContext::ClearAll() {
		// pre clear all
		ExecuteManagersOnPreClearAll();

		// order object manager clear all objects
		m_ObjectManager->DestroyAllObjects();

		// post clear all
		ExecuteManagersOnPostClearAll();
	}

#pragma endregion

#pragma region Objects Management / Access

	ObjImpls::CKObject* CKContext::CreateObject(CK_CLASSID cls, CKSTRING name, CK_OBJECTCREATION_OPTIONS options, CK_CREATIONMODE* res) {
		return m_ObjectManager->CreateObject(cls, name, options, res);
	}

	ObjImpls::CKObject* CKContext::GetObject(CK_ID ObjID) {
		return m_ObjectManager->GetObject(ObjID);
	}

	CKDWORD CKContext::GetObjectCount() {
		return m_ObjectManager->GetObjectCount();
	}

	void CKContext::DestroyObject(ObjImpls::CKObject* obj) {
		CK_ID id = obj->GetID();
		return m_ObjectManager->DestroyObjects(&id, 1);
	}

	void CKContext::DestroyObject(CK_ID id) {
		return m_ObjectManager->DestroyObjects(&id, 1);
	}

	void CKContext::DestroyObjects(CK_ID* obj_ids, CKDWORD Count) {
		return m_ObjectManager->DestroyObjects(obj_ids, Count);
	}

	ObjImpls::CKObject* GeneralPrevFinder(XContainer::XObjectPointerArray& objptrs, ObjImpls::CKObject* previous) {
		if (objptrs.empty()) return nullptr;

		if (previous == nullptr) {
			return objptrs.front();
		} else {
			auto finder = std::find(objptrs.begin(), objptrs.end(), previous);
			if (finder == objptrs.end()) return nullptr;
			++finder;
			if (finder == objptrs.end()) return nullptr;
			return *finder;
		}
	}
	ObjImpls::CKObject* CKContext::GetObjectByName(CKSTRING name, ObjImpls::CKObject* previous) {
		if (name == nullptr) return nullptr;
		if (previous == nullptr) {
			m_ObjectPointerCache = m_ObjectManager->GetObjectByNameAndClass(name, CK_CLASSID::CKCID_OBJECT, true);
		}
		return GeneralPrevFinder(m_ObjectPointerCache, previous);
	}

	ObjImpls::CKObject* CKContext::GetObjectByNameAndClass(CKSTRING name, CK_CLASSID cid, ObjImpls::CKObject* previous) {
		if (name == nullptr) return nullptr;
		if (previous == nullptr) {
			m_ObjectPointerCache = m_ObjectManager->GetObjectByNameAndClass(name, cid, false);
		}
		return GeneralPrevFinder(m_ObjectPointerCache, previous);
	}

	ObjImpls::CKObject* CKContext::GetObjectByNameAndParentClass(CKSTRING name, CK_CLASSID pcid, ObjImpls::CKObject* previous) {
		if (name == nullptr) return nullptr;
		if (previous == nullptr) {
			m_ObjectPointerCache = m_ObjectManager->GetObjectByNameAndClass(name, pcid, true);
		}
		return GeneralPrevFinder(m_ObjectPointerCache, previous);
	}

	const XContainer::XObjectPointerArray CKContext::GetObjectListByType(CK_CLASSID cid, bool derived) {
		return m_ObjectManager->GetObjectByNameAndClass(nullptr, cid, derived);
	}

	CKDWORD CKContext::GetObjectsCountByClassID(CK_CLASSID cid) {
		auto result = m_ObjectManager->GetObjectByNameAndClass(nullptr, cid, false);

		m_ObjectCache.clear();
		for (auto& obj : result) {
			m_ObjectCache.emplace_back(obj->GetID());
		}

		return static_cast<CKDWORD>(m_ObjectCache.size());
	}

	CK_ID* CKContext::GetObjectsListByClassID(CK_CLASSID cid) {
		return m_ObjectCache.data();
	}

#pragma endregion

#pragma region Common Managers

	MgrImpls::CKObjectManager* CKContext::GetObjectManager() {
		return m_ObjectManager;
	}

	MgrImpls::CKPathManager* CKContext::GetPathManager() {
		return m_PathManager;
	}

	CKDWORD CKContext::GetManagerCount() {
		return m_ManagerList.size();
	}

	MgrImpls::CKBaseManager* CKContext::GetManager(CKDWORD index) {
		if (index >= m_ManagerList.size()) return nullptr;
		return m_ManagerList[index];
	}

	void CKContext::ExecuteManagersOnPreClearAll() {
		ExecuteManagersGeneral([](MgrImpls::CKBaseManager* mgr) -> void {
			mgr->PreClearAll();
			});
	}

	void CKContext::ExecuteManagersOnPostClearAll() {
		ExecuteManagersGeneral([](MgrImpls::CKBaseManager* mgr) -> void {
			mgr->PostClearAll();
			});
	}

	void CKContext::ExecuteManagersOnSequenceToBeDeleted(const CK_ID* objids, CKDWORD count) {
		ExecuteManagersGeneral([objids, count](MgrImpls::CKBaseManager* mgr) -> void {
			mgr->SequenceToBeDeleted(objids, count);
			});
	}

	void CKContext::ExecuteManagersOnSequenceDeleted(const CK_ID* objids, CKDWORD count) {
		ExecuteManagersGeneral([objids, count](MgrImpls::CKBaseManager* mgr) -> void {
			mgr->SequenceDeleted(objids, count);
			});
	}

	void CKContext::ExecuteManagersGeneral(std::function<void(MgrImpls::CKBaseManager*)> fct) {
		for (auto& mgrptr : m_ManagerList) {
			fct(mgrptr);
		}
	}

#pragma endregion

#pragma region File Save/Load Options

	void CKContext::SetCompressionLevel(CKINT level) {
		if (level > 0 && level < 10) {
			m_CompressionLevel = level;
		}
	}

	CKINT CKContext::GetCompressionLevel() {
		return m_CompressionLevel;
	}

	void CKContext::SetFileWriteMode(CK_FILE_WRITEMODE mode) {
		m_FileWriteMode = mode;
	}

	CK_FILE_WRITEMODE CKContext::GetFileWriteMode() {
		return m_FileWriteMode;
	}

	CK_TEXTURE_SAVEOPTIONS CKContext::GetGlobalImagesSaveOptions() {
		return m_GlobalImagesSaveOptions;
	}

	void CKContext::SetGlobalImagesSaveOptions(CK_TEXTURE_SAVEOPTIONS Options) {
		if (Options != CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_USEGLOBAL) {
			m_GlobalImagesSaveOptions = Options;
		}
	}

	const CKBitmapProperties* CKContext::GetGlobalImagesSaveFormat() {
		return m_GlobalImagesSaveFormat;
	}

	void CKContext::SetGlobalImagesSaveFormat(const CKBitmapProperties* Format) {
		// todo: copy CKBitmapProperties
	}

	CK_SOUND_SAVEOPTIONS CKContext::GetGlobalSoundsSaveOptions() {
		return m_GlobalSoundsSaveOptions;
	}

	void CKContext::SetGlobalSoundsSaveOptions(CK_SOUND_SAVEOPTIONS Options) {
		if (Options != CK_SOUND_SAVEOPTIONS::CKSOUND_USEGLOBAL) {
			m_GlobalSoundsSaveOptions = Options;
		}
	}


#pragma endregion

#pragma region Output utilities

	void CKContext::OutputToConsole(CKSTRING str) {
		if (m_OutputCallback == nullptr) return;
		m_OutputCallback(str);
	}

	void CKContext::OutputToConsoleEx(CKSTRING fmt, ...) {
		if (m_OutputCallback == nullptr) return;

		va_list argptr;
		va_start(argptr, fmt);

		std::string result;
		int count = std::vsnprintf(nullptr, 0, fmt, argptr);
		result.resize(count);
		int write_result = std::vsnprintf(result.data(), count, fmt, argptr);

		if (write_result < 0 || write_result > count) return;

		va_end(argptr);

		m_OutputCallback(result.c_str());
	}

	void CKContext::SetOutputCallback(OutputCallback cb) {
		m_OutputCallback = cb;
	}

#pragma endregion

#pragma region Encoding utilities

	void CKContext::GetUtf8String(const std::string& native_name, std::string& u8_name) {
		bool success = false;
		for (const auto& token : this->m_NameEncoding) {
			success = LibCmo::EncodingHelper::GetUtf8VirtoolsName(native_name, u8_name, token);
			if (success) break;
		}

		// fallback
		if (!success) {
			u8_name = native_name;
			this->OutputToConsole("Error when converting to UTF8 string.");
		}
	}

	void CKContext::GetNativeString(const std::string& u8_name, std::string& native_name) {
		bool success = false;
		for (const auto& token : this->m_NameEncoding) {
			success = LibCmo::EncodingHelper::GetNativeVirtoolsName(u8_name, native_name, token);
			if (success) break;
		}

		// fallback
		if (!success) {
			native_name = u8_name;
			this->OutputToConsole("Error when converting to native string.");
		}
	}

	void CKContext::SetEncoding(const std::vector<std::string> encoding_series) {
		// free all current series
		for (const auto& encoding : this->m_NameEncoding) {
			LibCmo::EncodingHelper::DestroyEncodingToken(encoding);
		}
		this->m_NameEncoding.clear();

		// add new encoding
		for (const auto& encoding_str : encoding_series) {
			this->m_NameEncoding.push_back(LibCmo::EncodingHelper::CreateEncodingToken(encoding_str));
		}
	}



#pragma endregion

}