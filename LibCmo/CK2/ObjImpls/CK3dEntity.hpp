#pragma once

#include "../../VTAll.hpp"
#include "CKRenderObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CK3dEntity : public CKRenderObject {
	public:
		CK3dEntity(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CK3dEntity();
		LIBCMO_DISABLE_COPY_MOVE(CK3dEntity);

		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_3DENTITY; 
		}
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

	protected:
		XContainer::XObjectPointerArray m_PotentialMeshes;
		CKMesh* m_CurrentMesh;
		VxMath::VxMatrix m_WorldMatrix;
		CKDWORD m_ZOrder;	// replace the whole heavy CKSceneGraphNode
		
		VxMath::VX_MOVEABLE_FLAGS m_MoveableFlags;
		CK_3DENTITY_FLAGS m_3dEntityFlags;

	};

}
