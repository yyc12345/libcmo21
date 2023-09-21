#include "BMap.hpp"

namespace BMap {

#pragma region BMMeshTransition

	BMMeshTransition::TransitionVertex::TransitionVertex(
		const LibCmo::VxMath::VxVector3& vec,
		const LibCmo::VxMath::VxVector3& norm,
		const LibCmo::VxMath::VxVector2& uv) :
		m_Vertex(vec), m_Norm(norm), m_UV(uv) {}

	BMMeshTransition::TransitionFace::TransitionFace(uint32_t _i1, uint32_t _i2, uint32_t _i3, uint32_t mtl_id) :
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

	void BMMeshTransition::PrepareVertexCount(uint32_t count) {
		if (m_IsParsed) return;
		m_Vertexs.resize(count);
		m_IsVertexOK = true;
	}

	void BMMeshTransition::PrepareVertex(uint32_t index, float x, float y, float z) {
		if (m_IsParsed || index >= m_Vertexs.size()) return;
		m_Vertexs[index].x = x;
		m_Vertexs[index].y = y;
		m_Vertexs[index].z = z;
	}

	void BMMeshTransition::PrepareNormalCount(uint32_t count) {
		if (m_IsParsed) return;
		m_Normals.resize(count);
		m_IsNormalOK = true;
	}

	void BMMeshTransition::PrepareNormal(uint32_t index, float x, float y, float z) {
		if (m_IsParsed || index >= m_Normals.size()) return;
		m_Normals[index].x = x;
		m_Normals[index].y = y;
		m_Normals[index].z = z;
	}

	void BMMeshTransition::PrepareUVCount(uint32_t count) {
		if (m_IsParsed) return;
		m_UVs.resize(count);
		m_IsUVOK = true;
	}

	void BMMeshTransition::PrepareUV(uint32_t index, float u, float v) {
		if (m_IsParsed || index >= m_UVs.size()) return;
		m_UVs[index].x = u;
		m_UVs[index].y = v;
	}

	void BMMeshTransition::PrepareMtlSlotCount(uint32_t count) {
		if (m_IsParsed) return;
		m_MtlSlots.resize(count, nullptr);
		m_IsMtlSlotOK = true;
	}

	void BMMeshTransition::PrepareMtlSlot(uint32_t index, BMMaterial* mtl) {
		if (m_IsParsed || index >= m_MtlSlots.size()) return;
		m_MtlSlots[index] = mtl;
	}

	void BMMeshTransition::PrepareFaceCount(uint32_t count) {
		if (m_IsParsed) return;
		m_FaceVertexs.resize(count * 3);
		m_FaceNormals.resize(count * 3);
		m_FaceUVs.resize(count * 3);
		m_FaceMtlSlotIdxs.resize(count);
		m_IsFaceOK = true;
	}

	void BMMeshTransition::PrepareFaceVertexIndices(uint32_t index, uint32_t indice1, uint32_t indice2, uint32_t indice3) {
		index *= 3;
		if (m_IsParsed || index >= m_FaceVertexs.size()) return;
		m_FaceVertexs[index] = indice1;
		m_FaceVertexs[index + 1] = indice2;
		m_FaceVertexs[index + 2] = indice3;
	}

	void BMMeshTransition::PrepareFaceNormalIndices(uint32_t index, uint32_t indice1, uint32_t indice2, uint32_t indice3) {
		index *= 3;
		if (m_IsParsed || index >= m_FaceNormals.size()) return;
		m_FaceNormals[index] = indice1;
		m_FaceNormals[index + 1] = indice2;
		m_FaceNormals[index + 2] = indice3;
	
	}

	void BMMeshTransition::PrepareFaceUVIndices(uint32_t index, uint32_t indice1, uint32_t indice2, uint32_t indice3) {
		index *= 3;
		if (m_IsParsed || index >= m_FaceUVs.size()) return;
		m_FaceUVs[index] = indice1;
		m_FaceUVs[index + 1] = indice2;
		m_FaceUVs[index + 2] = indice3;
	
	}

	void BMMeshTransition::PrepareFaceMtlSlot(uint32_t index, uint32_t mtl_slot) {
		if (m_IsParsed || index >= m_FaceMtlSlotIdxs.size()) return;
		m_FaceMtlSlotIdxs[index] = mtl_slot;
	}

	bool BMMeshTransition::Parse(BMMesh* write_into_mesh) {
		if (m_IsParsed || write_into_mesh == nullptr) return false;
		if (!m_IsVertexOK || !m_IsNormalOK || !m_IsUVOK || !m_IsFaceOK || !m_IsMtlSlotOK) return false;

		DoRealParse();
		ApplyToMesh(write_into_mesh);

		m_IsParsed = true;
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
			uint32_t idx[3];
			for (int j = 0; j < 3; ++j) {
				// create one first
				TransitionVertex tvec(
					m_Vertexs[m_FaceVertexs[faceid * 3 + j]],
					m_Normals[m_FaceVertexs[faceid * 3 + j]],
					m_UVs[m_FaceVertexs[faceid * 3 + j]]
				);

				// try insert it
				auto insert_result = m_ProcDupRemover.try_emplace(tvec, static_cast<uint32_t>(m_ProcVertexs.size()));
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

	void BMMeshTransition::ApplyToMesh(BMMesh* write_into_mesh) {
		// todo: apply to mesh
	}


#pragma endregion


}
