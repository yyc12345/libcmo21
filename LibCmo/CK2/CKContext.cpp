#include "CKContext.hpp"
#include "ObjImpls/CKObject.hpp"
#include <cstdarg>

namespace LibCmo::CK2 {

#if defined(LIBCMO_OS_WIN32)
	static wchar_t g_UniqueFolder[] = L"LibCmo";
#else
	static char g_UniqueFolder[] = "LibCmo";
#endif

#pragma region Objects Management

	ObjImpls::CKObject* CKContext::CreateCKObject(CK_CLASSID cls, CKSTRING name,
		CK_OBJECTCREATION_OPTIONS options, CK_CREATIONMODE* res) {
		// todo: Process paramter options and res

		// get description first
		const CKClassDesc* desc = CKGetClassDesc(cls);
		if (desc == nullptr) return nullptr;

		// allocate a CK_ID first
		CK_ID decided_id;
		if (this->m_ReturnedObjectIds.empty()) {
			// create new CK_ID.
			decided_id = m_ObjectsList.size();
			m_ObjectsList.resize(decided_id + 1);
		} else {
			// use returned CK_ID.
			decided_id = m_ReturnedObjectIds.back();
			m_ReturnedObjectIds.pop_back();
		}

		// create one
		ObjImpls::CKObject* obj = desc->CreationFct(this, decided_id, name);

		// put into slot
		m_ObjectsList[decided_id] = obj;
		
		// set out variable
		return obj;
	}

	ObjImpls::CKObject* CKContext::GetCKObject(CK_ID id) {
		if (id >= m_ObjectsList.size()) return nullptr;
		return m_ObjectsList[id];
	}

	/**
	 * @brief The real CKObject destroy worker shared by CKContext::DestroyCKObject and CKContext::~CKContext
	 * @param[in] ctx The CKContext
	 * @param[in] obj The CKObject need to be free.
	*/
	static void InternalDestroy(CKContext* ctx, ObjImpls::CKObject* obj) {
		// find desc by classid
		// if really we can not find it, we only can delete it directly.
		const CKClassDesc* desc = CKGetClassDesc(obj->GetClassID());
		if (desc == nullptr) {
			delete obj;
			return;
		}

		// free it and return its value
		desc->ReleaseFct(ctx, obj);
	}
	void CKContext::DestroyCKObject(CK_ID id) {
		if (id >= m_ObjectsList.size()) return;

		// get object and free it
		ObjImpls::CKObject* obj = m_ObjectsList[id];
		if (obj == nullptr) return;
		InternalDestroy(this, obj);
		
		// return its allocated id.
		m_ObjectsList[id] = nullptr;
		m_ReturnedObjectIds.emplace_back(id);

	}

	void CKContext::DestroyAllCKObjects() {
		// free all created objects
		for (auto& ptr : m_ObjectsList) {
			if (ptr != nullptr) {
				InternalDestroy(this, ptr);
			}
		}
		// restore returned object list
		m_ReturnedObjectIds.clear();
		// empty object list
		m_ObjectsList.clear();
	}

#pragma endregion

#pragma region Ctor Dtor

	CKContext::CKContext() :
		m_ObjectsList(), m_ReturnedObjectIds(),
		m_NameEncoding(), m_TempFolder(),
		m_OutputCallback(nullptr)
	{
		// preset for temp folder
		// todo: add current CKContext pointer as the part of temp path.
		// thus multiple CKContext can work.
		m_TempFolder = std::filesystem::temp_directory_path();
		m_TempFolder /= g_UniqueFolder;
		std::filesystem::create_directory(m_TempFolder);
	}

	CKContext::~CKContext() {
		DestroyAllCKObjects();
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

#pragma region Temp IO utilities

	void CKContext::SetTempPath(CKSTRING u8_temp) {
		EncodingHelper::SetStdPathFromU8Path(this->m_TempFolder, u8_temp);
	}

	FILE* CKContext::OpenTempFile(CKSTRING u8_filename, CKBOOL is_read) {
		std::filesystem::path stdfilename;
		EncodingHelper::SetStdPathFromU8Path(stdfilename, u8_filename);

		auto realfile = this->m_TempFolder / stdfilename;
		return EncodingHelper::OpenStdPathFile(realfile, is_read);
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