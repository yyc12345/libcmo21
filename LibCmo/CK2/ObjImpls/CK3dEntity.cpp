#include "CK3dEntity.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include "CKMesh.hpp"

namespace LibCmo::CK2::ObjImpls {

	CK3dEntity::CK3dEntity(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKRenderObject(ctx, ckid, name),
		m_PotentialMeshes(), m_CurrentMesh(nullptr),
		m_WorldMatrix(), m_ZOrder(0),
		m_MoveableFlags(EnumsHelper::Merge({
			VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_PICKABLE,
			VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_VISIBLE,
			VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_RENDERCHANNELS,
			VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_INVERSEWORLDMATVALID
		})),
		m_3dEntityFlags(static_cast<CK_3DENTITY_FLAGS>(0)) {}

	CK3dEntity::~CK3dEntity() {}

	void CK3dEntity::CheckPreDeletion() {
		CKRenderObject::CheckPreDeletion();

		// check active mesh
		if (m_CurrentMesh->IsToBeDeleted()) {
			m_CurrentMesh = nullptr;
		}

		// check potential meshes
		XContainer::NSXObjectPointerArray::PreDeletedCheck(m_PotentialMeshes, m_Context);
	}

	bool CK3dEntity::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKRenderObject::Save(chunk, file, flags);
		if (!suc) return false;

		// write associated mesh data
		if (m_CurrentMesh != nullptr || m_PotentialMeshes.size() != 0) {
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_3DENTITY::CK_STATESAVE_MESHS);

			// write current mesh
			chunk->WriteObjectPointer(m_CurrentMesh);

			// write potential meshes
			chunk->WriteXObjectPointerArray(m_PotentialMeshes);
		}

		// write core entity data
		{
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_3DENTITY::CK_STATESAVE_3DENTITYNDATA);

			// regulate self flag again
			// MARK: originally we should check parent here.
			// but we do not support parent and hierarchy feature, so we simply remove flag
			EnumsHelper::Rm(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_PARENTVALID);
			// MARK: originally we should check grouped into CKPlace here.
			// but we do not support CKPlace, so we simply remove this flag
			EnumsHelper::Rm(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_PLACEVALID);
			// check z-order, if not zero, save it
			if (m_ZOrder != 0) {
				EnumsHelper::Add(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_ZORDERVALID);
			} else {
				EnumsHelper::Rm(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_ZORDERVALID);
			}

			// write 2 flags
			chunk->WriteStruct(m_3dEntityFlags);
			chunk->WriteStruct(m_MoveableFlags);

			// write world matrix
			chunk->WriteStruct(reinterpret_cast<const VxMath::VxVector3*>(&m_WorldMatrix[0]));
			chunk->WriteStruct(reinterpret_cast<const VxMath::VxVector3*>(&m_WorldMatrix[1]));
			chunk->WriteStruct(reinterpret_cast<const VxMath::VxVector3*>(&m_WorldMatrix[2]));
			chunk->WriteStruct(reinterpret_cast<const VxMath::VxVector3*>(&m_WorldMatrix[3]));

			// MARK: because we do not support Parent and CKPlace,
			// and the IDA code also instruct that no need to write any data if no Parent or CKPlace.
			// so we skip the Parent and CKPlace writing

			// write z-order
			if (m_ZOrder != 0) {
				chunk->WriteStruct(m_ZOrder);
			}

		}

		chunk->SetClassId(CK_CLASSID::CKCID_3DENTITY);
		return true;
	}

	bool CK3dEntity::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKRenderObject::Load(chunk, file);
		if (!suc) return false;

		// backup moveable flags
		bool hasWorldAligned = EnumsHelper::Has(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_WORLDALIGNED);

		// MARK: object animation is skipped

		// read associated meshs data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_3DENTITY::CK_STATESAVE_MESHS)) {
			// MARK: I don't know why origianl code do not clear potential mesh list
			// so I clear it in there.
			m_PotentialMeshes.clear();

			// read current mesh
			CKObject* pendingMesh = nullptr;
			chunk->ReadObjectPointer(pendingMesh);
			if (pendingMesh != nullptr && pendingMesh->GetClassID() == CK_CLASSID::CKCID_MESH) {
				m_CurrentMesh = static_cast<CKMesh*>(pendingMesh);
			}

			// read other meshs
			XContainer::XObjectPointerArray potentials;
			chunk->ReadXObjectPointerArray(potentials);
			for (const auto& ptr : potentials) {
				if (ptr == nullptr) continue;
				XContainer::NSXObjectPointerArray::AddIfNotHere(m_PotentialMeshes, ptr);
			}

			// add current mesh to potential meshes
			if (m_CurrentMesh != nullptr) {
				XContainer::NSXObjectPointerArray::AddIfNotHere(m_PotentialMeshes, m_CurrentMesh);
			}

		}

		// read core entity data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_3DENTITY::CK_STATESAVE_3DENTITYNDATA)) {
			// read 2 flags
			chunk->ReadStruct(m_3dEntityFlags);
			chunk->ReadStruct(m_MoveableFlags);
			// remove some properties
			EnumsHelper::Rm(m_3dEntityFlags, EnumsHelper::Merge({
				CK_3DENTITY_FLAGS::CK_3DENTITY_UPDATELASTFRAME,
				CK_3DENTITY_FLAGS::CK_3DENTITY_RESERVED0
				}));
			EnumsHelper::Rm(m_MoveableFlags, EnumsHelper::Merge({
				VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_RESERVED2,
				VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_STENCILONLY,
				VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_DONTUPDATEFROMPARENT,
				VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_INVERSEWORLDMATVALID,
				VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_HASMOVED,
				VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_BOXVALID,
				VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_USERBOX,
				VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_UPTODATE
				}));
			if (hasWorldAligned) {
				EnumsHelper::Add(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_WORLDALIGNED);
			}

			// if order render first
			if (EnumsHelper::Has(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_RENDERFIRST)) {
				m_ZOrder = 10000;
			}

			// read matrix
			// reset
			m_WorldMatrix.ResetToIdentity();
			// force read as vector3
			chunk->ReadStruct(reinterpret_cast<VxMath::VxVector3*>(&m_WorldMatrix[0]));
			chunk->ReadStruct(reinterpret_cast<VxMath::VxVector3*>(&m_WorldMatrix[1]));
			chunk->ReadStruct(reinterpret_cast<VxMath::VxVector3*>(&m_WorldMatrix[2]));
			chunk->ReadStruct(reinterpret_cast<VxMath::VxVector3*>(&m_WorldMatrix[3]));
			// MARK: check right-hand?
			// I don't know how it checked, just reinterpter IDA code.
			VxMath::VxVector3 col2(*reinterpret_cast<const VxMath::VxVector3*>(&m_WorldMatrix[2])),
				col1(*reinterpret_cast<const VxMath::VxVector3*>(&m_WorldMatrix[1])),
				col0(*reinterpret_cast<const VxMath::VxVector3*>(&m_WorldMatrix[0]));
			VxMath::VxVector3 crossProduct = VxMath::NSVxVector::CrossProduct(col0, col1);
			CKFLOAT dotProduct = VxMath::NSVxVector::DotProduct(crossProduct, col2);
			if (dotProduct >= 0.0f) {
				EnumsHelper::Rm(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_INDIRECTMATRIX);
			} else {
				EnumsHelper::Add(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_INDIRECTMATRIX);
			}

			// copy visible data
			// process direct visible
			if (EnumsHelper::Has(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE)) {
				EnumsHelper::Add(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_VISIBLE);
			} else {
				EnumsHelper::Rm(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_VISIBLE);
			}
			// process indirect visible
			if (EnumsHelper::Has(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE)) {
				EnumsHelper::Add(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_HIERARCHICALHIDE);
			} else {
				EnumsHelper::Rm(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_HIERARCHICALHIDE);
			}

			// read associated CKPlace
			if (EnumsHelper::Has(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_PLACEVALID)) {
				// MARK: we drop the support of CKPlace.
				// so we just read it and skip it.
				CK_ID placeid;
				chunk->ReadObjectID(placeid);
				// and remove this flag
				EnumsHelper::Rm(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_PLACEVALID);
			}

			// read parent
			if (EnumsHelper::Has(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_PARENTVALID)) {
				// MAKR: we drop the support of parent and the whole 3dentity hierarchy system
				// we ignore this field.
				CK_ID parentid;
				chunk->ReadObjectID(parentid);
				// and remove this flag
				EnumsHelper::Rm(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_PARENTVALID);
			}

			// read priority (non-zero zorder)
			if (EnumsHelper::Has(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_ZORDERVALID)) {
				chunk->ReadStruct(m_ZOrder);
			}
			
		} 
		// MARK: compatibility alternative core data read code removed because I don't need them

		
		// MARK: skin and bone are skipped.

		return true;
	}

	void CK3dEntity::Show(CK_OBJECT_SHOWOPTION show) {
		CKObject::Show(show);

		EnumsHelper::Rm(m_MoveableFlags, EnumsHelper::Merge({
			VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_VISIBLE,
			VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_HIERARCHICALHIDE,
			}));
		switch (show) {
			case CK_OBJECT_SHOWOPTION::CKSHOW:
				EnumsHelper::Add(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_VISIBLE);
				break;
			case CK_OBJECT_SHOWOPTION::CKHIERARCHICALHIDE:
				EnumsHelper::Add(m_MoveableFlags, VxMath::VX_MOVEABLE_FLAGS::VX_MOVEABLE_HIERARCHICALHIDE);
				break;
			case CK_OBJECT_SHOWOPTION::CKHIDE:
				break;
		}
	}

	bool CK3dEntity::IsVisible() const {
		// MARK: originally there is a call to this->IsHiddenByParent.
		// but we drop the support of parent, so we drop that condition.
		return CKObject::IsVisible();
	}

#pragma region Misc Oper

	const VxMath::VxMatrix& CK3dEntity::GetWorldMatrix() const {
		return m_WorldMatrix;
	}

	void CK3dEntity::SetWorldMatrix(const VxMath::VxMatrix& mat) {
		m_WorldMatrix = mat;
	}

	CK_3DENTITY_FLAGS CK3dEntity::GetEntityFlags() const {
		return m_3dEntityFlags;
	}

	void CK3dEntity::SetEntityFlags(CK_3DENTITY_FLAGS flags) {
		m_3dEntityFlags = flags;
	}

	VxMath::VX_MOVEABLE_FLAGS CK3dEntity::GetMoveableFlags() const {
		return m_MoveableFlags;
	}

	void CK3dEntity::SetMoveableFlags(VxMath::VX_MOVEABLE_FLAGS flags) {
		m_MoveableFlags = flags;
	}

	CKDWORD CK3dEntity::GetZOrder() const {
		return m_ZOrder;
	}

	void CK3dEntity::SetZOrder(CKDWORD ord) {
		m_ZOrder = ord;
	}

#pragma endregion

#pragma region Mesh Oper

	void CK3dEntity::AddPotentialMesh(CKMesh* mesh) {
		XContainer::NSXObjectPointerArray::AddIfNotHere(m_PotentialMeshes, mesh);
	}

	void CK3dEntity::RemovePotentialMesh(CKMesh* mesh) {
		std::erase(m_PotentialMeshes, mesh);
	}

	CKDWORD CK3dEntity::GetPotentialMeshCount() const {
		return static_cast<CKDWORD>(m_PotentialMeshes.size());
	}

	CKMesh* CK3dEntity::GetPotentialMesh(CKDWORD idx) const {
		if (idx >= m_PotentialMeshes.size()) return nullptr;
		return static_cast<CKMesh*>(m_PotentialMeshes[idx]);
	}

	CKMesh* CK3dEntity::GetCurrentMesh() const {
		return m_CurrentMesh;
	}

	void CK3dEntity::SetCurrentMesh(CKMesh* mesh) {
		m_CurrentMesh = mesh;
		AddPotentialMesh(mesh);
	}

#pragma endregion


}
