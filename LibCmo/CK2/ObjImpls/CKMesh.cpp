#include "CKMesh.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include "CKMaterial.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKMesh::CKMesh(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKBeObject(ctx, ckid, name),
		// init vertex
		m_VertexCount(0),
		m_VertexPosition(), m_VertexNormal(), m_VertexUV(),
		m_VertexColor(), m_VertexSpecularColor(),
		// init mtl slots
		m_MaterialSlotCount(0),
		m_MaterialSlot(),
		// init face data
		m_FaceCount(0),
		m_FaceIndices(), m_FaceMtlIndex(), m_FaceOthers(),
		// init line
		m_LineCount(0),
		m_LineIndices(),
		// init flags
		m_Flags(EnumsHelper::Merge({
			VxMath::VXMESH_FLAGS::VXMESH_VISIBLE,
			VxMath::VXMESH_FLAGS::VXMESH_RENDERCHANNELS
		})) {
		// set visible in default
		EnumsHelper::Add(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
	}

	CKMesh::~CKMesh() {}

	void CKMesh::CheckPreDeletion() {
		CKBeObject::CheckPreDeletion();

		// check material slots
		for (auto& slot : m_MaterialSlot) {
			if (slot != nullptr && slot->IsToBeDeleted()) {
				slot = nullptr;
			}
		}
	}

	bool CKMesh::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKBeObject::Save(chunk, file, flags);
		if (!suc) return false;

		// write mesh flags
		{
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHFLAGS);
			chunk->WriteStruct(m_Flags);
		}

		// write material slots
		// MARK: due to virtools shit implement, we must make sure there is at least one material channel existed.
		// so if the material slot is empty, we write a mullptr slot for it.
		{
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHMATERIALS);

			if (GetMaterialSlotCount() != 0) {
				// write real slots
				chunk->WriteStruct(GetMaterialSlotCount());
				for (auto& mtlSlot : m_MaterialSlot) {
					// write object id
					chunk->WriteObjectPointer(mtlSlot);
					// MARK: write a zero? idk what the fuck it is.
					chunk->WriteStruct(static_cast<CKDWORD>(0));
				}
			} else {
				// write fake one like real one
				chunk->WriteStruct(static_cast<CKDWORD>(1));
				// write id and blank
				chunk->WriteObjectPointer(nullptr);
				chunk->WriteStruct(static_cast<CKDWORD>(0));
			}
		}

		// write face data
		if (GetFaceCount() != 0) {
			CKDWORD faceCount = GetFaceCount();
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHFACES);
			chunk->WriteStruct(faceCount);

			// write compressed data, see Read for more info about this struct
			// lock buffer first
			auto buf = chunk->LockWriteBufferWrapper(faceCount * CKSizeof(CKDWORD) * 2);
			CKWORD* rawbuf = static_cast<CKWORD*>(buf.get());

			// copy indice
			VxMath::VxCopyStructure(
				faceCount,
				rawbuf,
				2 * CKSizeof(CKDWORD),
				3 * CKSizeof(CKWORD),
				m_FaceIndices.data(),
				3 * CKSizeof(CKWORD)
			);
			// copy mtl index
			VxMath::VxCopyStructure(
				faceCount,
				rawbuf + 3,
				2 * CKSizeof(CKDWORD),
				CKSizeof(CKWORD),
				m_FaceMtlIndex.data(),
				CKSizeof(CKWORD)
			);

			// free buf
			buf.reset();
		}

		// write line data
		if (GetLineCount() != 0) {
			CKDWORD lineCount = GetLineCount();
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHLINES);
			chunk->WriteStruct(lineCount);

			chunk->WriteBuffer(m_LineIndices.data(), CKSizeof(CKWORD) * 2 * lineCount);
		}

		// write vertex data
		if (GetVertexCount() != 0) {
			CKDWORD vtxCount = GetVertexCount();
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_MESH::CK_STATESAVE_MESHVERTICES);
			chunk->WriteStruct(vtxCount);

			// construct vertex save flags
			// and save it
			VertexSaveFlags saveflags = GenerateSaveFlags();
			chunk->WriteStruct(saveflags);

			// reserve enough space for full data written, but we can specify the real consumed size later
			// we also need calc the consumed size when writing file
			auto buf = chunk->LockWriteBufferWrapper((
				CKSizeof(VxMath::VxVector3) +	// vertex position
				CKSizeof(CKDWORD) + CKSizeof(CKDWORD) +		// color and specular color
				CKSizeof(VxMath::VxVector3) +	// vertex normal
				CKSizeof(VxMath::VxVector2)		// vertex uv
			) * vtxCount);	// mul vertex count
			CKBYTE* rawbuf = static_cast<CKBYTE*>(buf.get());
			
			// reserve length data
			CKDWORD* reservedBufDwordSize = reinterpret_cast<CKDWORD*>(rawbuf);
			rawbuf += CKSizeof(CKDWORD);

			// write vertex position
			if (!EnumsHelper::Has(saveflags, VertexSaveFlags::NoPos)) {
				CKDWORD consumed = CKSizeof(VxMath::VxVector3) * vtxCount;
				std::memcpy(rawbuf, m_VertexPosition.data(), consumed);
				rawbuf += consumed;
			}

			// write color and specular color
			{
				CKDWORD consumed = 0;
				if (!EnumsHelper::Has(saveflags, VertexSaveFlags::SingleColor)) {
					consumed = CKSizeof(CKDWORD) * vtxCount;
				} else {
					consumed = CKSizeof(CKDWORD);
				}

				std::memcpy(rawbuf, m_VertexColor.data(), consumed);
				rawbuf += consumed;
			}
			{
				CKDWORD consumed = 0;
				if (!EnumsHelper::Has(saveflags, VertexSaveFlags::SingleSpecularColor)) {
					consumed = CKSizeof(CKDWORD) * vtxCount;
				} else {
					consumed = CKSizeof(CKDWORD);
				}

				std::memcpy(rawbuf, m_VertexSpecularColor.data(), consumed);
				rawbuf += consumed;
			}

			// write normal
			if (!EnumsHelper::Has(saveflags, VertexSaveFlags::NoNormal)) {
				CKDWORD consumed = CKSizeof(VxMath::VxVector3) * vtxCount;
				std::memcpy(rawbuf, m_VertexNormal.data(), consumed);
				rawbuf += consumed;
			}

			// write uv
			{
				CKDWORD consumed = 0;
				if (!EnumsHelper::Has(saveflags, VertexSaveFlags::SingleUV)) {
					consumed = CKSizeof(VxMath::VxVector2) * vtxCount;
				} else {
					consumed = CKSizeof(VxMath::VxVector2);
				}

				std::memcpy(rawbuf, m_VertexUV.data(), consumed);
				rawbuf += consumed;
			}

			// calc real consumed size
			CKDWORD realConsumedSize = static_cast<CKDWORD>(rawbuf - static_cast<CKBYTE*>(buf.get()));
			// assign to reserved length field
			// length also include length indicator it self
			*reservedBufDwordSize = realConsumedSize / CKSizeof(CKDWORD);
			// notify buffer real consumed size
			buf.get_deleter().SetConsumedSize(realConsumedSize);

			// free buffer
			buf.reset();

		}

		chunk->SetClassId(CK_CLASSID::CKCID_MESH);
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
			CKDWORD ph;
			CKObject* objptr = nullptr;
			for (auto& mtlSlot : m_MaterialSlot) {
				// read id
				chunk->ReadObjectPointer(objptr);
				// and read a place holder idk what the fuck it is.
				chunk->ReadStruct(ph);

				// try to assign
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

			chunk->ReadAndFillBuffer(m_LineIndices.data());
		}

		// build normals
		if (EnumsHelper::Has(saveflags, VertexSaveFlags::NoNormal)) {
			BuildNormals();
		} else {
			BuildFaceNormals();
		}

		// MARK: material channels, vertex weight, face mask added originally 
		// but removed at Oct 1st, 2023 because I will not use them and I couldn't test them.

		// MARK: progressive mesh data is dropper.

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
		SetVertexCount(0);
		SetMaterialSlotCount(0);
		SetFaceCount(0);
		SetLineCount(0);
	}

	VxMath::VXMESH_FLAGS CKMesh::GetMeshFlags() const {
		return m_Flags;
	}

	void CKMesh::SetMeshFlags(VxMath::VXMESH_FLAGS flags) {
		// set value
		m_Flags = flags;

		// sync visibility to CKObject layer.
		if (EnumsHelper::Has(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_VISIBLE)) {
			EnumsHelper::Add(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
		} else {
			EnumsHelper::Rm(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
		}
	}

	VxMath::VXMESH_LITMODE CKMesh::GetLitMode() const {
		if (EnumsHelper::Has(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_PRELITMODE)) {
			return VxMath::VXMESH_LITMODE::VX_PRELITMESH;
		} else {
			return VxMath::VXMESH_LITMODE::VX_LITMESH;
		}
	}

	void CKMesh::SetLitMode(VxMath::VXMESH_LITMODE mode) {
		switch (mode) {
			case VxMath::VXMESH_LITMODE::VX_PRELITMESH:
				EnumsHelper::Add(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_PRELITMODE);
				break;
			case VxMath::VXMESH_LITMODE::VX_LITMESH:
				EnumsHelper::Rm(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_PRELITMODE);
				break;
		}
	}

	VxMath::VXTEXTURE_WRAPMODE CKMesh::GetWrapMode() const {
		VxMath::VXTEXTURE_WRAPMODE ret = VxMath::VXTEXTURE_WRAPMODE::VXTEXTUREWRAP_NONE;

		if (EnumsHelper::Has(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_WRAPU)) {
			EnumsHelper::Add(ret, VxMath::VXTEXTURE_WRAPMODE::VXTEXTUREWRAP_U);
		}
		if (EnumsHelper::Has(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_WRAPV)) {
			EnumsHelper::Add(ret, VxMath::VXTEXTURE_WRAPMODE::VXTEXTUREWRAP_V);
		}

		return ret;
	}

	void CKMesh::SetWrapMode(VxMath::VXTEXTURE_WRAPMODE mode) {
		if (EnumsHelper::Has(mode, VxMath::VXTEXTURE_WRAPMODE::VXTEXTUREWRAP_U)) {
			EnumsHelper::Add(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_WRAPU);
		} else {
			EnumsHelper::Rm(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_WRAPU);
		}

		if (EnumsHelper::Has(mode, VxMath::VXTEXTURE_WRAPMODE::VXTEXTUREWRAP_V)) {
			EnumsHelper::Add(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_WRAPV);
		} else {
			EnumsHelper::Rm(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_WRAPV);
		}
	}

	CKMesh::VertexSaveFlags CKMesh::GenerateSaveFlags() {
		// set to initial status
		VertexSaveFlags saveflags = EnumsHelper::Merge({
			VertexSaveFlags::SingleColor,
			VertexSaveFlags::SingleSpecularColor,
			VertexSaveFlags::NoNormal,
			VertexSaveFlags::SingleUV
			});

		// check no pos
		// if position is generated, skip saving position
		if (EnumsHelper::Has(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_PROCEDURALPOS)) {
			EnumsHelper::Add(saveflags, VertexSaveFlags::NoPos);
		}

		// check uv
		// if uv is not generated and all uv are not the same value, remove single uv
		if (!EnumsHelper::Has(m_Flags, VxMath::VXMESH_FLAGS::VXMESH_PROCEDURALUV)) {
			for (const auto& uv : m_VertexUV) {
				if (uv != m_VertexUV.front()) {
					EnumsHelper::Rm(saveflags, VertexSaveFlags::SingleUV);
					break;
				}
			}
		}

		// check color and specular color
		// if all color are not the same value, remove single color
		for (const auto& col : m_VertexColor) {
			if (col != m_VertexColor.front()) {
				EnumsHelper::Rm(saveflags, VertexSaveFlags::SingleColor);
				break;
			}
		}
		for (const auto& col : m_VertexSpecularColor) {
			if (col != m_VertexSpecularColor.front()) {
				EnumsHelper::Rm(saveflags, VertexSaveFlags::SingleSpecularColor);
				break;
			}
		}

		// if normal not changed, and position is not generated, we should consider whether we need save normal (step into if)
		if (!EnumsHelper::Has(m_Flags, EnumsHelper::Merge({ VxMath::VXMESH_FLAGS::VXMESH_NORMAL_CHANGED, VxMath::VXMESH_FLAGS::VXMESH_PROCEDURALPOS }))) {
			// MARK: we should build face normal first
			// then we build vertex normal like BuildNormals.
			// then, we compare the difference between the generated normals and user specified normals, by simply using operator- (userNml - generatedNml) and abs the result.
			// then we accumulate these difference, by simply adding them together.
			// then we div the accumulation by the count of vertex, we got a normalized accumulated difference.
			// we compare its length with 0.001. if is length is lower than 0.001, it prove that the difference is enough small and we can skip normal save.
			// othersize we should save normal one by one.

			BuildFaceNormals();

			// init generated nml list first
			XContainer::XArray<VxMath::VxVector3> generated(m_VertexCount, VxMath::VxVector3());
			// and accumulated for each normal
			for (CKDWORD fid = 0; fid < m_FaceCount; ++fid) {
				generated[m_FaceIndices[fid * 3]] += m_FaceOthers[fid].m_Normal;
				generated[m_FaceIndices[fid * 3 + 1]] += m_FaceOthers[fid].m_Normal;
				generated[m_FaceIndices[fid * 3 + 2]] += m_FaceOthers[fid].m_Normal;
			}

			// init accumulated difference vector first
			VxMath::VxVector3 accnml;
			// accumulate difference
			for (CKDWORD vid = 0; vid < m_VertexCount; ++vid) {
				// normalize generated normal first
				generated[vid].Normalized();
				// get diff by distance
				VxMath::VxVector3 diff = m_VertexNormal[vid] - generated[vid];
				// abs the diff and add into accumulated diff
				VxMath::NSVxVector::Abs(diff);
				accnml += diff;
			}
			
			// div by vertex count and compare its length
			accnml /= static_cast<CKFLOAT>(m_VertexCount);
			if (accnml.Length() > 0.001f) {
				// too large difference, we need save normal
				EnumsHelper::Rm(saveflags, VertexSaveFlags::NoNormal);
			}

		}

		return saveflags;
	}

	void CKMesh::BuildNormals() {
		if (m_FaceCount == 0 || m_VertexCount == 0) return;

		// build face normal first
		BuildFaceNormals();

		// iterate all face and add face normal to each point's normal
		for (CKDWORD fid = 0; fid < m_FaceCount; ++fid) {
			m_VertexNormal[m_FaceIndices[fid * 3]] += m_FaceOthers[fid].m_Normal;
			m_VertexNormal[m_FaceIndices[fid * 3 + 1]] += m_FaceOthers[fid].m_Normal;
			m_VertexNormal[m_FaceIndices[fid * 3 + 2]] += m_FaceOthers[fid].m_Normal;
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
			m_FaceOthers[fid].m_Normal = nml;
		}
	}

#pragma endregion

#pragma region Vertex Section

	CKDWORD CKMesh::GetVertexCount() const {
		return m_VertexCount;
	}

	void CKMesh::SetVertexCount(CKDWORD count) {
		m_VertexCount = count;
		m_VertexPosition.resize(count);
		m_VertexNormal.resize(count);
		m_VertexUV.resize(count);
		m_VertexColor.resize(count, 0xFFFFFFFF);
		m_VertexSpecularColor.resize(count, 0x00000000);
	}

	VxMath::VxVector3* CKMesh::GetVertexPositions() {
		if (m_VertexCount == 0) return nullptr;
		return m_VertexPosition.data();
	}

	VxMath::VxVector3* CKMesh::GetVertexNormals() {
		if (m_VertexCount == 0) return nullptr;
		return m_VertexNormal.data();
	}

	VxMath::VxVector2* CKMesh::GetVertexUVs() {
		if (m_VertexCount == 0) return nullptr;
		return m_VertexUV.data();
	}

	CKDWORD* CKMesh::GetVertexColors() {
		if (m_VertexCount == 0) return nullptr;
		return m_VertexColor.data();
	}

	CKDWORD* CKMesh::GetVertexSpecularColors() {
		if (m_VertexCount == 0) return nullptr;
		return m_VertexSpecularColor.data();
	}

#pragma endregion

#pragma region Material Slot Section

	CKDWORD CKMesh::GetMaterialSlotCount() const {
		return m_MaterialSlotCount;
	}

	void CKMesh::SetMaterialSlotCount(CKDWORD count) {
		m_MaterialSlotCount = count;
		m_MaterialSlot.resize(count, nullptr);
	}

	CKMaterial** CKMesh::GetMaterialSlots() {
		if (m_MaterialSlotCount == 0) return nullptr;
		return m_MaterialSlot.data();
	}

#pragma endregion

#pragma region Face Section

	CKDWORD CKMesh::GetFaceCount() const {
		return m_FaceCount;
	}

	void CKMesh::SetFaceCount(CKDWORD count) {
		m_FaceCount = count;
		m_FaceIndices.resize(count * 3, 0);
		m_FaceMtlIndex.resize(count, 0);
		m_FaceOthers.resize(count);
	}

	CKWORD* CKMesh::GetFaceIndices() {
		if (m_FaceCount == 0) return nullptr;
		return m_FaceIndices.data();
	}

	CKWORD* CKMesh::GetFaceMaterialSlotIndexs() {
		if (m_FaceCount == 0) return nullptr;
		return m_FaceMtlIndex.data();
	}

	VxMath::VxVector3* CKMesh::GetFaceNormals(CKDWORD& stride) {
		stride = CKSizeof(FaceData_t);

		if (m_FaceCount == 0) return nullptr;
		return &m_FaceOthers.data()->m_Normal;
	}

#pragma endregion

#pragma region Line Section

	CKDWORD CKMesh::GetLineCount() const {
		return m_LineCount;
	}

	void CKMesh::SetLineCount(CKDWORD count) {
		m_LineCount = count;
		m_LineIndices.resize(count * 2, 0);
	}

	CKWORD* CKMesh::GetLineIndices() {
		if (m_LineCount == 0) return nullptr;
		return m_LineIndices.data();
	}

#pragma endregion

}
