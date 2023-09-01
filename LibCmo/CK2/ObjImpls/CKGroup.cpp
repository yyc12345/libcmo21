#include "CKGroup.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include <algorithm>

namespace LibCmo::CK2::ObjImpls {

	CKGroup::CKGroup(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKBeObject(ctx, ckid, name),
		m_ObjectArray(),
		m_GroupIndex(m_Context->AllocateGroupGlobalIndex()) {}

	CKGroup::~CKGroup() {
		m_Context->FreeGroupGlobalIndex(m_GroupIndex);
	}

	bool CKGroup::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKBeObject::Save(chunk, file, flags);
		if (!suc) return false;

		return true;
	}

	bool CKGroup::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKBeObject::Load(chunk, file);
		if (!suc) return false;

		return true;
	}

	CKDWORD CKGroup::GetGroupIndex() {
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
		o->CKGroup_SetGroups(m_GroupIndex, true);
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
		obj->CKGroup_SetGroups(m_GroupIndex, false);
		// remove self
		m_ObjectArray.erase(it);
		return obj;
	}

	void CKGroup::RemoveObject(CKBeObject* obj) {
		// find first
		auto finder = std::find(m_ObjectArray.begin(), m_ObjectArray.end(), static_cast<CKObject*>(obj));
		if (finder != m_ObjectArray.end()) {
			// set object
			static_cast<CKBeObject*>(*finder)->CKGroup_SetGroups(m_GroupIndex, false);
			// remove self
			m_ObjectArray.erase(finder);
		}
	}

	void CKGroup::Clear() {
		for (auto& beobj : m_ObjectArray) {
			// set object
			static_cast<CKBeObject*>(beobj)->CKGroup_SetGroups(m_GroupIndex, false);
		}

		m_ObjectArray.clear();
	}

	CKBeObject* CKGroup::GetObject(CKDWORD pos) {
		if (pos >= m_ObjectArray.size()) return nullptr;
		else return static_cast<CKBeObject*>(m_ObjectArray[pos]);
	}

	CKDWORD CKGroup::GetObjectCount() {
		return static_cast<CKDWORD>(m_ObjectArray.size());
	}

}
