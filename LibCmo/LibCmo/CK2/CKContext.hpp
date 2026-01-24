#pragma once

#include "../VTInternal.hpp"
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
		YYCC_DEL_CLS_COPY_MOVE(CKContext);
		
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
		/**
		 * @brief Convert given ordinary string to UTF8 string.
		 * @param[in] native_name The input ordinary string.
		 * @param[out] u8_name The output UTF8 string.
		 * @return True if convertion is success, otherwise false.
		 * @exception RuntimeException Raised when perform this operation with a blank encoding sequence.
		 * @remarks
		 * The encoding of ordinary is specified by encoding sequence.
		 * If we fail to do convertion, the result will leave to blank and output a message to CKContext.
		 * However, if you use this function with blank encoding sequence, it will raise exception.
		 * So becore using this function, please make sure that you have checked by calling IsValidEncoding().
		*/
		bool GetUTF8String(const std::string& native_name, XContainer::XString& u8_name);
		/**
		 * @brief Convert given UTF8 string to ordinary string.
		 * @param[in] u8_name The input UTF8 string.
		 * @param[out] native_name The output ordinary string.
		 * @return True if convertion is success, otherwise false.
		 * @exception RuntimeException Raised when perform this operation with a blank encoding sequence.
		 * @remarks
		 * The encoding of ordinary is specified by encoding sequence.
		 * If we fail to do convertion, the result will leave to blank and output a message to CKContext.
		 * However, if you use this function with blank encoding sequence, it will raise exception.
		 * So becore using this function, please make sure that you have checked by calling IsValidEncoding().
		*/
		bool GetOrdinaryString(const XContainer::XString& u8_name, std::string& native_name);
		/**
		 * @brief Set the encoding sequence.
		 * @param[in] encoding_series The encoding name in this sequence.
		 * @remarks
		 * \li The order in encoding sequence is important. The encoding name with lower index will be used for convertion first.
		 * \li Encoding sequence will be used for performing GetUTF8String() and GetOrdinaryString().
		 * We will try using it to do convertion from top to bottom (if one failed we will continue trying to use next one to do convertion).
		*/
		void SetEncoding(const XContainer::XArray<XContainer::XString>& encoding_series);
		/**
		 * @brief Clear specified encoding sequence.
		*/
		void ClearEncoding();
		/**
		 * @brief Check whether current encoding sequence at least has one valid encoding for convertion.
		 * @return True if it is, otherwise false.
		*/
		bool IsValidEncoding();
		
	protected:
		XContainer::XArray<EncodingHelper::EncodingToken> m_NameEncoding;

		// ========== Print utilities ==========
	public:
		/**
		 * @brief The callback prototype.
		 * @details It accept a CKSTRING representing the string need to be printed.
		 * The passed CKSTRING is guaranteen that it can not be nullptr.
		*/
		using OutputCallback = std::function<void(CKSTRING)>;
		/**
		 * @brief Output plain message.
		 * @param[in] str Plain message. nullptr is allowed but not suggested.
		*/
		void OutputToConsole(CKSTRING str);
		/**
		 * @brief Output message with given format.
		 * @param[in] fmt The format string. nullptr is allowed but not suggested.
		 * @param[in] ... The arguments of format string.
		*/
		void OutputToConsoleEx(CKSTRING fmt, ...);
		/**
		 * @brief Set the callback for message printing.
		 * @param[in] cb The function pointer to callback. nullptr to remove callback.
		*/
		void SetOutputCallback(OutputCallback cb);

	protected:
		OutputCallback m_OutputCallback;
	};

}
