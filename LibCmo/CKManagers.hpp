#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

namespace LibCmo::CK2::CKManagerImplements {

	class CKBaseManager {
	public:
		CKBaseManager();
		CKBaseManager(const CKBaseManager&) = delete;
		CKBaseManager& operator=(const CKBaseManager&) = delete;
		virtual ~CKBaseManager();

		virtual CKERROR LoadData(CKStateChunk* statechunk, CKFileData::ShallowDocument* doc);
		virtual CKStateChunk* SaveData(CKFileData::ShallowDocument* doc);
	private:

	};

	class CKAttributeManager {
	public:
		CKAttributeManager();
		CKAttributeManager(const CKAttributeManager&) = delete;
		CKAttributeManager& operator=(const CKAttributeManager&) = delete;
		virtual ~CKAttributeManager();

	private:

	};

}
