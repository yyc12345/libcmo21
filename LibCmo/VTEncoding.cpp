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

	bool WcharToChar(const wchar_t* src, std::string& dest, const UINT codepage) {
		int count, write_result;

		//converter to CHAR
		count = WideCharToMultiByte(codepage, 0, src, -1, NULL, 0, NULL, NULL);
		if (count <= 0) return false;

		dest.resize(count);
		write_result = WideCharToMultiByte(codepage, 0, src, -1, dest.data(), count, NULL, NULL);
		if (write_result <= 0) return false;

		return true;
	}
	bool WcharToChar(const std::wstring& src, std::string& dest, const UINT codepage) {
		return WcharToChar(src.c_str(), dest, codepage);
	}

	bool CharToWchar(const char* src, std::wstring& dest, const UINT codepage) {
		int wcount, write_result;

		// convert to WCHAR
		wcount = MultiByteToWideChar(codepage, 0, src, -1, NULL, 0);
		if (wcount <= 0) return false;

		dest.resize(wcount);
		write_result = MultiByteToWideChar(codepage, 0, src, -1, dest.data(), wcount);
		if (write_result <= 0) return false;

		return true;
	}
	bool CharToWchar(const std::string& src, std::wstring& dest, const UINT codepage) {
		return CharToWchar(src.c_str(), dest, codepage);
	}

	bool CharToChar(const char* src, std::string& dest, const UINT src_codepage, const UINT dest_codepage) {
		std::wstring intermediary;
		if (!CharToWchar(src, intermediary, src_codepage)) return false;
		if (!WcharToChar(intermediary, dest, dest_codepage)) return false;
		return true;
	}
	bool CharToChar(const std::string& src, std::string& dest, const UINT src_codepage, const UINT dest_codepage) {
		return CharToChar(src.c_str(), dest, src_codepage, dest_codepage);
	}

#else

	static constexpr const size_t IconvInc = 16;
	static const iconv_t InvalidIconvDescriptor = reinterpret_cast<iconv_t>(-1);

	bool CreateIconvDescriptor(const char* enc_from, const char* enc_to, iconv_t& val) {
		val = iconv_open(enc_to, enc_from);
		return val != InvalidIconvDescriptor;
	}
	void DestroyIconvDescriptor(iconv_t& val) {
		if (val == InvalidIconvDescriptor) return;

		iconv_close(val);
		val = InvalidIconvDescriptor;
	}

	// Reference: https://stackoverflow.com/questions/13297458/simple-utf8-utf16-string-conversion-with-iconv
	bool DoIconv(iconv_t& cd, const std::string& str_from, std::string& str_to) {
		char *inbuf = nullptr, *outbuf = nullptr;
		size_t inbytesleft, outbytesleft, nchars, result_len;

		// check empty
		if (str_from.empty()) {
			str_to.clear();
			return true;
		}

		// check iconv descriptor
		if (cd == InvalidIconvDescriptor) {
			// invalid iconv descriptor
			return false;
		}

		// pre-resize
		str_to.resize(str_from.size() + IconvInc);
		// setup some variables
		inbytesleft = str_from.size();
		inbuf = const_cast<char*>(str_from.c_str());
		
		outbytesleft = str_to.size();
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

		// restore descriptor initial state
		iconv(cd, nullptr, nullptr, nullptr, nullptr);

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

	ENCODING_TOKEN CreateEncodingToken(const std::string& token_string) {
		ENCODING_TOKEN token = new UINT();
		if (!GetWindowsCodePage(token_string.c_str(), token)) {
			*token = CP_ACP;
		}
		return token;
	}
	void DestroyEncodingToken(const ENCODING_TOKEN& token) {
		if (token != ENCODING_TOKEN_DEFAULT) {
			delete token;
		}
	}

	bool GetUtf8VirtoolsName(const std::string& native_name, std::string& u8_name, const ENCODING_TOKEN& token) {
		if (token == ENCODING_TOKEN_DEFAULT) return false;
		return CharToChar(native_name, u8_name, *token, CP_UTF8);
	}

	bool GetNativeVirtoolsName(const std::string& u8_name, std::string& native_name, const ENCODING_TOKEN& token) {
		if (token == ENCODING_TOKEN_DEFAULT) return false;
		return CharToChar(u8_name, native_name, CP_UTF8, *token);
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

	IconvPair::IconvPair() :
		FromUtf8(InvalidIconvDescriptor), ToUtf8(InvalidIconvDescriptor) {
	}

	IconvPair::~IconvPair() {
		DestroyIconvDescriptor(this->FromUtf8);
		DestroyIconvDescriptor(this->ToUtf8);
	}


	static constexpr const char UTF8_SYMBOL[] = "UTF-8";

	ENCODING_TOKEN CreateEncodingToken(const std::string& token_string) {
		ENCODING_TOKEN token = new IconvPair();
		if (!CreateIconvDescriptor(UTF8_SYMBOL, token_string.c_str(), token->FromUtf8) ||
			!CreateIconvDescriptor(token_string.c_str(), UTF8_SYMBOL, token->ToUtf8)) {
			delete token;
			return ENCODING_TOKEN_DEFAULT;
		}

		return token;
	}

	void DestroyEncodingToken(const ENCODING_TOKEN& token) {
		if (token != ENCODING_TOKEN_DEFAULT) {
			delete token;
		}
	}
	
	bool GetUtf8VirtoolsName(const std::string& native_name, std::string& u8_name, const ENCODING_TOKEN& token) {
		if (token == ENCODING_TOKEN_DEFAULT) return false;
		return DoIconv(token->ToUtf8, native_name, u8_name);
	}

	bool GetNativeVirtoolsName(const std::string& u8_name, std::string& native_name, const ENCODING_TOKEN& token) {
		if (token == ENCODING_TOKEN_DEFAULT) return false;
		return DoIconv(token->FromUtf8, u8_name, native_name);
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

