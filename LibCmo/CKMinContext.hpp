#pragma once

#include "CKDefines.hpp"
#include "CKObjects.hpp"
#include "VTEncoding.hpp"
#include <filesystem>

namespace LibCmo::CK2 {

	class CKMinContext {
	public:
		CKMinContext();
		CKMinContext(const CKMinContext&) = delete;
		CKMinContext& operator=(const CKMinContext&) = delete;
		~CKMinContext();

		using PrintCallback = void (*)(std::string&);
		void Printf(CKSTRING fmt, ...);
		void SetPrintCallback(PrintCallback cb);

		CKObjectImplements::CKObject* CreateCKObject(CK_ID id, CK_CLASSID cls, CKSTRING name);
		CKObjectImplements::CKObject* GetCKObject(CK_ID id);
		void DestroyCKObject(CKObjectImplements::CKObject* obj);

		CKManagerImplements::CKBaseManager* CreateCKManager(CKGUID guid);
		CKManagerImplements::CKBaseManager* GetCKManager(CKGUID guid);
		void DestroyCKManager(CKManagerImplements::CKBaseManager* mgr);

		void GetUtf8String(std::string& native_name, std::string& u8_name);
		void GetNativeString(std::string& u8_name, std::string& native_name);

		void SetEncoding(CKSTRING encoding);
		void SetTempPath(CKSTRING u8_temp);

		FILE* OpenTempFile(CKSTRING u8_filename, bool is_read);

	private:
		void RefetchEncodingToken(void);

		std::string m_NameEncoding;
		EncodingHelper::ENCODING_TOKEN m_NameEncodingToken;
		std::filesystem::path m_TempFolder;
		PrintCallback m_PrintCallback;
	};

}
