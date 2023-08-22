#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

namespace LibCmo::CK2::CKManagerImplements {

	class CKBaseManager {
	public:
		CKBaseManager(CKContext* ctx, CK_ID ckid);
		CKBaseManager(const CKBaseManager&) = delete;
		CKBaseManager& operator=(const CKBaseManager&) = delete;
		virtual ~CKBaseManager();

		virtual CKERROR LoadData(CKStateChunk* statechunk, CKFileDocument* doc);
		virtual CKStateChunk* SaveData(CKFileDocument* doc);

	private:

	};

	class CKAttributeManager : public CKBaseManager {
	public:
		CKAttributeManager(CKContext* ctx, CK_ID ckid);
		CKAttributeManager(const CKAttributeManager&) = delete;
		CKAttributeManager& operator=(const CKAttributeManager&) = delete;
		virtual ~CKAttributeManager();

	private:

	};

}
