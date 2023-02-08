#pragma once

#include <string>
#include <cstdarg>

namespace Unvirt {
	namespace StringHelper {

		void StdstringPrintf(std::string& strl, const char* format, ...);
		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr);

	}
}
