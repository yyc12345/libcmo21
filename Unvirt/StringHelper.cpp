#include "StringHelper.hpp"
#include <stdexcept>

namespace Unvirt::StringHelper {

	void StdstringPrintf(std::string& strl, const char* format, ...) {
		va_list argptr;
		va_start(argptr, format);
		StdstringVPrintf(strl, format, argptr);
		va_end(argptr);
	}

	void StdstringVPrintf(std::string& strl, const char* format, va_list argptr) {
		va_list args1;
		va_copy(args1, argptr);
		va_list args2;
		va_copy(args2, argptr);

		int count = std::vsnprintf(nullptr, 0, format, args1);
		if (count < 0) throw new std::length_error("Invalid length returned by vsnprintf.");
		va_end(args1);

		strl.resize(count);
		int write_result = std::vsnprintf(strl.data(), strl.size() + 1, format, args2);
		va_end(args2);
		
		if (write_result < 0 || write_result > count) throw new std::length_error("Invalid write result in vsnprintf.");
	}

}
