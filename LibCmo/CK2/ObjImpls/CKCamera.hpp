#pragma once

#include "../../VTInternal.hpp"
#include "CK3dEntity.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKCamera : public CK3dEntity {
	public:
		CKCamera(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKCamera();
		YYCC_DEL_CLS_COPY_MOVE(CKCamera);

		virtual CK_CLASSID GetClassID() override {
			return CK_CLASSID::CKCID_CAMERA;
		}
		
		// 2 RW funcions
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		

		CK_CAMERA_PROJECTION GetProjectionType() const;
		void SetProjectionType(CK_CAMERA_PROJECTION proj);

		CKFLOAT GetOrthographicZoom() const;
		void SetOrthographicZoom(CKFLOAT zoom);

		CKFLOAT GetFrontPlane() const;
		CKFLOAT GetBackPlane() const;
		CKFLOAT GetFov() const;
		void SetFrontPlane(CKFLOAT front);
		void SetBackPlane(CKFLOAT back);
		void SetFov(CKFLOAT fov);

		void GetAspectRatio(CKDWORD& width, CKDWORD& height) const;
		void SetAspectRatio(CKDWORD width, CKDWORD height);

		void ComputeProjectionMatrix(VxMath::VxMatrix& mat) const;

		// TODO: Finish CKCamera roll feature because it now involve some functions which is not implemented in CK3dEntity.
		// Roll Angle
		//void ResetRoll();
		//void Roll(CKFLOAT angle);

		// Target access
		virtual CK3dEntity* GetTarget() const;
		virtual void SetTarget(CK3dEntity* target);

	protected:
		CKFLOAT m_Fov;
		CKFLOAT m_FrontPlane, m_BackPlane;
		CK_CAMERA_PROJECTION m_ProjectType;
		CKFLOAT m_OrthographicZoom;
		CKDWORD m_Width, m_Height;
	};

}
