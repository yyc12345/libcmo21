#include "StringHelper.hpp"
#include <stdexcept>

namespace Unvirt {
	namespace StringHelper {

		void StdstringPrintf(std::string& strl, const char* format, ...) {
			va_list argptr;
			va_start(argptr, format);
			StdstringVPrintf(strl, format, argptr);
			va_end(argptr);
		}

		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr) {
			int count = _vsnprintf(NULL, 0, format, argptr);

			strl.resize(count);
			int write_result = _vsnprintf(strl.data(), count, format, argptr);

			if (write_result < 0 || write_result > count) throw new std::length_error("Invalid write_result in _vsnprintf.");
		}

	}
}
