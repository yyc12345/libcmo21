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
			chunk->ReadStruct(m_Flags);
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
		VertexSaveFlags saveflags = VertexSaveFlags::None;
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHVERTICES)) {
			// read and set vertex count
			CKDWORD vertexCount;
			chunk->ReadStruct(vertexCount);
			SetVertexCount(vertexCount);

			if (vertexCount != 0) {
				// read save flags
				chunk->ReadStruct(saveflags);

				// read size in dword (including it self)
				CKDWORD sizeInDword;
				chunk->ReadStruct(sizeInDword);
				--sizeInDword; // remove self.

				// lock read buffer
				auto buf = chunk->LockReadBufferWrapper(sizeInDword * CKSizeof(CKDWORD));
				const CKBYTE* rawbuf = static_cast<const CKBYTE*>(buf.get());

				// copy position if it have
				if (!EnumsHelper::Has(saveflags, VertexSaveFlags::NoPos)) {
					CKDWORD consumed = CKSizeof(VxMath::VxVector3) * vertexCount;
					std::memcpy(m_VertexPosition.data(), rawbuf, consumed);
					rawbuf += consumed;
				}

				// copy color or apply single color
				if (!EnumsHelper::Has(saveflags, VertexSaveFlags::SingleColor)) {
					CKDWORD consumed = CKSizeof(CKDWORD) * vertexCount;
					std::memcpy(m_VertexColor.data(), rawbuf, consumed);
					rawbuf += consumed;
				} else {
					VxMath::VxCopyStructure(
						vertexCount,
						m_VertexColor.data(),
						CKSizeof(CKDWORD),
						CKSizeof(CKDWORD),
						rawbuf,
						0	// InStride = 0 to make sure copy this single value to every elements.
					);
					rawbuf += CKSizeof(CKDWORD);
				}

				// copy specular color or apply a single color
				if (!EnumsHelper::Has(saveflags, VertexSaveFlags::SingleSpecularColor)) {
					CKDWORD consumed = CKSizeof(CKDWORD) * vertexCount;
					std::memcpy(m_VertexSpecularColor.data(), rawbuf, consumed);
					rawbuf += consumed;
				} else {
					VxMath::VxCopyStructure(
						vertexCount,
						m_VertexSpecularColor.data(),
						CKSizeof(CKDWORD),
						CKSizeof(CKDWORD),
						rawbuf,
						0	// InStride = 0 to make sure copy this single value to every elements.
					);
					rawbuf += CKSizeof(CKDWORD);
				}

				// copy normals if it has
				if (!EnumsHelper::Has(saveflags, VertexSaveFlags::NoNormal)) {
					CKDWORD consumed = CKSizeof(VxMath::VxVector3) * vertexCount;
					std::memcpy(m_VertexNormal.data(), rawbuf, consumed);
					rawbuf += consumed;
				}

				// copy uv or apply single uv
				if (!EnumsHelper::Has(saveflags, VertexSaveFlags::SingleUV)) {
					CKDWORD consumed = CKSizeof(VxMath::VxVector2) * vertexCount;
					std::memcpy(m_VertexUV.data(), rawbuf, consumed);
					rawbuf += consumed;
				} else {
					VxMath::VxCopyStructure(
						vertexCount,
						m_VertexUV.data(),
						CKSizeof(VxMath::VxVector2),
						CKSizeof(VxMath::VxVector2),
						rawbuf,
						0	// InStride = 0 to make sure copy this single value to every elements.
					);
					rawbuf += CKSizeof(VxMath::VxVector2);
				}

				// free buf
				buf.reset();
			}
		}

		// read face data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHFACES)) {
			// read face count and set
			CKDWORD faceCount;
			chunk->ReadStruct(faceCount);
			SetFaceCount(faceCount);

			// lock buffer
			auto buf = chunk->LockReadBufferWrapper(faceCount * CKSizeof(CKDWORD) * 2);
			const CKWORD* rawbuf = static_cast<const CKWORD*>(buf.get());
			// each face use 2 CKDWORD to describe
			// first CKDWORD describe first 2 face vertex indices
			// HIGH >>> 0xFFFF(indice 1) 0xFFFF(indice 0) <<< LOW
			// second CKDWORD describe the third indices and used material slot index
			// HIGH >>> 0xFFFF(mtl slot index) 0xFFFF(indice 2) <<< LOW
			
			// due to little endian, the data listed before are placed in memory like this:
			// (indice 0) (indice 1) (indice 2) (mtl idx)

			// copy indice
			VxMath::VxCopyStructure(
				faceCount,
				m_FaceIndices.data(),
				3 * CKSizeof(CKWORD),
				3 * CKSizeof(CKWORD),
				rawbuf,
				2 * CKSizeof(CKDWORD)
			);
			// copy mtl index
			VxMath::VxCopyStructure(
				faceCount,
				m_FaceMtlIndex.data(),
				CKSizeof(CKWORD),
				CKSizeof(CKWORD),
				rawbuf + 3,
				2 * CKSizeof(CKDWORD)
			);

			// free buf
			buf.reset();
		}

		// read line data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHLINES)) {
			// read and set line count;
			CKDWORD lineCount;
			chunk->ReadStruct(lineCount);
			SetLineCount(lineCount);

			chunk->ReadAndFillBuffer(m_LineIndices.data(), CKSizeof(CKWORD) * lineCount * 2);
		}

		// build normals
		if (EnumsHelper::Has(saveflags, VertexSaveFlags::NoNormal)) {
			BuildNormals();
		} else {
			BuildFaceNormals();
		}

		return true;
	}

	void CKMesh::Show(CK_OBJECT_SHOWOPTION show) {
		CKObject::Show(show);

		if (show == CK_OBJECT_SHOWOPTION::CKSHOW) {
			EnumsHelper::Add(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_VISIBLE);
		} else {
			EnumsHelper::Rm(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_VISIBLE);
		}
	}

#pragma region Misc Section

	void CKMesh::CleanMesh() {
		// clear material channel first
		SetMtlChannelCount(0);
		// then clear other
		SetVertexCount(0);
		SetMaterialSlotCount(0);
		SetFaceCount(0);
		SetLineCount(0);
	}

	void CKMesh::BuildNormals() {
		if (m_FaceCount == 0 || m_VertexCount == 0) return;

		// build face normal first
		BuildFaceNormals();

		// iterate all face and add face normal to each point's normal
		for (CKDWORD fid = 0; fid < m_FaceCount; ++fid) {
			m_VertexNormal[m_FaceIndices[fid * 3]] += m_Faces[fid].m_Normal;
			m_VertexNormal[m_FaceIndices[fid * 3 + 1]] += m_Faces[fid].m_Normal;
			m_VertexNormal[m_FaceIndices[fid * 3 + 2]] += m_Faces[fid].m_Normal;
		}

		// then normalize all vertex normal
		for (auto& nml : m_VertexNormal) {
			nml.Normalized();
		}
	}

	void CKMesh::BuildFaceNormals() {
		if (m_FaceCount == 0 || m_VertexCount == 0) return;

		// iertate all face to build face normal according to position data
		for (CKDWORD fid = 0; fid < m_FaceCount; ++fid) {
			VxMath::VxVector3 *p0 = &m_VertexPosition[m_FaceIndices[fid * 3]];

			VxMath::VxVector3 p0_p1 = m_VertexPosition[m_FaceIndices[fid * 3 + 1]] - *p0,
				p0_p2 = m_VertexPosition[m_FaceIndices[fid * 3 + 2]] - *p0;

			// cross product to get normal
			// and normalize it
			VxMath::VxVector3 nml = VxMath::NSVxVector::CrossProduct(p0_p1, p0_p2);
			nml.Normalized();

			// assign it
			m_Faces[fid].m_Normal = nml;
		}
	}

#pragma endregion

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

	CKFLOAT* CKMesh::GetVertexWeights() {
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
