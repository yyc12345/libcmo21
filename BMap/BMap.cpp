#include "BMap.hpp"

namespace BMap {

#pragma region BMMeshTransition

	BMMeshTransition::TransitionVertex::TransitionVertex(
		const LibCmo::VxMath::VxVector3& vec,
		const LibCmo::VxMath::VxVector3& norm,
		const LibCmo::VxMath::VxVector2& uv) :
		m_Vertex(vec), m_Norm(norm), m_UV(uv) {}

	BMMeshTransition::TransitionFace::TransitionFace(LibCmo::CKDWORD _i1, LibCmo::CKDWORD _i2, LibCmo::CKDWORD _i3, LibCmo::CKDWORD mtl_id) :
		m_Idx1(_i1), m_Idx2(_i2), m_Idx3(_i3), m_MtlSlotIdx(mtl_id) {}

	bool BMMeshTransition::TransitionVertexCompare::operator()(const TransitionVertex& lhs, const TransitionVertex& rhs) const {
		if (auto cmp = std::memcmp(&lhs.m_Vertex, &rhs.m_Vertex, sizeof(LibCmo::VxMath::VxVector3)); cmp != 0) return cmp < 0;
		if (auto cmp = std::memcmp(&lhs.m_Norm, &rhs.m_Norm, sizeof(LibCmo::VxMath::VxVector3)); cmp != 0) return cmp < 0;
		return std::memcmp(&lhs.m_UV, &rhs.m_UV, sizeof(LibCmo::VxMath::VxVector2)) < 0;
	}

	BMMeshTransition::BMMeshTransition() :
		m_IsVertexOK(false), m_IsNormalOK(false), m_IsUVOK(false), m_IsFaceOK(false), m_IsMtlSlotOK(false),
		m_IsParsed(false),
		m_Vertexs(), m_Normals(), m_UVs(),
		m_MtlSlots(),
		m_FaceVertexs(), m_FaceNormals(), m_FaceUVs(), m_FaceMtlSlotIdxs(),
		m_ProcVertexs(), m_ProcFaces(), m_ProcDupRemover() {}

	BMMeshTransition::~BMMeshTransition() {}

	void BMMeshTransition::PrepareVertexCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return;
		m_Vertexs.resize(count);
		m_IsVertexOK = true;
	}

	LibCmo::VxMath::VxVector3* BMMeshTransition::PrepareVertex() {
		if (m_IsParsed || !m_IsVertexOK) return nullptr;
		return m_Vertexs.data();
	}

	void BMMeshTransition::PrepareNormalCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return;
		m_Normals.resize(count);
		m_IsNormalOK = true;
	}

	LibCmo::VxMath::VxVector3* BMMeshTransition::PrepareNormal() {
		if (m_IsParsed || !m_IsNormalOK) return nullptr;
		return m_Normals.data();
	}

	void BMMeshTransition::PrepareUVCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return;
		m_UVs.resize(count);
		m_IsUVOK = true;
	}

	LibCmo::VxMath::VxVector2* BMMeshTransition::PrepareUV() {
		if (m_IsParsed || !m_IsUVOK) return nullptr;
		return m_UVs.data();
	}

	void BMMeshTransition::PrepareMtlSlotCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return;
		m_MtlSlots.resize(count, nullptr);
		m_IsMtlSlotOK = true;
	}

	LibCmo::CK2::ObjImpls::CKMaterial** BMMeshTransition::PrepareMtlSlot() {
		if (m_IsParsed || !m_IsMtlSlotOK) return nullptr;
		return m_MtlSlots.data();
	}

	void BMMeshTransition::PrepareFaceCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return;
		m_FaceVertexs.resize(count * 3);
		m_FaceNormals.resize(count * 3);
		m_FaceUVs.resize(count * 3);
		m_FaceMtlSlotIdxs.resize(count);
		m_IsFaceOK = true;
	}

	LibCmo::CKDWORD* BMMeshTransition::PrepareFaceVertexIndices() {
		if (m_IsParsed || !m_IsFaceOK) return nullptr;
		return m_FaceVertexs.data();
	}

	LibCmo::CKDWORD* BMMeshTransition::PrepareFaceNormalIndices() {
		if (m_IsParsed || !m_IsFaceOK) return nullptr;
		return m_FaceVertexs.data();
	}

	LibCmo::CKDWORD* BMMeshTransition::PrepareFaceUVIndices() {
		if (m_IsParsed || !m_IsFaceOK) return nullptr;
		return m_FaceVertexs.data();
	}

	LibCmo::CKDWORD* BMMeshTransition::PrepareFaceMtlSlot() {
		if (m_IsParsed || !m_IsFaceOK) return nullptr;
		return m_FaceVertexs.data();
	}

	bool BMMeshTransition::Parse(LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh) {
		if (m_IsParsed || write_into_mesh == nullptr) return false;
		if (!m_IsVertexOK || !m_IsNormalOK || !m_IsUVOK || !m_IsFaceOK || !m_IsMtlSlotOK) return false;
		m_IsParsed = true;

		// do parse
		DoRealParse();
		
		// check vertex overflow
		if (m_ProcVertexs.size() > std::numeric_limits<LibCmo::CKWORD>::max()) {
			return false;
		}
		// check mtl slot overflow
		if (m_MtlSlots.size() > std::numeric_limits<LibCmo::CKWORD>::max()) {
			return false;
		}

		// apply to mesh
		ApplyToMesh(write_into_mesh);

		return true;
	}

	void BMMeshTransition::DoRealParse() {
		// reserve vector to prevent extra mem alloc
		// use the most bad situation to reserve
		size_t face_size = m_FaceMtlSlotIdxs.size();
		m_ProcVertexs.reserve(face_size * 3);
		m_ProcFaces.reserve(face_size);

		// iterate face
		for (size_t faceid = 0; faceid < face_size; ++faceid) {
			LibCmo::CKDWORD idx[3];
			for (int j = 0; j < 3; ++j) {
				// create one first
				TransitionVertex tvec(
					m_Vertexs[m_FaceVertexs[faceid * 3 + j]],
					m_Normals[m_FaceVertexs[faceid * 3 + j]],
					m_UVs[m_FaceVertexs[faceid * 3 + j]]
				);

				// try insert it
				auto insert_result = m_ProcDupRemover.try_emplace(tvec, static_cast<LibCmo::CKDWORD>(m_ProcVertexs.size()));
				// get the new inserted index or existed index.
				idx[j] = insert_result.first->second;
				// if insert successfully, append to proc vertexs
				if (insert_result.second) {
					m_ProcVertexs.emplace_back(tvec);
				}
			}

			// create face
			m_ProcFaces.emplace_back(idx[0], idx[1], idx[2], m_FaceMtlSlotIdxs[faceid]);
		}
	}

	void BMMeshTransition::ApplyToMesh(LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh) {
		LibCmo::CKDWORD vec_count = static_cast<LibCmo::CKDWORD>(m_ProcVertexs.size()),
			face_count = static_cast<LibCmo::CKDWORD>(m_ProcFaces.size()),
			mtl_count = static_cast<LibCmo::CKDWORD>(m_MtlSlots.size());
		write_into_mesh->CleanMesh();

		// write vertex
		write_into_mesh->SetVertexCount(vec_count);
		LibCmo::VxMath::VxCopyStructure(
			vec_count,
			write_into_mesh->GetVertexPositions(),
			CKSizeof(LibCmo::VxMath::VxVector3),
			CKSizeof(LibCmo::VxMath::VxVector3),
			&m_ProcVertexs.data()->m_Vertex,
			CKSizeof(TransitionVertex)
		);
		LibCmo::VxMath::VxCopyStructure(
			vec_count,
			write_into_mesh->GetVertexNormals(),
			CKSizeof(LibCmo::VxMath::VxVector3),
			CKSizeof(LibCmo::VxMath::VxVector3),
			&m_ProcVertexs.data()->m_Norm,
			CKSizeof(TransitionVertex)
		);
		LibCmo::VxMath::VxCopyStructure(
			vec_count,
			write_into_mesh->GetVertexUVs(),
			CKSizeof(LibCmo::VxMath::VxVector2),
			CKSizeof(LibCmo::VxMath::VxVector2),
			&m_ProcVertexs.data()->m_UV,
			CKSizeof(TransitionVertex)
		);

		// write face
		write_into_mesh->SetFaceCount(face_count);
		auto pIndices = write_into_mesh->GetFaceIndices();
		auto pMtlIdx = write_into_mesh->GetFaceMaterialSlotIndexs();
		for (LibCmo::CKDWORD i = 0; i < face_count; ++i) {
			*(pIndices++) = static_cast<LibCmo::CKWORD>(m_ProcFaces[i].m_Idx1);
			*(pIndices++) = static_cast<LibCmo::CKWORD>(m_ProcFaces[i].m_Idx2);
			*(pIndices++) = static_cast<LibCmo::CKWORD>(m_ProcFaces[i].m_Idx3);

			*(pMtlIdx++) = static_cast<LibCmo::CKWORD>(m_ProcFaces[i].m_MtlSlotIdx);
		}

		// set mtl slot
		write_into_mesh->SetMaterialSlotCount(mtl_count);
		auto pMtlSlot = write_into_mesh->GetMaterialSlots();
		for (LibCmo::CKDWORD i = 0; i < mtl_count; ++i) {
			*(pMtlSlot++) = m_MtlSlots[i];
		}
	}


#pragma endregion

#pragma region BMfile

	BMFile::BMFile(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[], bool is_reader) {
	
	}

	BMFile::~BMFile() {}

	bool BMFile::Load(LibCmo::CKSTRING filename) {
		return false;
	}

	bool BMFile::Save(LibCmo::CKSTRING filename, LibCmo::CKINT compress_level) {
		return false;
	}

#define VISITOR_IMPL(namepart, cidpart) \
LibCmo::CKDWORD BMFile::Get ## namepart ## Count() { \
	if (!m_IsReader) return 0; \
	return static_cast<LibCmo::CKDWORD>(m_Obj ## namepart ## s.size()); \
} \
LibCmo::CK2::ObjImpls::CK ## namepart * BMFile::Get ## namepart (LibCmo::CKDWORD idx) { \
	if (!m_IsReader || idx >= m_Obj ## namepart ## s.size()) return nullptr; \
	return m_Obj ## namepart ## s[idx]; \
} \
LibCmo::CK2::ObjImpls::CK ## namepart * BMFile::Create ## namepart (LibCmo::CKSTRING name) { \
	if (m_IsReader) return nullptr; \
	LibCmo::CK2::ObjImpls::CK ## namepart * obj = static_cast<LibCmo::CK2::ObjImpls::CK ## namepart *>( \
		m_Context->CreateObject(LibCmo::CK2::CK_CLASSID::CKCID_ ## cidpart, name) \
		); \
	if (obj != nullptr) m_Obj ## namepart ## s.emplace_back(obj); \
	return obj; \
}
	
	VISITOR_IMPL(Group, GROUP)
	VISITOR_IMPL(3dObject, 3DOBJECT)
	VISITOR_IMPL(Mesh, MESH)
	VISITOR_IMPL(Material, MATERIAL)
	VISITOR_IMPL(Texture, TEXTURE)

#undef VISITOR_IMPL

#pragma endregion

}
