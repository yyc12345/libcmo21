#include "ColorfulTerminal.hpp"
#include <VTUtils.hpp>

#if defined(LIBCMO_OS_WIN32)
#include <Windows.h>
#include <cstdio>
#include <io.h>
#endif

namespace Unvirt {
	namespace ColorfulTerminal {

		// all of these functions only works on Windows platform
		// due to shitty Windows implementations.

		bool EnsureTerminalColor(void) {
#if defined(LIBCMO_OS_WIN32)
			if (_isatty(_fileno(stdout))) {
				HANDLE h_output;
				DWORD dw_mode;

				h_output = (HANDLE)_get_osfhandle(_fileno(stdout));
				if (!GetConsoleMode(h_output, &dw_mode)) return false;
				if (!SetConsoleMode(h_output, dw_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) return false;

				return true;
			} else return false;
#else
			return true;
#endif
		}

		bool EnsureTerminalEncoding(void) {
#if defined(LIBCMO_OS_WIN32)
			if (!SetConsoleCP(CP_UTF8)) return false;
			if (!SetConsoleOutputCP(CP_UTF8)) return false;
#endif
			return true;
		}

	}
}