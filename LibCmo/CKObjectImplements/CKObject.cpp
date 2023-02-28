#include "../CKObjects.hpp"

namespace LibCmo::CK2::CKObjectImplements {

	CKObject::CKObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name) :
		m_ID(ckid), m_Context(ctx),
		m_Name(name == nullptr ? "" : name),
		m_ObjectFlags(CK_OBJECT_FLAGS::CK_PARAMETERIN_DISABLED) {
		;
	}

	CKObject::~CKObject() {

	}

	CKERROR CKObject::Load(CKStateChunk* chunk, const CKFileData::ShallowDocument* doc) {
		return CKERROR::CKERR_OK;
	}
	CKStateChunk* CKObject::Save(CKFileData::ShallowDocument* doc) {
		return nullptr;
	}

	CKSceneObject::CKSceneObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	}
	CKSceneObject::~CKSceneObject() {
	}

	CKBeObject::CKBeObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKSceneObject(ctx, ckid, name) {
	}
	CKBeObject::~CKBeObject() {
	}

	CKGroup::CKGroup(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	}
	CKGroup::~CKGroup() {
	}

	CKMesh::CKMesh(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	}
	CKMesh::~CKMesh() {
	}

	CKTexture::CKTexture(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	}
	CKTexture::~CKTexture() {
	}

	CKMaterial::CKMaterial(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	}
	CKMaterial::~CKMaterial() {
	}

	CKRenderObject::CKRenderObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKBeObject(ctx, ckid, name) {
	}
	CKRenderObject::~CKRenderObject() {
	}

	CK3dEntity::CK3dEntity(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKRenderObject(ctx, ckid, name) {
	}
	CK3dEntity::~CK3dEntity() {
	}

	CK3dObject::CK3dObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CK3dEntity(ctx, ckid, name) {
	}
	CK3dObject::~CK3dObject() {
	}

}