#pragma once

#include "../../VTInternal.hpp"
#include "CKSceneObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKBeObject : public CKSceneObject {
	public:
		CKBeObject(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKBeObject();
		YYCC_DELETE_COPY_MOVE(CKBeObject)

		virtual CK_CLASSID GetClassID() override { 
			return CK_CLASSID::CKCID_BEOBJECT; 
		}

		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;

		/**
		 * @brief Check whether this object is in specified group.
		 * @param group[in] The group to be checked.
		 * @return True if in it.
		*/
		bool IsInGroup(CKGroup* group) const;
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