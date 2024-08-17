#pragma once

#include "../../VTInternal.hpp"
#include "CKBeObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKMesh : public CKBeObject {
	protected:
		enum class VertexSaveFlags : CKDWORD {
			None = 0,
			SingleColor = 0x1u,	/**< if not set, the VertexColor is a list, otherwise a single global CKDWORD.*/
			SingleSpecularColor = 0x2u,	/**< if not set, the VertexSpecularColor is a list, otherwise a single global CKDWORD. */
			NoNormal = 0x4u,	/**< if set, there are no normal data for vertex. */
			SingleUV = 0x8u,	/**< if not set, the VertexUV is a list, otherwise a single global VxVertex2. */
			NoPos = 0x10u,	/**< if set, there are no position data for vertex. */
		};

	public:
		CKMesh(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKMesh();
		YYCC_DEL_CLS_COPY_MOVE(CKMesh);
		
		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_MESH; 
		}
		
		virtual void CheckPreDeletion() override;

		// 2 RW functions
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;

		// it only have special Show method
		virtual void Show(CK_OBJECT_SHOWOPTION show = CK_OBJECT_SHOWOPTION::CKSHOW) override;

		// ===== Misc Section =====
	public:
		void CleanMesh();
		VxMath::VXMESH_FLAGS GetMeshFlags() const;
		void SetMeshFlags(VxMath::VXMESH_FLAGS flags);
		VxMath::VXMESH_LITMODE GetLitMode() const;
		void SetLitMode(VxMath::VXMESH_LITMODE mode);
		VxMath::VXTEXTURE_WRAPMODE GetWrapMode() const;
		void SetWrapMode(VxMath::VXTEXTURE_WRAPMODE mode);
	protected:
		VertexSaveFlags GenerateSaveFlags();
		void BuildNormals();
		void BuildFaceNormals();

		// ===== Vertex Section =====
	public:
		CKDWORD GetVertexCount() const;
		void SetVertexCount(CKDWORD count);
		VxMath::VxVector3* GetVertexPositions();
		VxMath::VxVector3* GetVertexNormals();
		VxMath::VxVector2* GetVertexUVs();
		CKDWORD* GetVertexColors();
		CKDWORD* GetVertexSpecularColors();
	
		// ===== Material Slot Section =====
	public:
		CKDWORD GetMaterialSlotCount() const;
		void SetMaterialSlotCount(CKDWORD count);
		CKMaterial** GetMaterialSlots();

		// ===== Face Section =====
	public:
		CKDWORD GetFaceCount() const;
		void SetFaceCount(CKDWORD count);
		CKWORD* GetFaceIndices();
		CKWORD* GetFaceMaterialSlotIndexs();
		VxMath::VxVector3* GetFaceNormals(CKDWORD& stride);

		// ===== Line Section =====
	public:
		CKDWORD GetLineCount() const;
		void SetLineCount(CKDWORD count);
		CKWORD* GetLineIndices();
		
	protected:
		struct FaceData_t {
			FaceData_t() :
				m_Normal()
			{}
			VxMath::VxVector3 m_Normal;
		};
		VxMath::VXMESH_FLAGS m_Flags;
		CKDWORD m_VertexCount;
		CKDWORD m_LineCount;
		CKDWORD m_MaterialSlotCount;
		CKDWORD m_FaceCount;

		XContainer::XArray<VxMath::VxVector3> m_VertexPosition;
		XContainer::XArray<VxMath::VxVector3> m_VertexNormal;
		XContainer::XArray<VxMath::VxVector2> m_VertexUV;
		XContainer::XArray<CKDWORD> m_VertexColor;
		XContainer::XArray<CKDWORD> m_VertexSpecularColor;

		XContainer::XArray<CKMaterial*> m_MaterialSlot;

		XContainer::XArray<CKWORD> m_FaceIndices;
		XContainer::XArray<CKWORD> m_FaceMtlIndex;
		XContainer::XArray<FaceData_t> m_FaceOthers;

		XContainer::XArray<CKWORD> m_LineIndices;

	};

}
