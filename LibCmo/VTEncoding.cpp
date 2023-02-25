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

		bool CharToChar(const char* src, std::string& dest, UINT src_codepage, UINT dest_codepage) {
			std::wstring intermediary;
			if (!CharToWchar(src, intermediary, src_codepage)) return false;
			if (!WcharToChar(intermediary, dest, dest_codepage)) return false;
			return true;
		}
		bool CharToChar(std::string& src, std::string& dest, UINT src_codepage, UINT dest_codepage) {
			return CharToChar(src.c_str(), dest, src_codepage, dest_codepage);
		}

#else

		//todo: linux implementation

#endif

#pragma endregion

#pragma region core functions

#if defined(LIBCMO_OS_WIN32)

		const ENCODING_TOKEN ENCODING_TOKEN_DEFAULT = nullptr;

		ENCODING_TOKEN CreateEncodingToken(std::string& token_string) {
			ENCODING_TOKEN token = new(std::nothrow) UINT();
			if (token == nullptr) return ENCODING_TOKEN_DEFAULT;

			if (!GetWindowsCodePage(token_string.c_str(), token)) {
				*token = CP_ACP;
			}
			return token;
		}
		void DestroyEncodingToken(ENCODING_TOKEN token) {
			if (token != ENCODING_TOKEN_DEFAULT) {
				delete token;
			}
		}

		void GetUtf8VirtoolsName(std::string& native_name, std::string& u8_name, ENCODING_TOKEN token) {
			if (token == ENCODING_TOKEN_DEFAULT) {
				u8_name = native_name.c_str();
				return;
			}

			// convert with fallback
			if (!CharToChar(native_name, u8_name, *token, CP_UTF8)) {
				u8_name = native_name.c_str();
			}
		}

		void GetNativeVirtoolsName(std::string& u8_name, std::string& native_name, ENCODING_TOKEN token) {
			if (token == ENCODING_TOKEN_DEFAULT) {
				native_name = u8_name.c_str();
				return;
			}

			// convert with fallback
			if (!CharToChar(u8_name, native_name, CP_UTF8, *token)) {
				native_name = u8_name.c_str();
			}
		}

		void SetStdPathFromU8Path(std::filesystem::path& stdpath, const char* u8_path) {
			std::wstring intermediary;
			if (CharToWchar(u8_path, intermediary, CP_UTF8)) {
				stdpath = intermediary.c_str();
			} else {
				// fallback
				stdpath = u8_path;
			}
		}

		FILE* OpenStdPathFile(std::filesystem::path& u8_filepath, bool is_read) {
			return _wfopen(u8_filepath.wstring().c_str(), is_read ? L"rb" : L"wb");
		}

#else

		const ENCODING_TOKEN ENCODING_TOKEN_DEFAULT = nullptr;

		//todo: linux implementation

#endif

#pragma endregion

	}
}

