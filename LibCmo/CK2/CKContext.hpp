#pragma once

#include "../VTAll.hpp"
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
		~CKContext();
		LIBCMO_DISABLE_COPY_MOVE(CKContext);
		
		// ========== Engine runtime ==========
	public:
		/**
		 * @brief Simply clear all CKContext to restore its status.
		*/
		void ClearAll();

		// ========== Objects Management / Access ==========
		// These functions is a simply redirect to CKObjectManager

		ObjImpls::CKObject* CreateObject(CK_CLASSID cls, CKSTRING name,
			CK_OBJECTCREATION_OPTIONS options = CK_OBJECTCREATION_OPTIONS::CK_OBJECTCREATION_NONAMECHECK,
			CK_CREATIONMODE* res = nullptr);

		ObjImpls::CKObject* GetObject(CK_ID ObjID);
		CKDWORD GetObjectCount();
		void DestroyObject(ObjImpls::CKObject *obj);
		void DestroyObject(CK_ID id);
		void DestroyObjects(CK_ID* obj_ids, CKDWORD Count);

		ObjImpls::CKObject* GetObjectByName(CKSTRING name, ObjImpls::CKObject *previous = nullptr);
		ObjImpls::CKObject* GetObjectByNameAndClass(CKSTRING name, CK_CLASSID cid, ObjImpls::CKObject *previous = nullptr);
		ObjImpls::CKObject* GetObjectByNameAndParentClass(CKSTRING name, CK_CLASSID pcid, ObjImpls::CKObject* previous = nullptr);
		const XContainer::XObjectPointerArray GetObjectListByType(CK_CLASSID cid, bool derived);
		CKDWORD GetObjectsCountByClassID(CK_CLASSID cid);
		CK_ID* GetObjectsListByClassID(CK_CLASSID cid);

	protected:
		XContainer::XObjectPointerArray m_ObjectPointerCache;
		XContainer::XObjectArray m_ObjectCache;

		// ========== Common Managers ==========
	public:
		MgrImpls::CKObjectManager* GetObjectManager();
		MgrImpls::CKPathManager* GetPathManager();

		CKDWORD GetManagerCount();
		MgrImpls::CKBaseManager* GetManager(CKDWORD index);

		void ExecuteManagersOnPreClearAll();
		void ExecuteManagersOnPostClearAll();
		void ExecuteManagersOnSequenceToBeDeleted(const CK_ID* objids, CKDWORD count);
		void ExecuteManagersOnSequenceDeleted(const CK_ID* objids, CKDWORD count);

	protected:
		void ExecuteManagersGeneral(std::function<void(MgrImpls::CKBaseManager*)> fct);
		XContainer::XArray<MgrImpls::CKBaseManager*> m_ManagerList;

		MgrImpls::CKObjectManager* m_ObjectManager;
		MgrImpls::CKPathManager* m_PathManager;

		// ========== File Save/Load Options ==========
	public:
		void SetCompressionLevel(CKINT level);
		CKINT GetCompressionLevel();

		void SetFileWriteMode(CK_FILE_WRITEMODE mode);
		CK_FILE_WRITEMODE GetFileWriteMode();

		CK_TEXTURE_SAVEOPTIONS GetGlobalImagesSaveOptions();
		void SetGlobalImagesSaveOptions(CK_TEXTURE_SAVEOPTIONS Options);

		const CKBitmapProperties& GetGlobalImagesSaveFormat();
		void SetGlobalImagesSaveFormat(const CKBitmapProperties& Format);

		CK_SOUND_SAVEOPTIONS GetGlobalSoundsSaveOptions();
		void SetGlobalSoundsSaveOptions(CK_SOUND_SAVEOPTIONS Options);
		
	protected:
		CKINT m_CompressionLevel;
		CK_FILE_WRITEMODE m_FileWriteMode;
		CK_TEXTURE_SAVEOPTIONS m_GlobalImagesSaveOptions;
		CK_SOUND_SAVEOPTIONS m_GlobalSoundsSaveOptions;
		CKBitmapProperties m_GlobalImagesSaveFormat;

		// ========== Encoding utilities ==========
	public:
		void GetUtf8String(const XContainer::XString& native_name, XContainer::XString& u8_name);
		void GetNativeString(const XContainer::XString& u8_name, XContainer::XString& native_name);
		void SetEncoding(const XContainer::XArray<XContainer::XString>& encoding_series);
		
	protected:
		XContainer::XArray<EncodingHelper::ENCODING_TOKEN> m_NameEncoding;

		// ========== Print utilities ==========
	public:
		using OutputCallback = std::function<void(CKSTRING)>;
		void OutputToConsole(CKSTRING str);
		void OutputToConsoleEx(CKSTRING fmt, ...);
		void SetOutputCallback(OutputCallback cb);

	protected:
		OutputCallback m_OutputCallback;
	};

}
