#include "CKMesh.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include "CKMaterial.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKMesh::CKMesh(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKBeObject(ctx, ckid, name) {}

	CKMesh::~CKMesh() {}

	bool CKMesh::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKBeObject::Save(chunk, file, flags);
		if (!suc) return false;

		return true;
	}

	bool CKMesh::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKBeObject::Load(chunk, file);
		if (!suc) return false;

		// clear all data
		CleanMesh();

		// check data version.
		// MARK: too low data is not supported.
		// because my work are not related to them
		if (chunk->GetDataVersion() < CK_STATECHUNK_DATAVERSION::CHUNK_MESHCHANGE_VERSION) {
			return false;
		}

		// read flag
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHFLAGS)) {
			CKDWORD flags;
			chunk->ReadStruct(flags);

			m_Flags = static_cast<VxMath::VXMESH_FLAGS>(flags);
			EnumsHelper::Mask(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_ALLFLAGS);

			// I don't know why, just interpter the IDA code.
			EnumsHelper::Rm(m_Flags, EnumsHelper::Merge({
				VxMath::VXMESH_FLAGS::VXMESH_BOUNDINGUPTODATE,
				VxMath::VXMESH_FLAGS::VXMESH_OPTIMIZED
				}));
		}

		// read material slots
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHMATERIALS)) {
			// get and set material count
			CKDWORD mtlCount;
			chunk->ReadStruct(mtlCount);
			SetMaterialSlotCount(mtlCount);

			// read slot
			CK_ID mtlId;
			CKDWORD ph;
			CKObject* objptr;
			for (auto& mtlSlot : m_MaterialSlot) {
				// read id
				chunk->ReadObjectID(mtlId);
				// and read a place holder idk what the fuck it is.
				chunk->ReadStruct(ph);

				// try getting object pointer and assign
				objptr = m_Context->GetObject(mtlId);
				if (objptr != nullptr && objptr->GetClassID() == CK_CLASSID::CKCID_MATERIAL) {
					mtlSlot = static_cast<CKMaterial*>(objptr);
				} else {
					mtlSlot = nullptr;
				}
			}
		}

		// read vertex data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHVERTICES)) {
			// read and set vertex count
			CKDWORD vertexCount;
			chunk->ReadStruct(vertexCount);

			if (vertexCount != 0) {
				VertexSaveFlags saveflags;
				chunk->ReadStruct(saveflags);




			}
		}

		return true;
	}

	void CKMesh::CleanMesh() {
		// clear material channel first
		SetMtlChannelCount(0);
		// then clear other
		SetVertexCount(0);
		SetMaterialSlotCount(0);
		SetFaceCount(0);
		SetLineCount(0);
	}

#pragma region Vertex Section

	CKDWORD CKMesh::GetVertexCount() {
		return m_VertexCount;
	}

	void CKMesh::SetVertexCount(CKDWORD count) {
		m_VertexCount = count;
		m_VertexPosition.resize(count);
		m_VertexNormal.resize(count);
		m_VertexUV.resize(count);
		m_VertexColor.resize(count, 0xFFFFFFFF);
		m_VertexSpecularColor.resize(count, 0x00000000);
		m_VertexWeight.resize(count, 0.0f);

		// notify mtl channels refresh its custom uv
		SyncVertexCountToMtlChannel();
	}

	VxMath::VxVector3* CKMesh::GetVertexPositions() {
		return m_VertexPosition.data();
	}

	VxMath::VxVector3* CKMesh::GetVertexNormals() {
		return m_VertexNormal.data();
	}

	VxMath::VxVector2* CKMesh::GetVertexUVs() {
		return m_VertexUV.data();
	}

	CKDWORD* CKMesh::GetVertexColors() {
		return m_VertexColor.data();
	}

	CKDWORD* CKMesh::GetVertexSpecularColors() {
		return m_VertexSpecularColor.data();
	}

	float* CKMesh::GetVertexWeights() {
		return m_VertexWeight.data();
	}

#pragma endregion

#pragma region Material Slot Section

	CKDWORD CKMesh::GetMaterialSlotCount() {
		return m_MtlSlotCount;
	}

	void CKMesh::SetMaterialSlotCount(CKDWORD count) {
		m_MtlSlotCount = count;
		m_MaterialSlot.resize(count, nullptr);
	}

	void CKMesh::SetMaterialSlot(CKMaterial* mtl, CKDWORD idx) {
		if (idx >= m_MtlSlotCount) return;
		m_MaterialSlot[idx] = mtl;
	}

#pragma endregion

#pragma region Face Section

	CKDWORD CKMesh::GetFaceCount() {
		return m_FaceCount;
	}

	void CKMesh::SetFaceCount(CKDWORD count) {
		m_FaceCount = count;
		m_FaceIndices.resize(count * 3, 0);
		m_FaceMtlIndex.resize(count, 0);
		m_Faces.resize(count);
	}

	CKWORD* CKMesh::GetFaceIndices() {
		return m_FaceIndices.data();
	}

	CKWORD* CKMesh::GetFaceMaterialSlotIndexs() {
		return m_FaceIndices.data();
	}

	VxMath::VxVector3* CKMesh::GetFaceNormals(CKDWORD& stride) {
		stride = CKSizeof(FaceData_t);
		return &m_Faces.data()->m_Normal;
	}

	CKWORD* CKMesh::GetFaceChannelMasks(CKDWORD& stride) {
		stride = CKSizeof(FaceData_t);
		return &m_Faces.data()->m_ChannelMask;
	}

#pragma endregion

#pragma region Line Section

	CKDWORD CKMesh::GetLineCount() {
		return m_LineCount;
	}

	void CKMesh::SetLineCount(CKDWORD count) {
		m_LineCount = count;
		m_LineIndices.resize(count * 2, 0);
	}

	CKWORD* CKMesh::GetLineIndices() {
		return m_LineIndices.data();
	}

#pragma endregion

#pragma region Mtl Channel Section

	CKDWORD CKMesh::GetMtlChannelCount() {
		return m_MtlChannelCount;
	}

	void CKMesh::SetMtlChannelCount(CKDWORD count) {
		m_MtlChannelCount = count;
		m_MaterialChannels.resize(count);

		// sync mask to each face.
		// each face accept all mask in default
		SyncMtlChannelToFaceMask();
	}

	CKMaterial** CKMesh::GetMtlChannelMaterials(CKDWORD& stride) {
		stride = CKSizeof(MaterialChannel_t);
		return &m_MaterialChannels.data()->m_Material;
	}

	VxMath::VXBLEND_MODE* CKMesh::GetMtlChannelSourceBlends(CKDWORD& stride) {
		stride = CKSizeof(MaterialChannel_t);
		return &m_MaterialChannels.data()->m_SourceBlend;
	}

	VxMath::VXBLEND_MODE* CKMesh::GetMtlChannelDestBlends(CKDWORD& stride) {
		stride = CKSizeof(MaterialChannel_t);
		return &m_MaterialChannels.data()->m_DestBlend;
	}

	VxMath::VxVector2* CKMesh::GetMtlChannelCustomUVs(CKDWORD idx) {
		if (idx >= m_MtlChannelCount) return nullptr;
		return m_MaterialChannels[idx].m_CustomUV.data();
	}

	VxMath::VXCHANNEL_FLAGS CKMesh::GetMtlChannelFlags(CKDWORD idx) {
		if (idx >= m_MtlChannelCount) return static_cast<VxMath::VXCHANNEL_FLAGS>(0);
		return m_MaterialChannels[idx].m_Flags;
	}

	void CKMesh::SetMtlChannelFlags(CKDWORD idx, VxMath::VXCHANNEL_FLAGS flags) {
		if (idx >= m_MtlChannelCount) return;
		m_MaterialChannels[idx].m_Flags = flags;
		
		// refresh self custom uv
		SyncVertexCountToMtlChannel();
	}

	void CKMesh::SyncVertexCountToMtlChannel() {
		for (auto& channel : m_MaterialChannels) {
			if (!EnumsHelper::Has(channel.m_Flags, VxMath::VXCHANNEL_FLAGS::VXCHANNEL_SAMEUV)) {
				channel.m_CustomUV.resize(m_VertexCount);
			} else {
				channel.m_CustomUV.clear();
			}
		}
	}

	void CKMesh::SyncMtlChannelToFaceMask() {
		CKWORD mask = static_cast<CKWORD>(~(0xFFFF << m_MtlChannelCount));
		for (auto& face : m_Faces) {
			face.m_ChannelMask |= mask;
		}
	}

#pragma endregion


}
