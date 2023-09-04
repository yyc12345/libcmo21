#include "CKObjectManager.hpp"
#include "../ObjImpls/CKObject.hpp"

namespace LibCmo::CK2::MgrImpls {

	CKObjectManager::CKObjectManager(CKContext* ctx) :
		CKBaseManager(ctx, OBJECT_MANAGER_GUID, "Object Manager"),
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
		// if really we can not find it, we only can delete it directly.
		const CKClassDesc* desc = CKGetClassDesc(obj->GetClassID());
		if (desc == nullptr) {
			delete obj;
			return;
		}

		// free it
		desc->ReleaseFct(m_Context, obj);
	}

	void CKObjectManager::DestroyObject(CK_ID id) {
		CKDWORD off = Id2Offset(id);
		if (off >= m_ObjectsList.size()) return;

		// get object and free it
		ObjImpls::CKObject* obj = m_ObjectsList[off];
		if (obj == nullptr) return;
		InternalDestroy(obj);

		// return its allocated id.
		// and dec count
		m_ObjectsList[off] = nullptr;
		m_ReturnedObjectOffsets.emplace_back(off);
		--m_ObjectCount;

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
	}

	CKDWORD CKObjectManager::AllocateGroupGlobalIndex(ObjImpls::CKObject* group) {
		// try find first nullptr position
		CKDWORD index = 0;
		for (const auto& ptr : m_GroupGlobalIndex) {
			if (ptr == nullptr) break;
			++index;
		}
		// resize array for new position
		if (index == m_GroupGlobalIndex.size()) {
			m_GroupGlobalIndex.resize(m_GroupGlobalIndex.size() + 1);
		}

		// set to occupy
		m_GroupGlobalIndex[index] = group;
		return index;
	}

	CKDWORD CKObjectManager::AllocateSceneGlobalIndex(ObjImpls::CKObject* scene) {
		// same as group
		CKDWORD index = 0;
		for (const auto& ptr : m_SceneGlobalIndex) {
			if (ptr == nullptr) break;
			++index;
		}
		// resize array for new position
		if (index == m_SceneGlobalIndex.size()) {
			m_SceneGlobalIndex.resize(m_SceneGlobalIndex.size() + 1);
		}

		// set to occupy
		m_SceneGlobalIndex[index] = scene;
		return index;
	}

	ObjImpls::CKObject* CKObjectManager::GetGroupByGlobalIndex(CKDWORD index) {
		if (index >= m_GroupGlobalIndex.size()) return nullptr;
		else return m_GroupGlobalIndex[index];
	}

	ObjImpls::CKObject* CKObjectManager::GetSceneByGlobalIndex(CKDWORD index) {
		if (index >= m_SceneGlobalIndex.size()) return nullptr;
		else return m_SceneGlobalIndex[index];
	}

	void CKObjectManager::FreeGroupGlobalIndex(CKDWORD id) {
		// check position
		if (id >= m_GroupGlobalIndex.size()) return;
		// set value
		m_GroupGlobalIndex[id] = nullptr;
	}

	void CKObjectManager::FreeSceneGlobalIndex(CKDWORD id) {
		// same as group
		if (id >= m_SceneGlobalIndex.size()) return;
		m_SceneGlobalIndex[id] = nullptr;
	}

}
