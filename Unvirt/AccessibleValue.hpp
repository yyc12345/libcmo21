#pragma once

#include <VTConstants.hpp>
#include <string>

namespace Unvirt {
	namespace AccessibleValue {



		void GetClassIdName(std::string& strl, LibCmo::CK_CLASSID cls);
		void GetCkErrorName(std::string& strl, LibCmo::CKERROR err);
		void GetClassIdHierarchy(std::string& strl, LibCmo::CK_CLASSID cls);
		void GetCkErrorDescription(std::string& strl, LibCmo::CKERROR err);
		void GetAccessibleFileSize(std::string& strl, uint64_t size);

	}
}
