#pragma once

#include "../../VTAll.hpp"
#include "CKBeObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKGroup : public CKBeObject {
	public:
		CKGroup(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKGroup();
		LIBCMO_DISABLE_COPY_MOVE(CKGroup);
		
		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_GROUP; 
		}
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

		CKDWORD GetGroupIndex();
		
		// ===== Insert =====
		CKERROR AddObject(CKBeObject *o);
		
		// ===== Remove =====
		CKBeObject* RemoveObject(CKDWORD pos);
		void RemoveObject(CKBeObject *obj);
		void Clear();
		
		// ===== Access =====
		CKBeObject* GetObject(CKDWORD pos);
		CKDWORD GetObjectCount();

	protected:
		XContainer::XObjectPointerArray m_ObjectArray;
		CKDWORD m_GroupIndex;
	};

}
