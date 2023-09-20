#pragma once

#include "../../VTAll.hpp"

/**
CKObject virtual functions implementations help

Implement as original meaning:
- PreSave()
- Save()
- Load()
- PostLoad()

- GetClassID()

- Show()
- IsVisible()

- CheckPreDeletion()
- CheckPostDeletion()

No implement because don't care:
- GetMemoryOccupation()
- IsObjectUsed()
- PrepareDependencies()
- RemapDependencies()

- IsHiddenByParent()
- CanBeHide()

Implement moved into other location:
- Copy(): Use CKObject::CKObject(CK_ID newid, const CKObject* obj) ctor and CKClassDesc to implement.
- PreDelete(): Write in dtor.

*/

namespace LibCmo::CK2::ObjImpls {

	class CKObject {
	public:
		CKObject(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKObject();
		LIBCMO_DISABLE_COPY_MOVE(CKObject);

		CK_ID GetID(void) const;
		CKSTRING GetName(void) const;
		void SetName(CKSTRING u8_name);
		CK_OBJECT_FLAGS GetObjectFlags(void) const;
		void SetObjectFlags(CK_OBJECT_FLAGS flags);
		CKContext* GetCKContext() const;

		virtual CK_CLASSID GetClassID(void) { 
			return CK_CLASSID::CKCID_OBJECT; 
		}

		virtual void CheckPreDeletion();
		virtual void CheckPostDeletion();

		virtual void PreSave(CKFileVisitor* file, CKDWORD flags);
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags);
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file);
		virtual void PostLoad();

		/**
		 * @brief Shows or hides an object
		 * @remark
		 * + If show is set to CKHIERARCHICALHIDE this object will be hidden along with all its children.
		 * + The render engine renders objets in a hierarchical way and stops iterating a hierarchy if it encounters an object which is hierarchically hidden. The problem is that, for performance reason, children objets visibility flags are left unchanged (ie. if a child object was visible CKObject::IsVisible will still return TRUE). To test is a object is hidden because one of its parent object is hierarchically hidden use CKObject::IsHiddenByParent
		 * + The CKHIERARCHICALHIDE option is only relevant for CK2dEntity and CK3dEntity derived classes.
		 * + If show is set to CKSHOW the object will be shown and it also removes the hierarchically hidden flags.
		 * + If show is set to CKHIDE the object will be hidden and it also removes the hierarchically hidden flags.
		 * + This function is overload by CKGroup,CKMesh and CK3dEntity.
		*/
		virtual void Show(CK_OBJECT_SHOWOPTION show = CK_OBJECT_SHOWOPTION::CKSHOW);
		/**
		 * @brief Returns whether this object is visible
		 * @return TRUE if the object is visible, FALSE otherwise
		 * @remark
		 * + Only CKRenderObject and derived classes(CK2dEntity,CK3dEntity),CKMesh and CKGroup return relevant information about their visibility state. Other classes may return any values.
		 * + An object can return CKSHOW and still be hidden if its parent (see CK3dEntity::GetParent and CK2dEntity::GetParent) is hierarchically hidden (see CKObject::Show)
		*/
		virtual bool IsVisible() const;

	protected:
		CK_ID m_ID;
		XContainer::XString m_Name;
		CK_OBJECT_FLAGS m_ObjectFlags;
		CKContext* m_Context;
	};

}
