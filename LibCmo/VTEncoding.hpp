#pragma once

#include "VTUtils.hpp"
#include <string>
#include <filesystem>

#if defined(LIBCMO_OS_WIN32)
#include <Windows.h>
#include <fileapi.h>
#else
#include <iconv.h>
#endif

namespace LibCmo::EncodingHelper {

#pragma region assist functions

#if defined(LIBCMO_OS_WIN32)

	bool GetWindowsCodePage(const char* u8_encoding_spec, UINT* result);

	bool WcharToChar(const wchar_t* src, std::string& dest, const UINT codepage);
	bool WcharToChar(const std::wstring& src, std::string& dest, const UINT codepage);

	bool CharToWchar(const char* src, std::wstring& dest, const UINT codepage);
	bool CharToWchar(const std::string& src, std::wstring& dest, const UINT codepage);

	bool CharToChar(const char* src, std::string& dest, const UINT src_codepage, const UINT dest_codepage);
	bool CharToChar(const std::string& src, std::string& dest, const UINT src_codepage, const UINT dest_codepage);

#else

	bool CreateIconvDescriptor(const char* enc_from, const char* enc_to, iconv_t& val);
	void DestroyIconvDescriptor(iconv_t& val);

	bool DoIconv(iconv_t& cd, const std::string& str_from, std::string& str_to);

#endif

#pragma endregion

#pragma region core functions

#if defined(LIBCMO_OS_WIN32)

	// Token is the ticket for using encoding functions.
	// It should be created by "GenerateEncodingToken" and free by "DestroyEncodingToken".
	using ENCODING_TOKEN = UINT*;
	constexpr const ENCODING_TOKEN ENCODING_TOKEN_DEFAULT = nullptr;

#else

	class IconvPair {
	public:
		IconvPair();
		IconvPair(const IconvPair&) = delete;
		IconvPair& operator=(const IconvPair&) = delete;
		~IconvPair();
		
		iconv_t FromUtf8;
		iconv_t ToUtf8;
	};
	using ENCODING_TOKEN = IconvPair*;
	constexpr const ENCODING_TOKEN ENCODING_TOKEN_DEFAULT = nullptr;

#endif

	ENCODING_TOKEN CreateEncodingToken(const std::string& token_string);
	void DestroyEncodingToken(const ENCODING_TOKEN& token);

	bool GetUtf8VirtoolsName(const std::string& native_name, std::string& u8_name, const ENCODING_TOKEN& token);
	bool GetNativeVirtoolsName(const std::string& u8_name, std::string& native_name, const ENCODING_TOKEN& token);

	void SetStdPathFromU8Path(std::filesystem::path& stdpath, const char* u8_path);
	FILE* StdPathFOpen(std::filesystem::path& std_filepath, const char* u8_mode);

#pragma endregion

}
