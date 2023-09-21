#pragma once

#include <VTUserAll.hpp>
#include <vector>
#include <cstdint>
#include <cinttypes>

namespace BMap {
	
	class BMGroup {
	public:
		BMGroup(LibCmo::CK2::ObjImpls::CKGroup* ptr);
		~BMGroup();
		LIBCMO_DEFAULT_COPY_MOVE(BMGroup);

		LibCmo::CK2::ObjImpls::CKGroup* m_NativePtr;
	};
	
	class BMTexture {
	public:
		BMTexture(LibCmo::CK2::ObjImpls::CKTexture* ptr);
		~BMTexture();
		LIBCMO_DEFAULT_COPY_MOVE(BMTexture);

		LibCmo::CK2::ObjImpls::CKTexture* m_NativePtr;
	};
	
	class BMMaterial {
	public:
		BMMaterial(LibCmo::CK2::ObjImpls::CKMaterial* ptr);
		~BMMaterial();
		LIBCMO_DEFAULT_COPY_MOVE(BMMaterial);

		LibCmo::CK2::ObjImpls::CKMaterial* m_NativePtr;
	};
	
	class BMMesh {
	public:
		BMMesh(LibCmo::CK2::ObjImpls::CKMesh* ptr);
		~BMMesh();
		LIBCMO_DEFAULT_COPY_MOVE(BMMesh);

		LibCmo::CK2::ObjImpls::CKMesh* m_NativePtr;
	};
	
	class BM3dEntity {
	public:
		BM3dEntity(LibCmo::CK2::ObjImpls::CK3dEntity* ptr);
		~BM3dEntity();
		LIBCMO_DEFAULT_COPY_MOVE(BM3dEntity);

		LibCmo::CK2::ObjImpls::CK3dEntity* m_NativePtr;
	};
	
	class BMMeshTransition {
	private:
		struct TransitionVertex {
			TransitionVertex(
				const LibCmo::VxMath::VxVector3& vec,
				const LibCmo::VxMath::VxVector3& norm,
				const LibCmo::VxMath::VxVector2& uv);
			LibCmo::VxMath::VxVector3 m_Vertex;
			LibCmo::VxMath::VxVector3 m_Norm;
			LibCmo::VxMath::VxVector2 m_UV;
		};
		struct TransitionFace {
			TransitionFace(uint32_t _i1, uint32_t _i2, uint32_t _i3, uint32_t mtl_id);
			uint32_t m_Idx1, m_Idx2, m_Idx3;
			uint32_t m_MtlSlotIdx;
		};
		struct TransitionVertexCompare {
			bool operator()(const TransitionVertex& lhs, const TransitionVertex& rhs) const;
		};

	public:
		BMMeshTransition();
		~BMMeshTransition();

		void PrepareVertexCount(uint32_t count);
		void PrepareVertex(uint32_t index, float x, float y, float z);
		void PrepareNormalCount(uint32_t count);
		void PrepareNormal(uint32_t index, float x, float y, float z);
		void PrepareUVCount(uint32_t count);
		void PrepareUV(uint32_t index, float u, float v);
		void PrepareMtlSlotCount(uint32_t count);
		void PrepareMtlSlot(uint32_t index, BMMaterial* mtl);
		void PrepareFaceCount(uint32_t count);
		void PrepareFaceVertexIndices(uint32_t index, uint32_t indice1, uint32_t indice2, uint32_t indice3);
		void PrepareFaceNormalIndices(uint32_t index, uint32_t indice1, uint32_t indice2, uint32_t indice3);
		void PrepareFaceUVIndices(uint32_t index, uint32_t indice1, uint32_t indice2, uint32_t indice3);
		void PrepareFaceMtlSlot(uint32_t index, uint32_t mtl_slot);

		bool Parse(BMMesh* write_into_mesh);

	private:
		void DoRealParse();
		void ApplyToMesh(BMMesh* write_into_mesh);

		bool m_IsVertexOK, m_IsNormalOK, m_IsUVOK, m_IsFaceOK, m_IsMtlSlotOK;
		bool m_IsParsed;

		std::vector<LibCmo::VxMath::VxVector3> m_Vertexs, m_Normals;
		std::vector<LibCmo::VxMath::VxVector2> m_UVs;
		std::vector<uint32_t> m_FaceVertexs, m_FaceNormals, m_FaceUVs;
		std::vector<uint32_t> m_FaceMtlSlotIdxs;
		std::vector<BMMaterial*> m_MtlSlots;

		std::vector<TransitionVertex> m_ProcVertexs;
		std::vector<TransitionFace> m_ProcFaces;
		// unordered_map have performance problem when dealing with massive data (in this case, big mesh)
		// so we use map to get stable time cost.
		std::map<TransitionVertex, uint32_t, TransitionVertexCompare> m_ProcDupRemover;
	};

	class BMFile {
	public:
		BMFile();
		~BMFile();

	private:
		LibCmo::CK2::CKContext* m_Context;
		std::vector<LibCmo::CK2::ObjImpls::CKGroup*> m_ObjGroups;
		std::vector<LibCmo::CK2::ObjImpls::CK3dObject*> m_Obj3dObjects;
		std::vector<LibCmo::CK2::ObjImpls::CKMesh*> m_ObjMeshs;
		std::vector<LibCmo::CK2::ObjImpls::CKMaterial*> m_ObjMaterials;
		std::vector<LibCmo::CK2::ObjImpls::CKTexture*> m_ObjTextures;
	};

}
