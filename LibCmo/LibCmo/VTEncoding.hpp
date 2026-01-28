#pragma once
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <string_view>

namespace LibCmo {

	class PrivEncodingPair;

	class EncodingPair {
	public:
		EncodingPair(const std::u8string_view& enc_name);
		~EncodingPair();
		YYCC_DELETE_COPY(EncodingPair)
		YYCC_DECL_MOVE(EncodingPair)

	public:
		bool ToUTF8(const std::string_view& src, std::u8string& dst);
		bool ToOrdinary(const std::u8string_view& src, std::string& dst);
		bool IsValid() const;
		std::u8string_view GetEncodingName() const;

	private:
		PrivEncodingPair* inner;
		bool is_valid;
		std::u8string encoding_name;
	};

}
