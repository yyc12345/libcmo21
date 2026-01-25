#pragma once

#include "../../VTInternal.hpp"
#include "CKCamera.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKTargetCamera : public CKCamera {
	public:
		CKTargetCamera(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKTargetCamera();
		YYCC_DELETE_COPY_MOVE(CKTargetCamera)

		virtual CK_CLASSID GetClassID() override {
			return CK_CLASSID::CKCID_TARGETCAMERA;
		}
		
		virtual void PreDelete() override;
		virtual void CheckPostDeletion() override;

		// 2 RW funcions
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		

		virtual CK3dEntity* GetTarget() const override;
		virtual void SetTarget(CK3dEntity* target) override;

	protected:
		CK_ID m_Target3dEntity;
	};

}
