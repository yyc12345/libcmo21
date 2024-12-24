#include "CKCamera.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {

#pragma region Class Operations

	CK_CAMERA_PROJECTION CKCamera::GetProjectionType() const {
		return CK_CAMERA_PROJECTION();
	}
	void CKCamera::SetProjectionType(CK_CAMERA_PROJECTION proj) {}

	CKFLOAT CKCamera::GetOrthographicZoom() const {
		return CKFLOAT();
	}
	void CKCamera::SetOrthographicZoom(CKFLOAT zoom) {}

	CKFLOAT CKCamera::GetFrontPlane() const {
		return CKFLOAT();
	}
	CKFLOAT CKCamera::GetBackPlane() const {
		return CKFLOAT();
	}
	CKFLOAT CKCamera::GetFov() const {
		return CKFLOAT();
	}
	void CKCamera::SetFrontPlane(CKFLOAT front) {}
	void CKCamera::SetBackPlane(CKFLOAT back) {}
	void CKCamera::SetFov(CKFLOAT fov) {}

	void CKCamera::GetAspectRatio(int& width, int& height) const {}
	void CKCamera::SetAspectRatio(int width, int height) {}

	void CKCamera::ComputeProjectionMatrix(VxMath::VxMatrix& mat) const {}

	void CKCamera::ResetRoll() {}
	void CKCamera::Roll(CKFLOAT angle) {}

	CK3dEntity* CKCamera::GetTarget() const {
		return nullptr;
	}
	void CKCamera::SetTarget(CK3dEntity* target) {}

#pragma endregion


}
