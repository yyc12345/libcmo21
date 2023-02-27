#include "VTUtils.hpp"
#include "CKMinContext.hpp"
#include "CKObjects.hpp"
#include "CKManagers.hpp"

namespace LibCmo::CK2 {

#if defined(LIBCMO_OS_WIN32)
	static wchar_t g_UniqueFolder[] = L"LibCmo";
#else
	static char g_UniqueFolder[] = "LibCmo";
#endif


	CKMinContext::CKMinContext() :
		m_NameEncoding(), m_NameEncodingToken(EncodingHelper::ENCODING_TOKEN_DEFAULT),
		m_TempFolder(),
		m_PrintCallback(nullptr),
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

	}

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

	void CKMinContext::GetUtf8String(std::string& native_name, std::string& u8_name) {
		EncodingHelper::GetUtf8VirtoolsName(native_name, u8_name, this->m_NameEncodingToken);
	}

	void CKMinContext::GetNativeString(std::string& u8_name, std::string& native_name) {
		EncodingHelper::GetNativeVirtoolsName(u8_name, native_name, this->m_NameEncodingToken);
	}

	void CKMinContext::SetEncoding(CKSTRING encoding) {
		this->m_NameEncoding = encoding;
		this->RefetchEncodingToken();
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

	void CKMinContext::RefetchEncodingToken(void) {
		EncodingHelper::DestroyEncodingToken(this->m_NameEncodingToken);
		this->m_NameEncodingToken = EncodingHelper::CreateEncodingToken(this->m_NameEncoding);
	}

}