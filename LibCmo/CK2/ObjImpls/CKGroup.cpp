#include "CKSceneObject.hpp"
#include "../CKStateChunk.hpp"
#include "CKGroup.hpp"

namespace LibCmo::CK2::ObjImpls {

	bool CKGroup::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		return false;
	}

	bool CKGroup::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		return false;
	}

	CKDWORD CKGroup::GetGroupIndex() {
		return m_GroupIndex;
	}

	CKObject* CKGroup::GetObject(CKDWORD pos) {
		return nullptr;
	}

	CKDWORD CKGroup::GetObjectCount() {
		return CKDWORD();
	}

}
