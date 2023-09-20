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

	bool CK3dEntity::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKRenderObject::Save(chunk, file, flags);
		if (!suc) return false;

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
			CK_ID currentMeshId;
			chunk->ReadObjectID(currentMeshId);
			CKObject* findobj = m_Context->GetObject(currentMeshId);
			if (findobj != nullptr && findobj->GetClassID() == CK_CLASSID::CKCID_MESH) {
				m_CurrentMesh = static_cast<CKMesh*>(findobj);
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
			// todo add if visible
			// and set or unset VX_MOVEABLE_VISIBLE
			
			// process indirect visible
			if (EnumsHelper::Has(m_ObjectFlags, CK_OBJECT_FLAGS::CKBEHAVIORLINK_ACTIVATEDLASTFRAME)) {
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
			}

			// read parent
			if (EnumsHelper::Has(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_PARENTVALID)) {
				// MAKR: we drop the support of parent and the whole 3dentity hierarchy system
				// we ignore this field.
				CK_ID parentid;
				chunk->ReadObjectID(parentid);
			}

			// read priority (non-zero zorder)
			if (EnumsHelper::Has(m_3dEntityFlags, CK_3DENTITY_FLAGS::CK_3DENTITY_ZORDERVALID)) {
				chunk->ReadStruct(m_ZOrder);
			}
			
		} else {
			// MARK: compatibility code removed because I don't need them
			return false;
		}
		
		// MARK: skin and bone are skipped.

		return true;
	}

}
