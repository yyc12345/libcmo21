#include "../CKManagers.hpp"

namespace LibCmo::CK2::CKManagerImplements {

	CKBaseManager::CKBaseManager(CKMinContext* ctx, CK_ID ckid) {

	}
	CKBaseManager::~CKBaseManager() {

	}


	CKAttributeManager::CKAttributeManager(CKMinContext* ctx, CK_ID ckid) : CKBaseManager(ctx, ckid) {

	}
	CKAttributeManager::~CKAttributeManager() {

	}

}
