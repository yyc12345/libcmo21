#pragma once

#include "../../VTAll.hpp"

/**
CKBaseManager virtual functions implementations help
All virtual functions is not supported except we implemented.

Read/Write functions:
- SaveData()
- LoadData()

Clear functions:
- PreClearAll()
- PostClearAll()

Delete functions:
- SequenceToBeDeleted()
- SequenceDeleted()

Moved functions
- OnCKInit(): Implement in ctor
- OnCKEnd(): Implement in dtor

*/

namespace LibCmo::CK2::MgrImpls {

	/**
	@brief Base Class for managers.
	@remark
		+ This class provides virtual methods that can be override by any managers. Any manager that inherits from CKBaseManager can override function to do some processing.
		+ The instances of managers may be retrieved through the global function CKContext::GetManagerByGuid()
		+ Some default managers implemented in Virtools can be accessed directly: See Managers Access
	*/
	class CKBaseManager {
	public:
		CKBaseManager(CKContext* ctx, CKGUID guid, CKSTRING name) :
			m_ManagerGuid(guid),
			m_ManagerName(name),
			m_Context(ctx) {}
		virtual ~CKBaseManager() {}
		LIBCMO_DISABLE_COPY_MOVE(CKBaseManager);

		/**
		@brief Acces to Manager GUID
		@return CKGUID of this manager.
		@remark
			+ Each Manager is given an unique GUID. When creating a new manager it should
			assign itself a GUID and name before registering itsef.
			```
			CKAttributeManager::CKAttributeManager(CKContext *Context) : CKBaseManager(Context, ATTRIBUTE_MANAGER_GUID, "Attribute Manager")
			{
				// ....
				// ....
				Context->RegisterNewManager(this);
			}
			```
		@see CKContext::RegisterNewManager, GetName
		*/
		CKGUID GetGuid() {
			return m_ManagerGuid;
		}
		/**
		@brief Acces to Manager name
		@return Name of this manager.
		@remark
			+ Each Manager is given an unique GUID. When creating a new manager it should
			assign itself a GUID and name before registering itsef.
			```
			CKAttributeManager::CKAttributeManager(CKContext *Context) : CKBaseManager(Context, ATTRIBUTE_MANAGER_GUID, "Attribute Manager")
			{
				// ....
				// ....
				Context->RegisterNewManager(this);
			}
			```
		*/
		CKSTRING GetName() {
			return m_ManagerName.toCKSTRING();
		}

		/**
		@brief Called to save manager data.
		@param SavedFile A pointer to the CKFile being saved.
		@return This function should return true or false if there is nothing to save.
		@remark
			+ During a save operation, each manager is given the opportunity to save its data in the file.
			+ The file being saved is given for information only and must not be modified. It can be used to decide whether it is worth saving
			data for your manager.
		@see CKStateChunk, LoadData
		*/
		virtual bool SaveData(CKStateChunk* chunk, CKFileVisitor* SavedFile) {
			return false;	// default value is false to indicate this manager do not need save any data.
		}
		/**
		@brief Called to load manager data.
		@param chunk A pointer to a CKStateChunk that was saved in the file.
		@param LoadedFile A pointer to the CKFile being loaded.
		@return CKERR_OK if successful or an error code otherwise.
		@remark
			+ During a load operation, each manager is automatically called if there was a chunk saved in the file with SaveData.
		@see CKStateChunk, SaveData
		*/
		virtual CKERROR LoadData(CKStateChunk* chunk, CKFileVisitor* LoadedFile) {
			return CKERROR::CKERR_OK;
		}

		/**
		@brief Called at the beginning of a CKContext::ClearAll operation.
		@return CK_OK if successful or an error code otherwise.
		@remark
			+ You can override this function to add specific processing at the beginning of a CKContext::ClearAll operation.
			+ You must override GetValidFunctionsMask and return a value including
			CKMANAGER_FUNC_PreClearAll for this function to get called.
		@see Main Virtools Events, PostClearAll, CKContext::ClearAll
		*/
		virtual CKERROR PreClearAll() { return CKERROR::CKERR_OK; }
		/**
		@brief Called at the end of a CKContext::ClearAll operation.
		@return CKERR_OK if successful or an error code otherwise.
		@remark
			+ You can override this function to add specific processing at the end of a CKContext::ClearAll operation.
			+ You must override GetValidFunctionsMask and return a value including
			CKMANAGER_FUNC_PostClearAll for this function to get called.
		@see Main Virtools Events, PreClearAll, CKContext::ClearAll
		*/
		virtual CKERROR PostClearAll() { return CKERROR::CKERR_OK; }

		/**
		@brief Called just before objects are deleted.
		@return CK_OK if successful or an error code otherwise.
		@param objids[in] A pointer to a list of CK_ID of the objects being deleted.
		@param count[in] number of objects in objids list.
		@remark
			+ You can override this function if you need to add specific processing before objects are deleted.
			+ You must override GetValidFunctionsMask and return a value including
			CKMANAGER_FUNC_OnSequenceToBeDeleted for this function to get called.
		@see Main Virtools Events, CKContext::DestroyObjects, SequenceDeleted
		*/
		virtual CKERROR SequenceToBeDeleted(const CK_ID* objids, CKDWORD count) { return CKERROR::CKERR_OK; }
		/**
		@brief Called after objects have been deleted.
		@return CK_OK if successful or an error code otherwise.
		@param objids[in] A pointer to a list of CK_ID of the objects being deleted.
		@param count[in] number of objects in objids list.
		@remark
			+ You can override this function if you need to add specific processing after objects have been deleted.
			+ You must override GetValidFunctionsMask and return a value including
			CKMANAGER_FUNC_OnSequenceDeleted for this function to get called.
		@see Main Virtools Events, CKContext::DestroyObjects, SequenceToBeDeleted
		*/
		virtual CKERROR SequenceDeleted(const CK_ID* objids, CKDWORD count) { return CKERROR::CKERR_OK; }


	protected:
		CKGUID m_ManagerGuid; ///> Manager GUID
		TypeHelper::MKString m_ManagerName; ///> Manager Name
		CKContext* m_Context; ///> A pointer to the CKContext on which this manager is valid.

	};

	//class CKAttributeManager : public CKBaseManager {
	//public:
	//	CKAttributeManager(CKContext* ctx, CK_ID ckid);
	//	CKAttributeManager(const CKAttributeManager&) = delete;
	//	CKAttributeManager& operator=(const CKAttributeManager&) = delete;
	//	virtual ~CKAttributeManager();

	//private:

	//};

}
