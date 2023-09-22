#include "CKSceneObject.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include "../MgrImpls/CKObjectManager.hpp"
#include "CKBeObject.hpp"
#include "CKGroup.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKBeObject::CKBeObject(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKSceneObject(ctx, ckid, name), m_Groups() {}

	CKBeObject::~CKBeObject() {}

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

	bool CKBeObject::IsInGroup(CKGroup* group) const {
		if (group == nullptr) return false;
		CKDWORD idx = group->GetGroupIndex();
		return XContainer::NSXBitArray::IsSet(m_Groups, idx);
	}

	void CKBeObject::ExplicitSetGroup(CKDWORD pos, bool val) {
		if (val) {
			XContainer::NSXBitArray::Set(m_Groups, pos);
		} else {
			XContainer::NSXBitArray::Unset(m_Groups, pos);
		}
	}

}