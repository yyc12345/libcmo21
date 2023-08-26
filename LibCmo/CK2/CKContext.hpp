#pragma once

#include "../VTAll.hpp"
#include <filesystem>
#include <map>
#include <deque>
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

		// ========== Objects Management ==========

		/**
		 * @brief Creates a CKObject or derived class instance.
		 * @param[in] cls Class Identifier (CK_CLASSID) of the object to create.
		 * @param[in] name The name of this object.
		 * @param[in] options Tell CKContext how to create this object when conflict happended.
		 * @param[out] res The value indicate the real method how this object created.
		 * @return A pointer to the newly created object.
		 * @remark CKObjects must be destroy with the DestroyObject method.
		 * @see CKObject, DestroyObject
		*/
		ObjImpls::CKObject* CreateCKObject(CK_CLASSID cls, CKSTRING name,
			CK_OBJECTCREATION_OPTIONS options = CK_OBJECTCREATION_OPTIONS::CK_OBJECTCREATION_NONAMECHECK, 
			CK_CREATIONMODE* res = nullptr);
		ObjImpls::CKObject* GetCKObject(CK_ID id);
		void DestroyCKObject(CK_ID id);

		// ========== Object Access ==========

		//CKManagerImplements::CKBaseManager* CreateCKManager(CKGUID guid);
		//CKManagerImplements::CKBaseManager* GetCKManager(CK_ID guid);
		//void DestroyCKManager(CKManagerImplements::CKBaseManager* mgr);

		//CKObject* GetObjectByName(CKSTRING name, CKObject* previous = NULL);
		//CKObject* GetObjectByNameAndClass(CKSTRING name, CK_CLASSID cid, CKObject* previous = NULL);
		//CKObject* GetObjectByNameAndParentClass(CKSTRING name, CK_CLASSID pcid, CKObject* previous);
		//const XContainer::XObjectPointerArray GetObjectListByType(CK_CLASSID cid, CKBOOL derived);
		//CKINT GetObjectsCountByClassID(CK_CLASSID cid);
		//CK_ID* GetObjectsListByClassID(CK_CLASSID cid);

		// ========== Encoding utilities ==========

		void GetUtf8String(const std::string& native_name, std::string& u8_name);
		void GetNativeString(const std::string& u8_name, std::string& native_name);
		void SetEncoding(const std::vector<std::string> encoding_series);

		// ========== Temp IO utilities ==========

		void SetTempPath(CKSTRING u8_temp);
		FILE* OpenTempFile(CKSTRING u8_filename, CKBOOL is_read);

		// ========== Print utilities ==========

		using OutputCallback = std::function<void(CKSTRING)>;
		void OutputToConsole(CKSTRING str);
		void OutputToConsoleEx(CKSTRING fmt, ...);
		void SetOutputCallback(OutputCallback cb);

	protected:
		// ========== Objects Management ==========

		XContainer::XArray<ObjImpls::CKObject*> m_ObjectsList;
		std::deque<CK_ID> m_ReturnedObjectIds;

		// ========== Encoding utilities ==========

		std::vector<EncodingHelper::ENCODING_TOKEN> m_NameEncoding;

		// ========== Temp IO utilities ==========

		std::filesystem::path m_TempFolder;

		// ========== Print utilities ==========

		OutputCallback m_OutputCallback;
	};

}
