#include "CKTargetCamera.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"

namespace LibCmo::CK2::ObjImpls {

	// MARK: THIS CODE IS BARELY FULL COPY OF CKTargetLight.
	// Please sync them if you modify one of them!

	CKTargetCamera::CKTargetCamera(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKCamera(ctx, ckid, name), m_Target3dEntity(0) {}

	CKTargetCamera::~CKTargetCamera() {}

	void CKTargetCamera::PreDelete() {
		// Remove associated target
		SetTarget(nullptr);
	}

	void CKTargetCamera::CheckPostDeletion() {
		CKCamera::CheckPostDeletion();

		// Remove target if is not existing.
		CKObject* target = m_Context->GetObject(m_Target3dEntity);
		if (target == nullptr) {
			m_Target3dEntity = 0;
		}
	}

	bool CKTargetCamera::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKCamera::Save(chunk, file, flags);
		if (!suc) return false;
		
		// Save target
		{
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_CAMERA::CK_STATESAVE_TCAMERATARGET);
			CKObject* target = m_Context->GetObject(m_Target3dEntity);
			chunk->WriteObjectPointer(target);
		}

		chunk->SetClassId(CK_CLASSID::CKCID_TARGETCAMERA);
		return true;
	}

	bool CKTargetCamera::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKCamera::Load(chunk, file);
		if (!suc) return false;
		
		// Read target
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_CAMERA::CK_STATESAVE_TCAMERATARGET)) {
			 chunk->ReadObjectID(m_Target3dEntity);
		}

		return true;
	}

	CK3dEntity* CKTargetCamera::GetTarget() const {
		return static_cast<CK3dEntity*>(m_Context->GetObject(m_Target3dEntity));
	}

	void CKTargetCamera::SetTarget(CK3dEntity* target) {
		// The target can not be self.
		if (target == this) return;

		// First remove current target
		CK3dEntity* old_target = static_cast<CK3dEntity*>(m_Context->GetObject(m_Target3dEntity));
		if (old_target != nullptr) {
			CK_3DENTITY_FLAGS old_target_flags = old_target->GetEntityFlags();
			YYCC::EnumHelper::Remove(old_target_flags, CK_3DENTITY_FLAGS::CK_3DENTITY_TARGETCAMERA);
			YYCC::EnumHelper::Add(old_target_flags, CK_3DENTITY_FLAGS::CK_3DENTITY_FRAME);
			old_target->SetEntityFlags(old_target_flags);
		}

		// Then add specified target
		if (target != nullptr) {
			CK_3DENTITY_FLAGS new_target_flags = target->GetEntityFlags();
			YYCC::EnumHelper::Add(new_target_flags, CK_3DENTITY_FLAGS::CK_3DENTITY_TARGETCAMERA);
			YYCC::EnumHelper::Remove(new_target_flags, CK_3DENTITY_FLAGS::CK_3DENTITY_FRAME);
			target->SetEntityFlags(new_target_flags);
		}

		// Get CK_ID of new target
		CK_ID target_id = 0;
		if (target != nullptr)
			target_id = target->GetID();

		// Assign target id.
		m_Target3dEntity = target_id;
	}

}
