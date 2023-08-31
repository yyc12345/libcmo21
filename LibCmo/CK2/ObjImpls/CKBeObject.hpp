#pragma once

#include "../../VTAll.hpp"
#include "CKSceneObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKBeObject : public CKSceneObject {
	public:
		CKBeObject(CKContext* ctx, CK_ID ckid, CKSTRING name) : 
			CKSceneObject(ctx, ckid, name)
		{}
		virtual ~CKBeObject() {}
		LIBCMO_DISABLE_COPY_MOVE(CKBeObject);

		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_BEOBJECT; 
		}
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

		bool IsInGroup(CKGroup* group);
		
	protected:
		XContainer::XBitArray m_Groups;
	};

}