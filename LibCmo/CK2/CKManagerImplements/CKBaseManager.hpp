#pragma once

#include "../../VTAll.hpp"

namespace LibCmo::CK2 {

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
			m_Context(ctx)
		{}
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
			return m_ManagerName.c_str();
		}

		/**
		@brief Called to save manager data.
		@param SavedFile A pointer to the CKFile being saved.
		@return This function should return a valid CKStateChunk that contain data to save or NULL if there is nothing to save.
		@remark
			+ During a save operation, each manager is given the opportunity to save its data in the file.
			+ The file being saved is given for information only and must not be modified. It can be used to decide whether it is worth saving
			data for your manager.
		@see CKStateChunk, LoadData
		*/
		virtual CKStateChunk* SaveData(CKFileVisitor* SavedFile) {
			return nullptr;
		}
		/**
		@brief Called to load manager data.
		@param chunk A pointer to a CKStateChunk that was saved in the file.
		@param LoadedFile A pointer to the CKFile being loaded.
		@return CK_OK if successful or an error code otherwise.
		@remark
			+ During a load operation, each manager is automatically called if there was a chunk saved in the file with SaveData.
		@see CKStateChunk, SaveData
		*/
		virtual CKERROR LoadData(CKStateChunk* chunk, CKFileVisitor* LoadedFile) {
			return CKERROR::CKERR_OK; 
		}

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
