#include "CKCamera.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {

	// Convenient macro to mark this object is not UPTODATE.
#define REMOVE_UPTODATE_FLAG { \
CK_OBJECT_FLAGS obj_flags = GetObjectFlags(); \
YYCC::EnumHelper::Remove(obj_flags, CK_OBJECT_FLAGS::CK_OBJECT_UPTODATE); \
SetObjectFlags(obj_flags); \
}

	CKCamera::CKCamera(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CK3dEntity(ctx, ckid, name),
		m_ProjectType(CK_CAMERA_PROJECTION::CK_PERSPECTIVEPROJECTION),
		m_Fov(0.5f), m_OrthographicZoom(1.0f),
		m_Width(4), m_Height(3),
		m_FrontPlane(1.0f), m_BackPlane(4000.0f) {
		REMOVE_UPTODATE_FLAG;
	}

	CKCamera::~CKCamera() {}

	bool CKCamera::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CK3dEntity::Save(chunk, file, flags);
		if (!suc) return false;

		// Save main data
		{
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_CAMERA::CK_STATESAVE_CAMERAONLY);
			chunk->WriteStruct(m_ProjectType);
			chunk->WriteStruct(m_Fov);
			chunk->WriteStruct(m_OrthographicZoom);

			// Build width and height compound.
			CKDWORD widht_and_height = m_Width & 0x0000FFFFu;
			widht_and_height |= (m_Height << 16) & 0xFFFF0000u;
			chunk->WriteStruct(widht_and_height);

			chunk->WriteStruct(m_FrontPlane);
			chunk->WriteStruct(m_BackPlane);
		}

		chunk->SetClassId(CK_CLASSID::CKCID_CAMERA);
		return true;
	}

	bool CKCamera::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CK3dEntity::Load(chunk, file);
		if (!suc) return false;

		// MARK: Drop the support for very old file format.
		if (chunk->GetDataVersion() < CK_STATECHUNK_DATAVERSION::CHUNK_MAJORCHANGE_VERSION) {
			return false;
		}

		// Read main data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_CAMERA::CK_STATESAVE_CAMERAONLY)) {
			chunk->ReadStruct(m_ProjectType);
			chunk->ReadStruct(m_Fov);
			chunk->ReadStruct(m_OrthographicZoom);

			// Width and Height is stored in one DWORD
			// Higher WORD is height and lower WORD is width.
			// HIGH >>> height (2 bytes), width (2 bytes) <<< LOW
			CKDWORD widht_and_height;
			chunk->ReadStruct(widht_and_height);
			m_Width = widht_and_height & 0x0000FFFFu;
			m_Height = (widht_and_height & 0xFFFF0000u) >> 16;

			chunk->ReadStruct(m_FrontPlane);
			chunk->ReadStruct(m_BackPlane);
		}

		REMOVE_UPTODATE_FLAG;

		return true;
	}

#pragma region Class Operations

	CK_CAMERA_PROJECTION CKCamera::GetProjectionType() const {
		return m_ProjectType;
	}
	void CKCamera::SetProjectionType(CK_CAMERA_PROJECTION proj) {
		m_ProjectType = proj;
		REMOVE_UPTODATE_FLAG;
	}

	CKFLOAT CKCamera::GetOrthographicZoom() const {
		return m_OrthographicZoom;
	}
	void CKCamera::SetOrthographicZoom(CKFLOAT zoom) {
		m_OrthographicZoom = zoom;
		REMOVE_UPTODATE_FLAG;
	}

	CKFLOAT CKCamera::GetFrontPlane() const {
		return m_FrontPlane;
	}
	CKFLOAT CKCamera::GetBackPlane() const {
		return m_BackPlane;
	}
	CKFLOAT CKCamera::GetFov() const {
		return m_Fov;
	}
	void CKCamera::SetFrontPlane(CKFLOAT front) {
		m_FrontPlane = front;
		REMOVE_UPTODATE_FLAG;
	}
	void CKCamera::SetBackPlane(CKFLOAT back) {
		m_BackPlane = back;
		REMOVE_UPTODATE_FLAG;
	}
	void CKCamera::SetFov(CKFLOAT fov) {
		m_Fov = fov;
		REMOVE_UPTODATE_FLAG;
	}

	void CKCamera::GetAspectRatio(CKDWORD& width, CKDWORD& height) const {
		width = m_Width;
		height = m_Height;
	}
	void CKCamera::SetAspectRatio(CKDWORD width, CKDWORD height) {
		m_Width = width;
		m_Height = height;
		REMOVE_UPTODATE_FLAG;
	}

	void CKCamera::ComputeProjectionMatrix(VxMath::VxMatrix& mat) const {
		CKFLOAT aspect = m_Width / m_Height;
		if (m_ProjectType == CK_CAMERA_PROJECTION::CK_PERSPECTIVEPROJECTION) {
			mat.Perspective(m_Fov, aspect, m_FrontPlane, m_BackPlane);
		} else {
			mat.Orthographic(m_OrthographicZoom, aspect, m_FrontPlane, m_BackPlane);
		}
	}

	//void CKCamera::ResetRoll() {}
	//void CKCamera::Roll(CKFLOAT angle) {}

	CK3dEntity* CKCamera::GetTarget() const {
		// Not supported, return nullptr anyway.
		return nullptr;
	}
	void CKCamera::SetTarget(CK3dEntity* target) {
		// Do nothing because no support.
	}

#pragma endregion

	// Undef convenient macro
#undef REMOVE_UPTODATE_FLAG

}
