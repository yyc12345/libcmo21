#include "CKObject.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {


	void CKObject::PreSave(CKFileVisitor* file, CKDWORD flags) {}

	bool CKObject::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		return true;
	}

	bool CKObject::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_OBJECT::CK_STATESAVE_OBJECTHIDDEN)) {
			EnumsHelper::Rm(this->m_ObjectFlags, 
				EnumsHelper::Merge({ CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE }));
		} else {
			if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_OBJECT::CK_STATESAVE_OBJECTHIERAHIDDEN)) {
				// != 0
				EnumsHelper::Rm(this->m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
				EnumsHelper::Add(this->m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE);

			} else {
				// == 0
				EnumsHelper::Add(this->m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
				EnumsHelper::Rm(this->m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE);

			}
		}

		return true;
	}

	void CKObject::PostLoad() {}


	//CKSceneObject::CKSceneObject(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	//}
	//CKSceneObject::~CKSceneObject() {
	//}

	//CKBeObject::CKBeObject(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKSceneObject(ctx, ckid, name) {
	//}
	//CKBeObject::~CKBeObject() {
	//}

	//CKGroup::CKGroup(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	//}
	//CKGroup::~CKGroup() {
	//}

	//CKMesh::CKMesh(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	//}
	//CKMesh::~CKMesh() {
	//}

	//CKTexture::CKTexture(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	//}
	//CKTexture::~CKTexture() {
	//}

	//CKMaterial::CKMaterial(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	//}
	//CKMaterial::~CKMaterial() {
	//}

	//CKRenderObject::CKRenderObject(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	//}
	//CKRenderObject::~CKRenderObject() {
	//}

	//CK3dEntity::CK3dEntity(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKRenderObject(ctx, ckid, name) {
	//}
	//CK3dEntity::~CK3dEntity() {
	//}

	//CK3dObject::CK3dObject(CKContext* ctx, CK_ID ckid, CKSTRING name) : CK3dEntity(ctx, ckid, name) {
	//}
	//CK3dObject::~CK3dObject() {
	//}


	//CKParameterIn::CKParameterIn(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	//}
	//CKParameterIn::~CKParameterIn() {
	//}

	//CKParameter::CKParameter(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	//}
	//CKParameter::~CKParameter() {
	//}

	//CKParameterOut::CKParameterOut(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKParameter(ctx, ckid, name) {
	//}
	//CKParameterOut::~CKParameterOut() {
	//}

	//CKParameterLocal::CKParameterLocal(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKParameter(ctx, ckid, name) {
	//}
	//CKParameterLocal::~CKParameterLocal() {
	//}

	//CKParameterOperation::CKParameterOperation(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	//}
	//CKParameterOperation::~CKParameterOperation() {
	//}

	//CKBehaviorLink::CKBehaviorLink(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	//}
	//CKBehaviorLink::~CKBehaviorLink() {
	//}

	//CKBehaviorIO::CKBehaviorIO(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	//}
	//CKBehaviorIO::~CKBehaviorIO() {
	//}

	//CKBehavior::CKBehavior(CKContext* ctx, CK_ID ckid, CKSTRING name) : CKSceneObject(ctx, ckid, name) {
	//}
	//CKBehavior::~CKBehavior() {
	//}

}