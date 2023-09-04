#pragma once

#include "../../VTAll.hpp"
#include "CKBaseManager.hpp"

namespace LibCmo::CK2::MgrImpls {

	class CKPathManager : public CKBaseManager {
	public:
		CKPathManager(CKContext* ctx) :
			CKBaseManager(ctx, PATH_MANAGER_GUID, "Path Manager") {}
		virtual ~CKPathManager() {}
		LIBCMO_DISABLE_COPY_MOVE(CKPathManager);

	protected:

	};

}
