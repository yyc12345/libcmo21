#include "../CKManagers.hpp"

namespace LibCmo::CK2::CKManagerImplements {

	CKBaseManager::CKBaseManager(CKMinContext* ctx, CK_ID ckid) {

	}
	CKBaseManager::~CKBaseManager() {

	}

	CKERROR CKBaseManager::LoadData(CKStateChunk* statechunk, CKFileDocument* doc) {
		return CKERROR::CKERR_OK;
	}
	CKStateChunk* CKBaseManager::SaveData(CKFileDocument* doc) {
		return nullptr;
	}


	CKAttributeManager::CKAttributeManager(CKMinContext* ctx, CK_ID ckid) : CKBaseManager(ctx, ckid) {

	}
	CKAttributeManager::~CKAttributeManager() {

	}

}
