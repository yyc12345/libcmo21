#pragma once

#include "../../VTAll.hpp"
#include "CKBeObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKMesh : public CKBeObject {
	public:
		CKMesh(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKMesh();
		LIBCMO_DISABLE_COPY_MOVE(CKMesh);
		
		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_MESH; 
		}
		
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

	protected:

	};

}
