#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"
#include "VTEncoding.hpp"
#include <filesystem>
#include <map>
#include <functional>

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
		void DestroyCKObject(CK_ID id);

		//CKManagerImplements::CKBaseManager* CreateCKManager(CKGUID guid);
		//CKManagerImplements::CKBaseManager* GetCKManager(CK_ID guid);
		//void DestroyCKManager(CKManagerImplements::CKBaseManager* mgr);

		CK_ID GetObjectMaxID(void);
		void SetObjectMaxID(CK_ID id);

		void GetUtf8String(std::string& native_name, std::string& u8_name);
		void GetNativeString(std::string& u8_name, std::string& native_name);

		void SetEncoding(CKSTRING encoding);
		void SetTempPath(CKSTRING u8_temp);

		FILE* OpenTempFile(CKSTRING u8_filename, bool is_read);

	private:
		void RefetchEncodingToken(void);

		std::map<CK_ID, CKObjectImplements::CKObject*> m_ObjectsList;
		std::map<CK_ID, CKManagerImplements::CKBaseManager*> m_ManagersList;

		std::map<CK_CLASSID, std::function<CKObjectImplements::CKObject* (CKMinContext*, CK_ID, CKSTRING)>> m_ObjectsCreationMap;
		std::map<CKGUID, std::function<CKManagerImplements::CKBaseManager* (CKMinContext*, CK_ID)>> m_ManagersCreationMap;

		CK_ID m_CKObjectMaxID;

		std::string m_NameEncoding;
		EncodingHelper::ENCODING_TOKEN m_NameEncodingToken;
		std::filesystem::path m_TempFolder;
		PrintCallback m_PrintCallback;
	};

}
