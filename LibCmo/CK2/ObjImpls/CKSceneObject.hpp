#pragma once

#include "../../VTInternal.hpp"
#include "CKObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKSceneObject : public CKObject {
	public:
		CKSceneObject(CKContext* ctx, CK_ID ckid, CKSTRING name) : 
			CKObject(ctx, ckid, name),
			m_Scenes() {}
		virtual ~CKSceneObject() {}
		YYCC_DEL_CLS_COPY_MOVE(CKSceneObject);

		virtual CK_CLASSID GetClassID() override { 
			return CK_CLASSID::CKCID_SCENEOBJECT; 
		}
		// CKSceneObject do not override any RW functions.
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		//virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		//virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

	protected:
		XContainer::XBitArray m_Scenes;
	};

}