#pragma once

#include <VTUserAll.hpp>
#include <vector>
#include <cstdint>
#include <cinttypes>

namespace BMap {
	
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
			TransitionFace(LibCmo::CKDWORD _i1, LibCmo::CKDWORD _i2, LibCmo::CKDWORD _i3, LibCmo::CKDWORD mtl_id);
			LibCmo::CKDWORD m_Idx1, m_Idx2, m_Idx3;
			LibCmo::CKDWORD m_MtlSlotIdx;
		};
		struct TransitionVertexCompare {
			bool operator()(const TransitionVertex& lhs, const TransitionVertex& rhs) const;
		};

	public:
		BMMeshTransition();
		~BMMeshTransition();
		LIBCMO_DISABLE_COPY_MOVE(BMMeshTransition);

		void PrepareVertexCount(LibCmo::CKDWORD count);
		LibCmo::VxMath::VxVector3* PrepareVertex();
		void PrepareNormalCount(LibCmo::CKDWORD count);
		LibCmo::VxMath::VxVector3* PrepareNormal();
		void PrepareUVCount(LibCmo::CKDWORD count);
		LibCmo::VxMath::VxVector2* PrepareUV();
		void PrepareMtlSlotCount(LibCmo::CKDWORD count);
		LibCmo::CK2::ObjImpls::CKMaterial** PrepareMtlSlot();
		void PrepareFaceCount(LibCmo::CKDWORD count);
		LibCmo::CKDWORD* PrepareFaceVertexIndices();
		LibCmo::CKDWORD* PrepareFaceNormalIndices();
		LibCmo::CKDWORD* PrepareFaceUVIndices();
		LibCmo::CKDWORD* PrepareFaceMtlSlot();

		bool Parse(LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh);

	private:
		void DoRealParse();
		void ApplyToMesh(LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh);

		bool m_IsVertexOK, m_IsNormalOK, m_IsUVOK, m_IsFaceOK, m_IsMtlSlotOK;
		bool m_IsParsed;

		std::vector<LibCmo::VxMath::VxVector3> m_Vertexs, m_Normals;
		std::vector<LibCmo::VxMath::VxVector2> m_UVs;
		std::vector<LibCmo::CKDWORD> m_FaceVertexs, m_FaceNormals, m_FaceUVs;
		std::vector<LibCmo::CKDWORD> m_FaceMtlSlotIdxs;
		std::vector<LibCmo::CK2::ObjImpls::CKMaterial*> m_MtlSlots;

		std::vector<TransitionVertex> m_ProcVertexs;
		std::vector<TransitionFace> m_ProcFaces;
		/**
		@brief The core duplication vertex remover.
		@remark
		unordered_map have performance problem when dealing with massive data (in this case, big mesh).
		so we use map to get stable time cost.
		*/
		std::map<TransitionVertex, LibCmo::CKDWORD, TransitionVertexCompare> m_ProcDupRemover;
	};

	class BMFile {
	public:
		BMFile(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING encodings[], bool is_reader);
		~BMFile();
		LIBCMO_DISABLE_COPY_MOVE(BMFile);

		bool IsFailed();
		bool Load(LibCmo::CKSTRING filename);
		bool Save(LibCmo::CKSTRING filename, LibCmo::CKINT compress_level);

#define VISITOR_DECL(namepart) \
LibCmo::CKDWORD Get ## namepart ## Count(); \
LibCmo::CK2::ObjImpls::CK ## namepart * Get ## namepart (LibCmo::CKDWORD idx); \
LibCmo::CK2::ObjImpls::CK ## namepart * Create ## namepart (LibCmo::CKSTRING name);

		VISITOR_DECL(Group)
		VISITOR_DECL(3dObject)
		VISITOR_DECL(Mesh)
		VISITOR_DECL(Material)
		VISITOR_DECL(Texture)

#undef VISITOR_DECL

	private:
		bool m_IsFailed;
		bool m_IsReader;
		LibCmo::CK2::CKContext* m_Context;

#define VISITOR_SELF(namepart) \
std::vector<LibCmo::CK2::ObjImpls::CK ## namepart *> m_Obj ## namepart ## s;

		VISITOR_SELF(Group)
		VISITOR_SELF(3dObject)
		VISITOR_SELF(Mesh)
		VISITOR_SELF(Material)
		VISITOR_SELF(Texture)

#undef VISITOR_SELF
	};

}
