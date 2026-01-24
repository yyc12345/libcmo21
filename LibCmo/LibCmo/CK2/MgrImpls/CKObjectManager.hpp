#pragma once

#include "../../VTInternal.hpp"
#include "CKBaseManager.hpp"
#include <deque>

namespace LibCmo::CK2::MgrImpls {

	class CKObjectManager : public CKBaseManager {
	public:
		CKObjectManager(CKContext* ctx);
		virtual ~CKObjectManager();
		YYCC_DEL_CLS_COPY_MOVE(CKObjectManager);

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
		ObjImpls::CKObject* CreateObject(CK_CLASSID cls, CKSTRING name,
			CK_OBJECTCREATION_OPTIONS options = CK_OBJECTCREATION_OPTIONS::CK_OBJECTCREATION_NONAMECHECK,
			CK_CREATIONMODE* res = nullptr);
		// todo: implement CopyObject by CKClassDesc
		//ObjImpls::CKObject* CopyObject(ObjImpls::CKObject *src,
		//	CKDependencies* Dependencies = nullptr,
		//	CKSTRING AppendName = nullptr,
		//	CK_OBJECTCREATION_OPTIONS Options = CK_OBJECTCREATION_OPTIONS::CK_OBJECTCREATION_NONAMECHECK);

		ObjImpls::CKObject* GetObject(CK_ID id);
		CKDWORD GetObjectCount();
		void DestroyObjects(CK_ID* ids, CKDWORD count);
		void DestroyAllObjects();

		// ========== Objects Access ==========

		/**
		 * @brief General object list query.
		 * @param name nullptr if no requirement.
		 * @param cid the class id
		 * @param derived whether considering derived class
		 * @return the result pointer list.
		*/
		XContainer::XObjectPointerArray GetObjectByNameAndClass(
			CKSTRING name, CK_CLASSID cid, bool derived);

		// ========== Object Check ==========
		bool IsObjectSafe(CK_ID objid);
		bool IsObjectPointerSafe(const ObjImpls::CKObject* objptr);

		// ========== Special Functions ==========

		CKDWORD AllocateGroupGlobalIndex();
		CKDWORD AllocateSceneGlobalIndex();
		void FreeGroupGlobalIndex(CKDWORD id);
		void FreeSceneGlobalIndex(CKDWORD id);

	protected:
		/**
		 * The global offset of created CK_ID.
		 * The value close to zero may cause some issue.
		 * So we add a static offset to every created CK_ID.
		*/
		const CK_ID c_ObjectIdOffset = 61u;
		CKDWORD Id2Offset(CK_ID id) { return static_cast<CKDWORD>(id - c_ObjectIdOffset); }
		CK_ID Offset2Id(CKDWORD off) { return static_cast<CK_ID>(off + c_ObjectIdOffset); }

		/**
		 * @brief The real CKObject destroy worker shared by CKObjectManager::DestroyObject and CKObjectManager::~CKObjectManager
		 * @param[in] obj The CKObject need to be free.
		*/
		void InternalDestroy(ObjImpls::CKObject* obj);

		CKDWORD m_ObjectCount;
		XContainer::XObjectPointerArray m_ObjectsList;
		XContainer::XArray<XContainer::XList<CKDWORD>> m_ObjectsListByClass;
		std::deque<CKDWORD> m_ReturnedObjectOffsets;

		XContainer::XBitArray m_GroupGlobalIndex;
		XContainer::XBitArray m_SceneGlobalIndex;

	};

}
