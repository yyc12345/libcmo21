#include "VTUtils.hpp"
#include "CKMinContext.hpp"
#include "CKObjects.hpp"
#include "CKManagers.hpp"
#include <cstdarg>

namespace LibCmo::CK2 {

#if defined(LIBCMO_OS_WIN32)
	static wchar_t g_UniqueFolder[] = L"LibCmo";
#else
	static char g_UniqueFolder[] = "LibCmo";
#endif

#pragma region Ctor Dtor

	CKMinContext::CKMinContext() :
		m_NameEncoding(), m_TempFolder(),
		m_PrintCallback(nullptr),
		m_CKObjectMaxID(0u),
		// register CKObjects
		m_ObjectsCreationMap{
			{CK_CLASSID::CKCID_OBJECT, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKObject(ctx, id, name); })},
			{CK_CLASSID::CKCID_SCENEOBJECT, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKSceneObject(ctx, id, name); })},
			{CK_CLASSID::CKCID_BEOBJECT, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKBeObject(ctx, id, name); })},
			{CK_CLASSID::CKCID_GROUP, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKGroup(ctx, id, name); })},
			{CK_CLASSID::CKCID_MESH, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKMesh(ctx, id, name); })},
			{CK_CLASSID::CKCID_TEXTURE, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKTexture(ctx, id, name); })},
			{CK_CLASSID::CKCID_MATERIAL, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKMaterial(ctx, id, name); })},
			{CK_CLASSID::CKCID_RENDEROBJECT, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKRenderObject(ctx, id, name); })},
			{CK_CLASSID::CKCID_3DENTITY, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CK3dEntity(ctx, id, name); })},
			{CK_CLASSID::CKCID_3DOBJECT, ([](CKMinContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CK3dObject(ctx, id, name); })}
	},
		// register CKBaseManagers
		m_ManagersCreationMap{
			{PredefinedGuids::ATTRIBUTE_MANAGER_GUID, ([](CKMinContext* ctx, CK_ID id) ->CKManagerImplements::CKBaseManager* { return new(std::nothrow) CKManagerImplements::CKAttributeManager(ctx, id); })},
	}
	{
		// preset for temp folder
		m_TempFolder = std::filesystem::temp_directory_path();
		m_TempFolder /= g_UniqueFolder;
		std::filesystem::create_directory(m_TempFolder);
	}
	CKMinContext::~CKMinContext() {
		// free all created objects
		for (const auto& [key, value] : this->m_ObjectsList) {
			delete value;
		}
		// todo: free all created managers

	}

#pragma endregion

#pragma region Print

	void CKMinContext::Printf(CKSTRING fmt, ...) {
		if (m_PrintCallback == nullptr) return;

		va_list argptr;
		va_start(argptr, fmt);

		std::string result;
		int count = std::vsnprintf(nullptr, 0, fmt, argptr);
		result.resize(count);
		int write_result = std::vsnprintf(result.data(), count, fmt, argptr);

		if (write_result < 0 || write_result > count) return;

		va_end(argptr);

		m_PrintCallback(result);
	}

	void CKMinContext::SetPrintCallback(PrintCallback cb) {
		m_PrintCallback = cb;
	}

#pragma endregion

#pragma region Objects

	CKObjectImplements::CKObject* CKMinContext::CreateCKObject(CK_ID id, CK_CLASSID cls, CKSTRING name) {
		// pick creation function
		const auto& creation = m_ObjectsCreationMap.find(cls);
		if (creation == m_ObjectsCreationMap.end()) return nullptr;

		// check ckid
		if (m_ObjectsList.contains(id)) return nullptr;
		
		// create it
		return creation->second(this, id, name);
	}

	CKObjectImplements::CKObject* CKMinContext::GetCKObject(CK_ID id) {
		const auto& probe = m_ObjectsList.find(id);
		if (probe == m_ObjectsList.end()) return nullptr;
		else return probe->second;
	}

	void CKMinContext::DestroyCKObject(CK_ID id) {
		const auto& probe = m_ObjectsList.find(id);
		if (probe != m_ObjectsList.end()) {
			delete (probe->second);
			m_ObjectsList.erase(probe);
		}
	}

	void CKMinContext::ClearCKObject(void) {
		// free all created objects
		for (const auto& [key, value] : this->m_ObjectsList) {
			delete value;
		}
		// clear list
		this->m_ObjectsList.clear();
	}

	CK_ID CKMinContext::GetObjectMaxID(void) {
		return this->m_CKObjectMaxID;
	}

	void CKMinContext::SetObjectMaxID(CK_ID id) {
		this->m_CKObjectMaxID = id;
	}

#pragma endregion

#pragma region Managers

#pragma endregion

#pragma region Misc Funcs

	void CKMinContext::GetUtf8String(const std::string& native_name, std::string& u8_name) {
		bool success = false;
		for (const auto& token : this->m_NameEncoding) {
			success = LibCmo::EncodingHelper::GetUtf8VirtoolsName(native_name, u8_name, token);
			if (success) break;
		}

		// fallback
		if (!success) {
			u8_name = native_name;
			this->Printf("Error when converting to UTF8 string.");
		}
	}

	void CKMinContext::GetNativeString(const std::string& u8_name, std::string& native_name) {
		bool success = false;
		for (const auto& token : this->m_NameEncoding) {
			success = LibCmo::EncodingHelper::GetNativeVirtoolsName(u8_name, native_name, token);
			if (success) break;
		}
		
		// fallback
		if (!success) {
			native_name = u8_name;
			this->Printf("Error when converting to native string.");
		}
	}

	void CKMinContext::SetEncoding(const std::vector<std::string> encoding_series) {
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

	void CKMinContext::SetTempPath(CKSTRING u8_temp) {
		EncodingHelper::SetStdPathFromU8Path(this->m_TempFolder, u8_temp);
	}

	FILE* CKMinContext::OpenTempFile(CKSTRING u8_filename, bool is_read) {
		std::filesystem::path stdfilename;
		EncodingHelper::SetStdPathFromU8Path(stdfilename, u8_filename);

		auto realfile = this->m_TempFolder / stdfilename;
		return EncodingHelper::OpenStdPathFile(realfile, is_read);
	}


#pragma endregion

}