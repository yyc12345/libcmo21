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

	bool WcharToChar(const wchar_t* src, std::string& dest, UINT codepage);
	bool WcharToChar(std::wstring& src, std::string& dest, UINT codepage);

	bool CharToWchar(const char* src, std::wstring& dest, UINT codepage);
	bool CharToWchar(std::string& src, std::wstring& dest, UINT codepage);

	bool CharToChar(const char* src, std::string& dest, UINT src_codepage, UINT dest_codepage);
	bool CharToChar(std::string& src, std::string& dest, UINT src_codepage, UINT dest_codepage);

#else

	bool DoIconv(const char* enc_from, const char* enc_to, std::string& str_from, std::string& str_to);

#endif

#pragma endregion

#pragma region core functions

#if defined(LIBCMO_OS_WIN32)

	// Token is the ticket for using encoding functions.
	// It should be created by "GenerateEncodingToken" and free by "DestroyEncodingToken".
	using ENCODING_TOKEN = UINT*;
	constexpr const ENCODING_TOKEN ENCODING_TOKEN_DEFAULT = nullptr;

#else

	using ENCODING_TOKEN = char*;
	constexpr const ENCODING_TOKEN ENCODING_TOKEN_DEFAULT = nullptr;

#endif

	ENCODING_TOKEN CreateEncodingToken(std::string& token_string);
	void DestroyEncodingToken(ENCODING_TOKEN token);

	void GetUtf8VirtoolsName(std::string& native_name, std::string& u8_name, ENCODING_TOKEN token);
	void GetNativeVirtoolsName(std::string& u8_name, std::string& native_name, ENCODING_TOKEN token);

	void SetStdPathFromU8Path(std::filesystem::path& stdpath, const char* u8_path);
	FILE* OpenStdPathFile(std::filesystem::path& u8_filepath, bool is_read);

#pragma endregion

}
