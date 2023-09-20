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
- IsHiddenByParent()
- CanBeHide()
- IsVisible()

- CheckPreDeletion()
- CheckPostDeletion()

No implement because don't care:
- GetMemoryOccupation()
- IsObjectUsed()
- PrepareDependencies()
- RemapDependencies()

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
		bool IsHierarchicallyHide() const;
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
		 * @brief Returns whether this object is hidden (and also hides its children).
		 * @return true if hierarchically hidden.
		 * @remark
		 * + This methods returns if this object is hidden and also hides all its sub-hierarchy.
		 * + See CKObject::Show¡ì for more details on hierarchically hidden objects.
		*/
		virtual bool IsHiddenByParent() const;
		/**
		 * @brief Returns whether this object class allows it to be shown or hidden.
		 * @return 3 possible value according to its type.
		 * + CKCANNOTHIDE if the object cannot be hidden
		 * + CKCANHIDE if the object can be hidden
		 * + CKCANHIERARCHICALHIDE if the object can be hidden and hierarchically hidden
		 * @remark
		 * + This virtual function is mainly used by the Virtools interface to known if visibility flags have an impact on a specific object class.
		 * + CKRenderObject and derived classes,can be triggered as to be shown or hidden or hierarchically hidden and will return 2 to this function.
		 * + CKMesh and CKGroup can be triggered as to be shown or hidden and will return 1 to this function.
		 * + Other classes will return 0 which means CKObject::Show function will not have any impact on them.

		*/
		virtual CK_OBJECT_CANBEHIDE CanBeHide() const;
		virtual bool IsVisible() const;

	protected:
		CK_ID m_ID;
		XContainer::XString m_Name;
		CK_OBJECT_FLAGS m_ObjectFlags;
		CKContext* m_Context;
	};

}
