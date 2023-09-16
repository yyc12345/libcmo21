#include "VTUtils.hpp"

namespace LibCmo {

	void LibPanic(int line, const char* file, const char* errmsg) {
		fprintf(stderr, "LIBCMO PANIC:%s (%s:L%d)\n",
			errmsg ? errmsg : "", file, line);
		std::abort();
	}

}
