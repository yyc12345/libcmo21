#include "TerminalHelper.hpp"
#include <VTUtils.hpp>
#include <VTEncoding.hpp>
#include <cstdarg>
#include <iostream>
#include <cstdio>

#if YYCC_OS == YYCC_OS_WINDOWS
#include <Windows.h>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#endif

namespace Unvirt {
	namespace TerminalHelper {

		// all of these functions only works on Windows platform
		// due to shitty Windows implementations.

		bool EnsureTerminalColor(void) {
#if YYCC_OS == YYCC_OS_WINDOWS
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
#if YYCC_OS == YYCC_OS_WINDOWS
			if (!SetConsoleCP(CP_UTF8)) return false;
			if (!SetConsoleOutputCP(CP_UTF8)) return false;
			
			/*_setmode(_fileno(stdout), _O_U8TEXT);*/
			_setmode(_fileno(stdin), _O_U16TEXT);
#endif
			return true;
		}

		void GetCmdLine(std::string& u8cmd) {
			fputs(UNVIRT_TERMCOL_LIGHT_GREEN(("Unvirt> ")), stdout);
#if YYCC_OS == YYCC_OS_WINDOWS
			std::wstring wcmd;
			std::getline(std::wcin, wcmd);
			LibCmo::EncodingHelper::WcharToChar(wcmd, u8cmd, CP_UTF8);
#else
			std::getline(std::cin, u8cmd);
#endif
		}

	}
}