#include "CKContext.hpp"
#include "ObjImpls/CKObject.hpp"
#include "MgrImpls/CKBaseManager.hpp"
#include "MgrImpls/CKObjectManager.hpp"
#include "MgrImpls/CKPathManager.hpp"
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
		m_GlobalImagesSaveFormat(),
		// misc init
		m_NameEncoding(),
		m_OutputCallback(nullptr) {

		// setup save format
		m_GlobalImagesSaveFormat.m_Ext.SetExt(u8"bmp");

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
		// free encoding
		this->ClearEncoding();
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
		return static_cast<CKDWORD>(m_ManagerList.size());
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

	const CKBitmapProperties& CKContext::GetGlobalImagesSaveFormat() {
		return m_GlobalImagesSaveFormat;
	}

	void CKContext::SetGlobalImagesSaveFormat(const CKBitmapProperties& Format) {
		m_GlobalImagesSaveFormat = Format;
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
		if (str == nullptr) return;
		m_OutputCallback(str);
	}

	void CKContext::OutputToConsoleEx(CKSTRING fmt, ...) {
		if (m_OutputCallback == nullptr) return;
		if (fmt == nullptr) return;

		va_list argptr;
		va_start(argptr, fmt);
		XContainer::XString result;
		YYCC::StringHelper::VPrintf(fmt, argptr);
		va_end(argptr);

		// use c_str(), not XContainer::NSXString::ToCKSTRING because we want make sure this paramter is not nullptr.
		// we always output a valid C style string, even if no chars need to write.
		m_OutputCallback(result.c_str());
	}

	void CKContext::SetOutputCallback(OutputCallback cb) {
		m_OutputCallback = cb;
	}

#pragma endregion

#pragma region Encoding utilities

	void CKContext::GetUTF8String(const std::string& native_name, XContainer::XString& u8_name) {
		bool conv_success = false, has_valid_token = false;
		for (const auto& token : this->m_NameEncoding) {
			if (token == EncodingHelper::INVALID_ENCODING_TOKEN) continue;
			has_valid_token = true;
			conv_success = EncodingHelper::ToUTF8(native_name, u8_name, token);
			if (conv_success) break;
		}
		// fallback if failed.
		if (!conv_success) {
			if (!has_valid_token) {
				throw RuntimeException("Try to get UTF8 string from ordinary string in CKContext but giving empty encoding candidate.");
			} else {
				u8_name.clear();
				this->OutputToConsole(u8"Error when converting to UTF8 string from ordinary string. The string will leave to blank.");
			}
		}
	}

	void CKContext::GetOrdinaryString(const XContainer::XString& u8_name, std::string& native_name) {
		bool conv_success = false, has_valid_token = false;
		for (const auto& token : this->m_NameEncoding) {
			if (token == EncodingHelper::INVALID_ENCODING_TOKEN) continue;
			has_valid_token = true;
			conv_success = EncodingHelper::ToOrdinary(u8_name, native_name, token);
			if (conv_success) break;
		}
		// fallback if failed.
		if (!conv_success) {
			if (!has_valid_token) {
				throw RuntimeException("Try to get ordinary string from UTF8 string in CKContext but giving empty encoding candidate.");
			} else {
				native_name.clear();
				this->OutputToConsole(u8"Error when converting to ordinary string from UTF8 string. The string will leave to blank.");
			}
		}
	}

	void CKContext::SetEncoding(const XContainer::XArray<XContainer::XString>& encoding_seq) {
		// free all current series
		this->ClearEncoding();
		// add new encoding
		for (const auto& encoding_str : encoding_seq) {
			this->m_NameEncoding.emplace_back(LibCmo::EncodingHelper::CreateEncodingToken(encoding_str));
		}
	}

	void CKContext::ClearEncoding() {
		for (const auto& token : this->m_NameEncoding) {
			if (token == EncodingHelper::INVALID_ENCODING_TOKEN) continue;
			LibCmo::EncodingHelper::DestroyEncodingToken(token);
		}
		this->m_NameEncoding.clear();
	}

	bool CKContext::IsValidEncoding() {
		for (const auto& token : this->m_NameEncoding) {
			if (token != EncodingHelper::INVALID_ENCODING_TOKEN) return true;
		}
		return false;
	}

#pragma endregion

}