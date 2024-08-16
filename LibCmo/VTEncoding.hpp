#pragma once
#include "VTUtils.hpp"
#include <string>
#include <string_view>

namespace LibCmo::EncodingHelper {

	using EncodingToken = void*;
	constexpr EncodingToken INVALID_ENCODING_TOKEN = nullptr;

	EncodingToken CreateEncodingToken(const std::u8string_view& enc_name);
	void DestroyEncodingToken(EncodingToken token);

	bool ToOrdinary(const std::u8string_view& src, std::string& dst, EncodingToken token);
	bool ToOrdinary(const char8_t* src, std::string& dst, EncodingToken token);
	std::string ToOrdinary(const std::u8string_view& src, EncodingToken token);
	std::string ToOrdinary(const char8_t* src, EncodingToken token);

	bool ToUTF8(const std::string_view& src, std::u8string& dst, EncodingToken token);
	bool ToUTF8(const char* src, std::u8string& dst, EncodingToken token);
	std::u8string ToUTF8(const std::string_view& src, EncodingToken token);
	std::u8string ToUTF8(const char* src, EncodingToken token);

}
