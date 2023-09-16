#pragma once

#include "../../VTAll.hpp"
#include "CKSceneObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKBeObject : public CKSceneObject {
	public:
		CKBeObject(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKBeObject();
		LIBCMO_DISABLE_COPY_MOVE(CKBeObject);

		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_BEOBJECT; 
		}
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

		bool IsInGroup(CKGroup* group);
		/**
		 * @brief Directly set group data.
		 * @param pos 
		 * @param val 
		 * @warning This function only should be called by CKGroup. Any other classes should not call this.
		*/
		void ExplicitSetGroup(CKDWORD pos, bool val);
		
	protected:
		XContainer::XBitArray m_Groups;
	};

}