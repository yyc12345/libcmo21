#include "VTEncoding.hpp"

namespace LibCmo {
	namespace Encoding {

#pragma region assist functions

#if defined(LIBCMO_OS_WIN32)

#define LIBCMO_STR_EQUAL(a, b) strcmp(reinterpret_cast<const char*>(a), reinterpret_cast<const char*>(b)) == 0
		bool GetWindowsCodePage(const char* u8_encoding_spec, UINT* result) {
			if (LIBCMO_STR_EQUAL(u8_encoding_spec, u8"CP_ACP")) *result = CP_ACP;
			else if (LIBCMO_STR_EQUAL(u8_encoding_spec, u8"CP_MACCP")) *result = CP_MACCP;
			else if (LIBCMO_STR_EQUAL(u8_encoding_spec, u8"CP_OEMCP")) *result = CP_OEMCP;
			else if (LIBCMO_STR_EQUAL(u8_encoding_spec, u8"CP_THREAD_ACPP")) *result = CP_THREAD_ACP;
			else if (LIBCMO_STR_EQUAL(u8_encoding_spec, u8"CP_UTF8")) *result = CP_UTF8;
			else {
				char* pend = nullptr;
				errno = 0;
				uint64_t v = std::strtoull(u8_encoding_spec, &pend, 10);

				if (pend == u8_encoding_spec || errno == ERANGE) return false;
				*result = static_cast<UINT>(v);
			}
			return true;
		}
#undef LIBCMO_STR_EQUAL

		bool WcharToChar(const wchar_t* src, std::string& dest, UINT codepage) {
			int count, write_result;

			//converter to CHAR
			count = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
			if (count <= 0) return false;

			dest.resize(count);
			write_result = WideCharToMultiByte(CP_UTF8, 0, src, -1, dest.data(), count, NULL, NULL);
			if (write_result <= 0) return false;

			return true;
		}
		bool WcharToChar(std::wstring& src, std::string& dest, UINT codepage) {
			return WcharToChar(src.c_str(), dest, codepage);
		}

		bool CharToWchar(const char* src, std::wstring& dest, UINT codepage) {
			int wcount, write_result;

			// convert to WCHAR
			wcount = MultiByteToWideChar(codepage, 0, src, -1, NULL, 0);
			if (wcount <= 0) return false;

			dest.resize(wcount);
			write_result = MultiByteToWideChar(CP_UTF8, 0, src, -1, dest.data(), wcount);
			if (write_result <= 0) return false;

			return true;
		}
		bool CharToWchar(std::string& src, std::wstring& dest, UINT codepage) {
			return CharToWchar(src.c_str(), dest, codepage);
		}

#else

		//todo: linux implementation

#endif

#pragma endregion

#pragma region core functions

#if defined(LIBCMO_OS_WIN32)

		void GetUtf8VirtoolsName(std::string& native_name, std::string& u8_name, const char* u8_encoding_spec) {
			// switch encoding spec
			UINT codepage = CP_ACP;
			if (!GetWindowsCodePage(u8_encoding_spec, &codepage)) {
				u8_name = native_name.c_str();
				return;
			}

			// do convert
			std::wstring intermediary;
			if (!CharToWchar(native_name, intermediary, codepage)) {
				u8_name = native_name.c_str();
				return;
			}
			if (!WcharToChar(intermediary, u8_name, CP_UTF8)) {
				u8_name = native_name.c_str();
				return;
			}

			return;
		}

		void GetWcharStdin(std::string& u8_input, std::wstring& wc_input) {
			// just redirect to prev func
			CharToWchar(u8_input, wc_input, CP_UTF8);
		}

#else
				//todo: linux implementation

#endif

#pragma endregion

	}
}

