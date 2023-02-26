#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

namespace LibCmo {

	// forward decl to rm recursive reference
	class CKMinContext;
	class CKStateChunk;

	namespace MgrsImpl {

		class CKBaseManager {
		public:
			CKBaseManager();
			CKBaseManager(const CKBaseManager&) = delete;
			CKBaseManager& operator=(const CKBaseManager&) = delete;
			virtual ~CKBaseManager();

		private:

		};

	}
}
