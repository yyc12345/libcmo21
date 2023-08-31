#include "CKSceneObject.hpp"
#include "../CKStateChunk.hpp"
#include "CKBeObject.hpp"

namespace LibCmo::CK2::ObjImpls {

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
		return false;
	}

}