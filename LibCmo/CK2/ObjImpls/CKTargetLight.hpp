#pragma once

#include "../../VTInternal.hpp"
#include "CKLight.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKTargetLight : public CKLight {
	public:
		CKTargetLight(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKTargetLight();
		YYCC_DEL_CLS_COPY_MOVE(CKTargetLight);

		virtual CK_CLASSID GetClassID() override {
			return CK_CLASSID::CKCID_TARGETLIGHT;
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
