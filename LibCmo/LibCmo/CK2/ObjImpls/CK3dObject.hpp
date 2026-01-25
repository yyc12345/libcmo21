#pragma once

#include "../../VTInternal.hpp"
#include "CK3dEntity.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CK3dObject : public CK3dEntity {
	public:
		CK3dObject(CKContext* ctx, CK_ID ckid, CKSTRING name) : 
			CK3dEntity(ctx, ckid, name)
		{}
		virtual ~CK3dObject() {}
		YYCC_DELETE_COPY_MOVE(CK3dObject)

		virtual CK_CLASSID GetClassID() override { 
			return CK_CLASSID::CKCID_3DOBJECT; 
		}
		// CK3dObject do not implement any load/save functions
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		//virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		//virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;
	protected:

	};

}
