#include "CKObject.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKObject::CKObject(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		m_ID(ckid),
		m_Name(),
		m_Context(ctx),
		m_ObjectFlags(CK_OBJECT_FLAGS::CK_PARAMETERIN_DISABLED) {
		// set name with possible nullptr.
		XContainer::NSXString::FromCKSTRING(m_Name, name);
	}

	CKObject::~CKObject() {}

#pragma region Non-virtual Functions

	CK_ID CKObject::GetID(void) const {
		return m_ID;
	}
	CKSTRING CKObject::GetName(void) const {
		return XContainer::NSXString::ToCKSTRING(m_Name);
	}
	void CKObject::SetName(CKSTRING u8_name) {
		XContainer::NSXString::FromCKSTRING(m_Name, u8_name);
	}
	CK_OBJECT_FLAGS CKObject::GetObjectFlags(void) const {
		return m_ObjectFlags;
	}
	void CKObject::SetObjectFlags(CK_OBJECT_FLAGS flags) {
		m_ObjectFlags = flags;
	}
	bool CKObject::IsToBeDeleted() const {
		return EnumsHelper::Has(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_TOBEDELETED);
	}
	CKContext* CKObject::GetCKContext() const {
		return m_Context;
	}

#pragma endregion


	void CKObject::PreDelete() {}

	void CKObject::CheckPreDeletion() {}

	void CKObject::CheckPostDeletion() {}


	void CKObject::PreSave(CKFileVisitor* file, CKDWORD flags) {}

	bool CKObject::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		if (EnumsHelper::Has(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE)) {
			// if hierarchy hidden
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_OBJECT::CK_STATESAVE_OBJECTHIERAHIDDEN);
		} else if (!EnumsHelper::Has(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE)) {
			// if really hidden
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_OBJECT::CK_STATESAVE_OBJECTHIDDEN);
		}

		chunk->SetClassId(CK_CLASSID::CKCID_OBJECT);
		return true;
	}

	bool CKObject::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_OBJECT::CK_STATESAVE_OBJECTHIDDEN)) {
			EnumsHelper::Rm(this->m_ObjectFlags, 
				EnumsHelper::Merge({ CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE }));
		} else {
			if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_OBJECT::CK_STATESAVE_OBJECTHIERAHIDDEN)) {
				// != 0
				EnumsHelper::Rm(this->m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
				EnumsHelper::Add(this->m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE);

			} else {
				// == 0
				EnumsHelper::Add(this->m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
				EnumsHelper::Rm(this->m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE);

			}
		}

		return true;
	}

	void CKObject::PostLoad() {}


	void CKObject::Show(CK_OBJECT_SHOWOPTION show) {
		// clear all visible data of object flags
		EnumsHelper::Rm(m_ObjectFlags, EnumsHelper::Merge({
			CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE,
			CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE
			}));

		switch (show) {
			case CK_OBJECT_SHOWOPTION::CKSHOW:
				EnumsHelper::Add(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
				break;
			case CK_OBJECT_SHOWOPTION::CKHIERARCHICALHIDE:
				EnumsHelper::Add(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_HIERACHICALHIDE);
				break;
			case CK_OBJECT_SHOWOPTION::CKHIDE:
				return;
		}
	}

	bool CKObject::IsVisible() const {
		return EnumsHelper::Has(m_ObjectFlags, CK_OBJECT_FLAGS::CK_OBJECT_VISIBLE);
	}


}