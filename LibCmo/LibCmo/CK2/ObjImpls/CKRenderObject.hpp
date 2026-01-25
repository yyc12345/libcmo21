#pragma once

#include "../../VTInternal.hpp"
#include "CKBeObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKRenderObject : public CKBeObject {
	public:
		CKRenderObject(CKContext* ctx, CK_ID ckid, CKSTRING name) : 
			CKBeObject(ctx, ckid, name)
		{}
		virtual ~CKRenderObject() {}
		YYCC_DELETE_COPY_MOVE(CKRenderObject)

		virtual CK_CLASSID GetClassID() override { 
			return CK_CLASSID::CKCID_RENDEROBJECT; 
		}

		// CKRenderObject do not implement any load/save functions
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		//virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		//virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

	protected:

	};

}
