#include "CKTexture.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKTexture::CKTexture(CKContext* ctx, CK_ID ckid, CKSTRING name) {}

	CKTexture::~CKTexture() {}

	bool CKTexture::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKBeObject::Save(chunk, file, flags);
		if (!suc) return false;

		return true;
	}

	bool CKTexture::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKBeObject::Load(chunk, file);
		if (!suc) return false;

		return true;
	}

}
