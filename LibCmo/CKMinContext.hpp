#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"
#include "VTEncoding.hpp"
#include <filesystem>

namespace LibCmo {

	// forward decl to rm recursive reference
	namespace ObjsImpl { class CKObject; }

	class CKMinContext {
	public:
		CKMinContext();
		CKMinContext(const CKMinContext&) = delete;
		CKMinContext& operator=(const CKMinContext&) = delete;
		~CKMinContext();

		void Printf(CKSTRING fmt, ...);

		ObjsImpl::CKObject* CreateObject(CK_ID id, CKSTRING name);
		void DestroyObject(ObjsImpl::CKObject* obj);

		void GetUtf8ObjectName(std::string& native_name, std::string& u8_name);
		void GetNativeObjectName(std::string& u8_name, std::string& native_name);

		void SetEncoding(CKSTRING encoding);
		void SetTempPath(CKSTRING u8_temp);

		FILE* OpenTempFile(CKSTRING u8_filename, bool is_read);

	private:
		void RefetchEncodingToken(void);

		std::string NameEncoding;
		Encoding::ENCODING_TOKEN NameEncodingToken;
		std::filesystem::path TempFolder;
	};

}
