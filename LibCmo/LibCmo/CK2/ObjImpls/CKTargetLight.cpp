#include "CKTargetLight.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include <yycc/cenum.hpp>

namespace LibCmo::CK2::ObjImpls {
	
	// MARK: THIS CODE IS BARELY FULL COPY OF CKTargetCamera.
	// Please sync them if you modify one of them!

	CKTargetLight::CKTargetLight(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKLight(ctx, ckid, name), m_Target3dEntity(0) {}

	CKTargetLight::~CKTargetLight() {}

	void CKTargetLight::PreDelete() {
		// MARK: In original code, there is no such override.
		// Following statement is written in a function called "vector deleting destructor".
		// Idk what it is. There is no resetting target code in its dtor and elsewhere.
		// I think this is crucial, so I add this overload as my understandings.

		// Remove associated target
		SetTarget(nullptr);
	}

	void CKTargetLight::CheckPostDeletion() {
		CKLight::CheckPostDeletion();

		// Remove target if is not existing.
		CKObject* target = m_Context->GetObject(m_Target3dEntity);
		if (target == nullptr) {
			m_Target3dEntity = 0;
		}
	}

	bool CKTargetLight::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKLight::Save(chunk, file, flags);
		if (!suc) return false;
		
		// Save target
		{
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_LIGHT::CK_STATESAVE_TLIGHTTARGET);
			CKObject* target = m_Context->GetObject(m_Target3dEntity);
			chunk->WriteObjectPointer(target);
		}

		chunk->SetClassId(CK_CLASSID::CKCID_TARGETLIGHT);
		return true;
	}

	bool CKTargetLight::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKLight::Load(chunk, file);
		if (!suc) return false;
		
		// Read target
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_LIGHT::CK_STATESAVE_TLIGHTTARGET)) {
			 chunk->ReadObjectID(m_Target3dEntity);
		}

		return true;
	}

	CK3dEntity* CKTargetLight::GetTarget() const {
		return static_cast<CK3dEntity*>(m_Context->GetObject(m_Target3dEntity));
	}
	void CKTargetLight::SetTarget(CK3dEntity* target) {
		// The target can not be self.
		if (target == this) return;

		// First remove current target
		CK3dEntity* old_target = static_cast<CK3dEntity*>(m_Context->GetObject(m_Target3dEntity));
		if (old_target != nullptr) {
			CK_3DENTITY_FLAGS old_target_flags = old_target->GetEntityFlags();
			yycc::cenum::remove(old_target_flags, CK_3DENTITY_FLAGS::CK_3DENTITY_TARGETLIGHT);
			yycc::cenum::add(old_target_flags, CK_3DENTITY_FLAGS::CK_3DENTITY_FRAME);
			old_target->SetEntityFlags(old_target_flags);
		}

		// Then add specified target
		if (target != nullptr) {
			CK_3DENTITY_FLAGS new_target_flags = target->GetEntityFlags();
			yycc::cenum::add(new_target_flags, CK_3DENTITY_FLAGS::CK_3DENTITY_TARGETLIGHT);
			yycc::cenum::remove(new_target_flags, CK_3DENTITY_FLAGS::CK_3DENTITY_FRAME);
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
