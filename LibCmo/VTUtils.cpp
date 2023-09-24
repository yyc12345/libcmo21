#include "VTUtils.hpp"

#if defined(LIBCMO_OS_WIN32)
#include <intrin.h>
// disable annoy macro at the same time
#undef GetObject
#undef GetClassName
#undef LoadImage
#undef GetTempPath
#else
#include <signal.h>
#endif

namespace LibCmo {

	void LibPanic(int line, const char* file, const char* errmsg) {
		fprintf(stderr, "LIBCMO PANIC:%s (%s:L%d)\n",
			errmsg ? errmsg : "", file, line);
		std::abort();
	}

	void LibOrderDebugger() {
#if defined(LIBCMO_BUILD_DEBUG)

#if defined(LIBCMO_OS_WIN32)
		// win32 debug break
		__debugbreak();
#else
		// generic debug break
		raise(SIGTRAP);
#endif

#endif
	}

}
