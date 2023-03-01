#include "VTEncoding.hpp"

namespace LibCmo::EncodingHelper {

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

	static constexpr const IconvInc = 16;
	bool DoIconv(const char* enc_from, const char* enc_to, 
			std::string& str_from, std::string& str_to) {
		iconv_t cd;
		char *inbuf = nullptr, *outbuf = nullptr;
		size_t inbytesleft, outbytesleft, nchars, result_len;

		// check empty
		if (str_from.empty()) {
			str_to.clear();
			return true;
		}

		// create iconv descriptor
		cd = iconv_open(enc_to, enc_from);
		if (cd == (iconv_t) -1) {
			// fail to create iconv descriptor
			return false;
		}

		// pre-resize
		str_to.resize(str_from.size() + IconvInc);
		// setup some variables
		inbytesleft = str_from.size();
		inbuf = str_from.data();
		
		outbytesleft = str_to_size();
		outbuf = str_to.data();
		
		result_len = str_to.size();

		// conv core
		nchars = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
		while (nchars == (size_t)-1 && errno == E2BIG) {
			// record the length has been converted
			size_t len = outbuf - str_to.data();

			// resize for variables
			result_len += IconvInc;
			outbytesleft += IconvInc;
			
			// resize for container
			str_to.resize(result_len);
			
		        // assign new outbuf from failed position	
			outbuf = str_to.data() + len;
			nchars = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
		}

		// close iconv descriptor
		iconv_close(cd);

		// check error
		if (nchars == (size_t)-1) {
			// failed
			return false;
		} else {
			// success
			// resize result to get correct data
			str_to.resize(result_len - outbytesleft);
			return true;
		}

	}

#endif

#pragma endregion

#pragma region core functions

#if defined(LIBCMO_OS_WIN32)

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

	static const char UTF8_SYMBOL[] = "UTF-8";

	ENCODING_TOKEN CreateEncodingToken(std::string& token_string) {
		ENCODING_TOKEN token = new(std::nothrow) char[token_string.size() + 1];
		if (token == nullptr) return ENCODING_TOKEN_DFAULT;

		std::memcpy(token, token_string.c_str(), token_string.size() + 1);
		return token;
	}

	void DestroyEncodingToken(ENCODING_TOKEN token) {
		if (token != ENCODING_TOKEN_DEFAULT) {
			delete[] token;
		}
	}
	
	void GetUtf8VirtoolsName(std::string& native_name, std::string& u8_name, ENCODING_TOKEN token) {
		if (token == ENCODING_TOKEN_DEFAULT) {
			u8_name = native_name.c_str();
			return;
		}

		// convert with fallback
		if (!DoIconv(token, UTF8_SYMBOL, native_name, u8_name)) {
			u8_name = native_name.c_str();
		}
	}

	void GetNativeVirtoolsName(std::string& u8_name, std::string& native_name, ENCODING_TOKEN token) {

		if (token == ENCODING_TOKEN_DEFAULT) {
			native_name = u8_name.c_str();
			return;
		}

		// convert with fallback
		if (!DoIconv(UTF8_SYMBOL, token, u8_name, native_name)) {
			native_name = u8_name.c_str();
		}
	}

	void SetStdPathFromU8Path(std::filesystem::path& stdpath, const char* u8_path) {
		stdpath = u8_path;
	}

	FILE* OpenStdPathFile(std::filesystem::path& u8_filepath, bool is_read) {
		return fopen(u8_filepath.string().c_str(), is_read ? "rb" : "wb");
	}


#endif

#pragma endregion

}

