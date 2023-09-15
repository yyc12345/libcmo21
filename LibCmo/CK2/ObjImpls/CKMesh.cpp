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

		// clear 


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
