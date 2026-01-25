#pragma once

#include "../../VTInternal.hpp"
#include "CKRenderObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CK3dEntity : public CKRenderObject {
	public:
		CK3dEntity(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CK3dEntity();
		YYCC_DELETE_COPY_MOVE(CK3dEntity)

		virtual CK_CLASSID GetClassID() override { 
			return CK_CLASSID::CKCID_3DENTITY; 
		}
		
		virtual void CheckPreDeletion() override;

		// 2 RW functions
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		
		// it have special Show and IsVisible method
		virtual void Show(CK_OBJECT_SHOWOPTION show = CK_OBJECT_SHOWOPTION::CKSHOW) override;
		virtual bool IsVisible() const override;

		const VxMath::VxMatrix& GetWorldMatrix() const;
		void SetWorldMatrix(const VxMath::VxMatrix& mat);
		CK_3DENTITY_FLAGS GetEntityFlags() const;
		void SetEntityFlags(CK_3DENTITY_FLAGS flags);
		VxMath::VX_MOVEABLE_FLAGS GetMoveableFlags() const;
		void SetMoveableFlags(VxMath::VX_MOVEABLE_FLAGS flags);
		CKDWORD GetZOrder() const;
		void SetZOrder(CKDWORD ord);

		void AddPotentialMesh(CKMesh* mesh);
		void RemovePotentialMesh(CKMesh* mesh);
		CKDWORD GetPotentialMeshCount() const;
		CKMesh* GetPotentialMesh(CKDWORD idx) const;
		CKMesh* GetCurrentMesh() const;
		void SetCurrentMesh(CKMesh* mesh);

	protected:
		XContainer::XObjectPointerArray m_PotentialMeshes;
		CKMesh* m_CurrentMesh;
		VxMath::VxMatrix m_WorldMatrix;
		CKDWORD m_ZOrder;	// replace the whole heavy CKSceneGraphNode
		
		VxMath::VX_MOVEABLE_FLAGS m_MoveableFlags;
		// MARK: This field is called m_EntityFlags in reverse project.
		// I change this because I want to give it a more explicit name to make it is different with other flags.
		CK_3DENTITY_FLAGS m_3dEntityFlags;

	};

}
