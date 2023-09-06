#include "CKSceneObject.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include "../MgrImpls/CKObjectManager.hpp"
#include "CKBeObject.hpp"
#include "CKGroup.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKBeObject::CKBeObject(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKSceneObject(ctx, ckid, name) {}

	CKBeObject::~CKBeObject() {
		// remove self from all group
		for (size_t i = 0; i < m_Groups.size(); ++i) {
			if (m_Groups[i]) {
				CKGroup* group = static_cast<CKGroup*>(m_Context->GetObjectManager()->GetGroupByGlobalIndex(static_cast<CKDWORD>(i)));
				group->RemoveObject(this);
			}
		}
	}

	bool CKBeObject::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKSceneObject::Save(chunk, file, flags);
		if (!suc) return false;

		return true;
	}

	bool CKBeObject::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKSceneObject::Load(chunk, file);
		if (!suc) return false;

		return true;
	}

	bool CKBeObject::IsInGroup(CKGroup* group) {
		if (group == nullptr) return false;
		CKDWORD idx = group->CKBeObject_GetGroupIndex();
		if (idx >= m_Groups.size()) return false;
		return m_Groups[idx];
	}

	void CKBeObject::CKGroup_SetGroups(CKDWORD pos, bool val) {
		if (pos >= m_Groups.size()) m_Groups.resize(pos + 1);
		m_Groups[pos] = val;
	}

}