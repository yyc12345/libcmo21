#include "../CKObjects.hpp"
#include "../CKStateChunk.hpp"
#include "../CKIdentifiers.hpp"
#include "../VTUtils.hpp"

namespace LibCmo::CK2::CKObjectImplements {

	CKObject::CKObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name) :
		m_ID(ckid), m_Context(ctx),
		m_Name(name == nullptr ? "" : name),
		m_ObjectFlags(CK_OBJECT_FLAGS::CK_PARAMETERIN_DISABLED) {
		;
	}

	CKObject::~CKObject() {

	}

	bool CKObject::Load(CKStateChunk* chunk, const CKFileDocument* doc) {
		if (chunk->SeekIdentifier(Identifiers::CK_STATESAVEFLAGS_OBJECT::CK_STATESAVE_OBJECTHIDDEN)) {
			EnumsHelper::FlagEnumRm(this->m_ObjectFlags,
				{ CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE,
				CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE }
			);
		} else {
			if (chunk->SeekIdentifier(Identifiers::CK_STATESAVEFLAGS_OBJECT::CK_STATESAVE_OBJECTHIERAHIDDEN)) {
				// != 0
				EnumsHelper::FlagEnumRm(this->m_ObjectFlags,
					{ CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE, }
				);
				EnumsHelper::FlagEnumAdd(this->m_ObjectFlags,
					{ CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE, }
				);

			} else {
				// == 0
				EnumsHelper::FlagEnumAdd(this->m_ObjectFlags,
					{ CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE, }
				);
				EnumsHelper::FlagEnumRm(this->m_ObjectFlags,
					{ CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE, }
				);
			}
		}

		return true;
	}
	CKStateChunk* CKObject::Save(const CKFileDocument* doc) {
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


	CKParameterIn::CKParameterIn(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	}
	CKParameterIn::~CKParameterIn() {
	}

	CKParameter::CKParameter(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	}
	CKParameter::~CKParameter() {
	}

	CKParameterOut::CKParameterOut(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKParameter(ctx, ckid, name) {
	}
	CKParameterOut::~CKParameterOut() {
	}

	CKParameterLocal::CKParameterLocal(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKParameter(ctx, ckid, name) {
	}
	CKParameterLocal::~CKParameterLocal() {
	}

	CKParameterOperation::CKParameterOperation(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	}
	CKParameterOperation::~CKParameterOperation() {
	}

	CKBehaviorLink::CKBehaviorLink(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	}
	CKBehaviorLink::~CKBehaviorLink() {
	}

	CKBehaviorIO::CKBehaviorIO(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKObject(ctx, ckid, name) {
	}
	CKBehaviorIO::~CKBehaviorIO() {
	}

	CKBehavior::CKBehavior(CKMinContext* ctx, CK_ID ckid, CKSTRING name) : CKSceneObject(ctx, ckid, name) {
	}
	CKBehavior::~CKBehavior() {
	}

}