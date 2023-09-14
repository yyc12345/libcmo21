#include "CKMaterial.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKMaterial::CKMaterial(CKContext* ctx, CK_ID ckid, CKSTRING name) :
	CKBeObject(ctx, ckid, name){}

	CKMaterial::~CKMaterial() {}

	bool CKMaterial::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		return false;
	}

	bool CKMaterial::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		return false;
	}

}