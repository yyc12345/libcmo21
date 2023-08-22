#pragma once

#include "../VTAll.hpp"
#include <filesystem>
#include <map>
#include <functional>

namespace LibCmo::CK2 {

	/**
	@brief Main Interface Object
	@details
		+ The CKContext object is the heart of all Virtools based applications, It is the first object that should be created in order to
		use Virtools SDK. A CKContext can be simply created by calling its constructor.
		+ The CKContext object act as the central interface to create/destroy objects,to access managers, to load/save files.
		+ Several CKContext can be created inside a same process (in multiple threads for example) but objects created
		by a specific CKContext must not be used in other contextes.
	@see CKContext::CreateObject, CKContext::GetObject, CKContext::DestroyObject
	*/
	class CKContext {
	public:
		CKContext();
		CKContext(const CKContext&) = delete;
		CKContext& operator=(const CKContext&) = delete;
		~CKContext();

		using PrintCallback = std::function<void(const std::string&)>;
		void Printf(CKSTRING fmt, ...);
		void SetPrintCallback(PrintCallback cb);

		/**
		 * @brief Creates a CKObject or derived class instance.
		 * @param[in] cls Class Identifier (CK_CLASSID) of the object to create.
		 * @param[in] name The name of this object.
		 * @return A pointer to the newly created object.
		 * @remark CKObjects must be destroy with the DestroyObject method.
		 * @see CKObject, DestroyObject
		*/
		CKObject* CreateCKObject(CK_CLASSID cls, CKSTRING name);
		CKObject* GetCKObject(CK_ID id);
		void DestroyCKObject(CK_ID id);

		//CKManagerImplements::CKBaseManager* CreateCKManager(CKGUID guid);
		//CKManagerImplements::CKBaseManager* GetCKManager(CK_ID guid);
		//void DestroyCKManager(CKManagerImplements::CKBaseManager* mgr);

		CK_ID GetObjectMaxID(void);
		void SetObjectMaxID(CK_ID id);

		void GetUtf8String(const std::string& native_name, std::string& u8_name);
		void GetNativeString(const std::string& u8_name, std::string& native_name);

		void SetEncoding(const std::vector<std::string> encoding_series);
		void SetTempPath(CKSTRING u8_temp);

		FILE* OpenTempFile(CKSTRING u8_filename, bool is_read);

	public:
		std::map<CK_ID, CKObject*> m_ObjectsList;
		std::map<CK_ID, CKBaseManager*> m_ManagersList;

		std::map<CK_CLASSID, std::function<CKObject* (CKContext*, CK_ID, CKSTRING)>> m_ObjectsCreationMap;
		std::map<CKGUID, std::function<CKBaseManager* (CKContext*, CK_ID)>> m_ManagersCreationMap;

		CK_ID m_CKObjectMaxID;

		std::vector<EncodingHelper::ENCODING_TOKEN> m_NameEncoding;
		std::filesystem::path m_TempFolder;
		PrintCallback m_PrintCallback;
	};

}
