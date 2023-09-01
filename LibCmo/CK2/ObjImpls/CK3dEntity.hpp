#pragma once

#include "../../VTAll.hpp"
#include "CKRenderObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CK3dEntity : public CKRenderObject {
	public:
		CK3dEntity(CKContext* ctx, CK_ID ckid, CKSTRING name) : 
			CKRenderObject(ctx, ckid, name),
			m_Meshes(), m_CurrentMesh(nullptr), m_WorldMatrix()
		{}
		virtual ~CK3dEntity() {}
		LIBCMO_DISABLE_COPY_MOVE(CK3dEntity);

		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_3DENTITY; 
		}
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

	protected:
		XContainer::XObjectPointerArray m_Meshes;
		CKMesh* m_CurrentMesh;
		VxMath::VxMatrix m_WorldMatrix;
		
	};

}
