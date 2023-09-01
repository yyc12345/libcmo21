#include "CK3dEntity.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {

	bool CK3dEntity::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKRenderObject::Save(chunk, file, flags);
		if (!suc) return false;

		return true;
	}

	bool CK3dEntity::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKRenderObject::Load(chunk, file);
		if (!suc) return false;

		return true;
	}

}
