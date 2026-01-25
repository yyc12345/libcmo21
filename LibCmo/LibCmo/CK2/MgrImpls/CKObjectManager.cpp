#include "CKObjectManager.hpp"
#include "../CKContext.hpp"
#include "../ObjImpls/CKObject.hpp"
#include <yycc/cenum.hpp>

namespace LibCmo::CK2::MgrImpls {

	CKObjectManager::CKObjectManager(CKContext* ctx) :
		CKBaseManager(ctx, OBJECT_MANAGER_GUID, u8"Object Manager"),
		m_ObjectsList(), m_ReturnedObjectOffsets(), m_ObjectCount(0),
		m_GroupGlobalIndex(), m_SceneGlobalIndex(),
		m_ObjectsListByClass(CKGetClassCount()) {}

	CKObjectManager::~CKObjectManager() {
		DestroyAllObjects();
	}

	ObjImpls::CKObject* CKObjectManager::CreateObject(CK_CLASSID cls, CKSTRING name,
		CK_OBJECTCREATION_OPTIONS options, CK_CREATIONMODE* res) {
		// todo: Process paramter options and res

		// get description first
		const CKClassDesc* desc = CKGetClassDesc(cls);
		// if no description, return directly to reject creating object.
		if (desc == nullptr) return nullptr;

		// allocate a CK_ID first
		CKDWORD decided_off;
		if (this->m_ReturnedObjectOffsets.empty()) {
			// create new CK_ID.
			decided_off = static_cast<CKDWORD>(m_ObjectsList.size());
			m_ObjectsList.resize(decided_off + 1);
		} else {
			// use returned CK_ID.
			decided_off = m_ReturnedObjectOffsets.back();
			m_ReturnedObjectOffsets.pop_back();
		}

		// create one
		ObjImpls::CKObject* obj = desc->CreationFct(m_Context, Offset2Id(decided_off), name);

		// put into slot and inc count
		m_ObjectsList[decided_off] = obj;
		++m_ObjectCount;

		// add into classid indexed object list
		m_ObjectsListByClass[static_cast<size_t>(cls)].push_back(Offset2Id(decided_off));

		// set out variable
		return obj;
	}

	ObjImpls::CKObject* CKObjectManager::GetObject(CK_ID id) {
		CKDWORD off = Id2Offset(id);
		if (off >= m_ObjectsList.size()) return nullptr;
		return m_ObjectsList[off];
	}

	CKDWORD CKObjectManager::GetObjectCount() {
		return m_ObjectCount;
	}

	void CKObjectManager::InternalDestroy(ObjImpls::CKObject* obj) {
		// find desc by classid
		const CKClassDesc* desc = CKGetClassDesc(obj->GetClassID());
		// a create CKObject instance definitely can find corresponding desc.
		// if not, throw exception.
		if (desc == nullptr) throw LogicException("Invalid CK_CLASSID");
		// free it
		desc->ReleaseFct(m_Context, obj);
	}

	void CKObjectManager::DestroyObjects(CK_ID* ids, CKDWORD count) {
		// pre notice manager
		m_Context->ExecuteManagersOnSequenceToBeDeleted(ids, count);

		// collect object data
		// remove invalid object first, set them to be deleted
		// collection deleted object class ids
		XContainer::XObjectArray validObjIds;
		XContainer::XBitArray cids;
		for (CKDWORD i = 0; i < count; ++i) {
			CKDWORD off = Id2Offset(ids[i]);
			if (off >= m_ObjectsList.size()) continue;
			ObjImpls::CKObject* obj = m_ObjectsList[off];
			if (obj == nullptr) continue;

			// set to be deleted
			CK_OBJECT_FLAGS objflag = obj->GetObjectFlags();
			yycc::cenum::add(objflag, CK_OBJECT_FLAGS::CK_OBJECT_TOBEDELETED);
			obj->SetObjectFlags(objflag);

			// collect class id
			XContainer::NSXBitArray::Set(cids, static_cast<CKDWORD>(obj->GetClassID()));

			// add into list
			validObjIds.emplace_back(ids[i]);
		}

		// then remove deleted object from m_ObjectListByClass
		// because we have set to be deleted flag.
		for (size_t i = 0; i < m_ObjectsListByClass.size(); ++i) {
			if (!XContainer::NSXBitArray::IsSet(cids, static_cast<CKDWORD>(i))) 
				continue;
			XContainer::NSXObjectArray::PreDeletedCheck(m_ObjectsListByClass[i], m_Context);
		}

		// use collected cid to get all class ids which need receive notify
		// and use m_ObjectListByClass to notify them
		XContainer::XBitArray notifyCids = CKGetAllNotifyClassID(cids);
		for (size_t i = 0; i < m_ObjectsListByClass.size(); ++i) {
			if (!XContainer::NSXBitArray::IsSet(notifyCids, static_cast<CKDWORD>(i))) 
				continue;
			for (auto& objid : m_ObjectsListByClass[i]) {
				m_ObjectsList[Id2Offset(objid)]->CheckPreDeletion();
			}
		}

		// calling PreDelete function for deleted objects
		for (const auto& objid : validObjIds) {
			m_ObjectsList[Id2Offset(objid)]->PreDelete();
		}

		// then free all valid object
		for (const auto& objid : validObjIds) {
			CKDWORD off = Id2Offset(objid);
			ObjImpls::CKObject* obj = m_ObjectsList[off];
			
			// free it
			InternalDestroy(obj);
			
			// return its allocated id.
			// and dec count
			m_ObjectsList[off] = nullptr;
			m_ReturnedObjectOffsets.emplace_back(off);
			--m_ObjectCount;
		}

		// run post deletion for notify object
		for (size_t i = 0; i < m_ObjectsListByClass.size(); ++i) {
			if (!XContainer::NSXBitArray::IsSet(notifyCids, static_cast<CKDWORD>(i))) 
				continue;
			for (auto& objid : m_ObjectsListByClass[i]) {
				m_ObjectsList[Id2Offset(objid)]->CheckPostDeletion();
			}
		}

		// notice post
		m_Context->ExecuteManagersOnSequenceDeleted(ids, count);
	}

	void CKObjectManager::DestroyAllObjects() {
		// free all created objects
		for (auto& ptr : m_ObjectsList) {
			if (ptr != nullptr) {
				InternalDestroy(ptr);
			}
		}
		// restore returned object list
		m_ReturnedObjectOffsets.clear();
		// empty object list
		m_ObjectsList.clear();
		// reset count
		m_ObjectCount = 0;

		// clear obj by class list
		for (auto& ls : m_ObjectsListByClass) {
			ls.clear();
		}

		// clear group and scene global index at the same time
		m_SceneGlobalIndex.clear();
		m_GroupGlobalIndex.clear();
	}

	XContainer::XObjectPointerArray CKObjectManager::GetObjectByNameAndClass(CKSTRING name, CK_CLASSID cid, bool derived) {
		XContainer::XObjectPointerArray result;

		for (size_t i = 0; i < m_ObjectsListByClass.size(); ++i) {
			// check class id first
			if (derived) {
				if (!CKIsChildClassOf(static_cast<CK_CLASSID>(i), cid)) continue;
			} else {
				if (static_cast<CK_CLASSID>(i) != cid) continue;
			}

			// iterate all sub object and check name
			for (const auto& objoff : m_ObjectsListByClass[i]) {
				ObjImpls::CKObject* obj = m_ObjectsList[objoff];

				if (name == nullptr) {
					// directly add
					result.emplace_back(obj);
				} else {
					// check name
					if (CKStrEqual(name, obj->GetName())) {
						result.emplace_back(obj);
					}
				}
			}
		}

		return result;
	}

#pragma region Object Check

	bool CKObjectManager::IsObjectSafe(CK_ID objid) {
		CKDWORD off = Id2Offset(objid);
		if (off >= m_ObjectsList.size()) return false;
		return m_ObjectsList[off] != nullptr;
	}

	bool CKObjectManager::IsObjectPointerSafe(const ObjImpls::CKObject* objptr) {
		if (objptr == nullptr) return false;

		// iterate all object list to check
		for (const auto& ptr : m_ObjectsList) {
			if (ptr == objptr) return true;
		}
		return false;
	}
	
#pragma endregion

#pragma region Special Functions

	CKDWORD CKObjectManager::AllocateGroupGlobalIndex() {
		// try find first non-true position
		CKDWORD index;
		if (!XContainer::NSXBitArray::GetUnsetBitPosition(m_GroupGlobalIndex, 0, index)) {
			// failed. distribute new one
			index = static_cast<CKDWORD>(m_GroupGlobalIndex.size());
			m_GroupGlobalIndex.resize(m_GroupGlobalIndex.size() + 1);
		}
		
		// set to occupy
		XContainer::NSXBitArray::Set(m_GroupGlobalIndex, index);
		return index;
	}

	CKDWORD CKObjectManager::AllocateSceneGlobalIndex() {
		// same as group
		CKDWORD index;
		if (!XContainer::NSXBitArray::GetUnsetBitPosition(m_SceneGlobalIndex, 0, index)) {
			index = static_cast<CKDWORD>(m_SceneGlobalIndex.size());
			m_SceneGlobalIndex.resize(m_SceneGlobalIndex.size() + 1);
		}
		
		XContainer::NSXBitArray::Set(m_SceneGlobalIndex, index);
		return index;
	}

	void CKObjectManager::FreeGroupGlobalIndex(CKDWORD id) {
		// check position
		if (id >= m_GroupGlobalIndex.size()) return;
		// set value
		XContainer::NSXBitArray::Unset(m_GroupGlobalIndex, id);
	}

	void CKObjectManager::FreeSceneGlobalIndex(CKDWORD id) {
		// same as group
		if (id >= m_SceneGlobalIndex.size()) return;
		XContainer::NSXBitArray::Unset(m_SceneGlobalIndex, id);
	}
	
#pragma endregion

}
