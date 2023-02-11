#pragma once

#include "VTUtils.hpp"
#include <string>

#if defined(LIBCMO_OS_WIN32)
#include <Windows.h>
#else
#include <iconv.h>
#endif

namespace LibCmo {
	namespace Encoding {

#pragma region assist functions

#if defined(LIBCMO_OS_WIN32)

		bool GetWindowsCodePage(const char* u8_encoding_spec, UINT* result);

		bool WcharToChar(const wchar_t* src, std::string& dest, UINT codepage);
		bool WcharToChar(std::wstring& src, std::string& dest, UINT codepage);

		bool CharToWchar(const char* src, std::wstring& dest, UINT codepage);
		bool CharToWchar(std::string& src, std::wstring& dest, UINT codepage);

#else
#error NO IMPLEMENTATION FOR LINUX ENCODING!
#endif

#pragma endregion

#pragma region core functions

#if defined(LIBCMO_OS_WIN32)

		void GetUtf8VirtoolsName(std::string& native_name, std::string& u8_name, const char* u8_encoding_spec);

#else
#error NO IMPLEMENTATION FOR LINUX ENCODING!
#endif

#pragma endregion

	}
}
