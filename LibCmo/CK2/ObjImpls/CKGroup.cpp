#include "CKGroup.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include "../MgrImpls/CKObjectManager.hpp"
#include <algorithm>

namespace LibCmo::CK2::ObjImpls {

	CKGroup::CKGroup(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKBeObject(ctx, ckid, name),
		m_ObjectArray(),
		m_GroupIndex(m_Context->GetObjectManager()->AllocateGroupGlobalIndex()) {}

	CKGroup::~CKGroup() {
		// free self allocated id
		m_Context->GetObjectManager()->FreeGroupGlobalIndex(m_GroupIndex);
	}

	void CKGroup::PreDelete() {
		CKBeObject::PreDelete();

		// unlink all grouped object
		for (auto& ptr : m_ObjectArray) {
			static_cast<CKBeObject*>(ptr)->ExplicitSetGroup(m_GroupIndex, false);
		}
		m_ObjectArray.clear();
	}

	void CKGroup::CheckPreDeletion() {
		CKBeObject::CheckPreDeletion();

		// remove self invalid object ptr
		XContainer::NSXObjectPointerArray::PreDeletedCheck(m_ObjectArray, m_Context);
	}

	bool CKGroup::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKBeObject::Save(chunk, file, flags);
		if (!suc) return false;

		return true;
	}

	bool CKGroup::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKBeObject::Load(chunk, file);
		if (!suc) return false;

		// cleat self
		this->Clear();

		// get grouped objects
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_GROUP::CK_STATESAVE_GROUPALL)) {
			XContainer::XObjectPointerArray ptrs;
			chunk->ReadXObjectPointerArray(ptrs);

			// filter pointer and check them type
			for (auto& ptr : ptrs) {
				// skip bad one
				if (ptr == nullptr || ptr == this
					|| !CKIsChildClassOf(ptr->GetClassID(), CK_CLASSID::CKCID_BEOBJECT)) {
					continue;
				}
				CKBeObject* beobj = static_cast<CKBeObject*>(ptr);
				if (beobj->IsInGroup(this)) continue;

				// add good one
				beobj->ExplicitSetGroup(m_GroupIndex, true);
				m_ObjectArray.emplace_back(beobj);
			}

		}

		return true;
	}

	void CKGroup::Show(CK_OBJECT_SHOWOPTION show) {
		CKObject::Show(show);

		// call Show for all sub object
		for (auto& ptr : m_ObjectArray) {
			if (ptr == nullptr) continue;
			ptr->Show(show);
		}
	}

	CKDWORD CKGroup::GetGroupIndex() const {
		return m_GroupIndex;
	}

	CKERROR CKGroup::AddObject(CKBeObject* o) {
		if (o == nullptr || o == this || !CKIsChildClassOf(o->GetClassID(), CK_CLASSID::CKCID_BEOBJECT)) {
			return CKERROR::CKERR_INVALIDPARAMETER;
		}
		if (o->IsInGroup(this)) {
			return CKERROR::CKERR_ALREADYPRESENT;
		}

		// set object
		o->ExplicitSetGroup(m_GroupIndex, true);
		// set self
		m_ObjectArray.emplace_back(o);
		return CKERROR::CKERR_OK;
	}

	CKBeObject* CKGroup::RemoveObject(CKDWORD pos) {
		// check pos
		if (pos >= m_ObjectArray.size()) return nullptr;

		auto it = m_ObjectArray.begin() + pos;
		CKBeObject* obj = static_cast<CKBeObject*>(*it);
		// set object
		obj->ExplicitSetGroup(m_GroupIndex, false);
		// remove self
		m_ObjectArray.erase(it);
		return obj;
	}

	void CKGroup::RemoveObject(CKBeObject* obj) {
		// find first
		auto finder = std::find(m_ObjectArray.begin(), m_ObjectArray.end(), static_cast<CKObject*>(obj));
		if (finder != m_ObjectArray.end()) {
			// set object
			static_cast<CKBeObject*>(*finder)->ExplicitSetGroup(m_GroupIndex, false);
			// remove self
			m_ObjectArray.erase(finder);
		}
	}

	void CKGroup::Clear() {
		for (auto& beobj : m_ObjectArray) {
			// set object
			static_cast<CKBeObject*>(beobj)->ExplicitSetGroup(m_GroupIndex, false);
		}

		m_ObjectArray.clear();
	}

	CKBeObject* CKGroup::GetObject(CKDWORD pos) const {
		if (pos >= m_ObjectArray.size()) return nullptr;
		else return static_cast<CKBeObject*>(m_ObjectArray[pos]);
	}

	CKDWORD CKGroup::GetObjectCount() const {
		return static_cast<CKDWORD>(m_ObjectArray.size());
	}

}
