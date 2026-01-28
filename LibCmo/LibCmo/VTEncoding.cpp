#include "VTEncoding.hpp"
#include <yycc/carton/pycodec.hpp>

namespace LibCmo {

#pragma region PrivEncodingPair

	class PrivEncodingPair {
	public:
		PrivEncodingPair(const std::u8string_view &enc_name) : to_utf8(enc_name), to_ordinary(enc_name) {}
		~PrivEncodingPair() {}
		YYCC_DELETE_COPY(PrivEncodingPair)
		YYCC_DEFAULT_MOVE(PrivEncodingPair)

	public:
		bool ToUTF8(const std::string_view &src, std::u8string &dst) {
			auto rv = to_utf8.to_utf8(src);
			if (rv.has_value()) {
				dst = std::move(rv.value());
				return true;
			} else {
				return false;
			}
		}
		bool ToOrdinary(const std::u8string_view &src, std::string &dst) {
			auto rv = to_ordinary.to_char(src);
			if (rv.has_value()) {
				dst = std::move(rv.value());
				return true;
			} else {
				return false;
			}
		}

	private:
		yycc::carton::pycodec::CharToUtf8 to_utf8;
		yycc::carton::pycodec::Utf8ToChar to_ordinary;
	};

#pragma endregion

#pragma region EncodingPair

#pragma endregion

	EncodingPair::EncodingPair(const std::u8string_view &enc_name) : inner(nullptr), is_valid(false), encoding_name(enc_name) {
		// Create private pair
		this->inner = new PrivEncodingPair(this->encoding_name);
		// Use empty string probe to check whether it works
		// because YYCC do not provide these interface.
		std::string ordinary_cache;
		std::u8string utf8_cache;
		if (!this->inner->ToOrdinary(u8"", ordinary_cache)) return;
		if (!this->inner->ToUTF8("", utf8_cache)) return;
		// Okey
		this->is_valid = true;
	}

	EncodingPair::~EncodingPair() {
		if (this->inner != nullptr) delete inner;
	}

	YYCC_IMPL_MOVE_CTOR(EncodingPair, rhs) : inner(rhs.inner), is_valid(rhs.is_valid), encoding_name(rhs.encoding_name) {
		rhs.inner = nullptr;
		rhs.is_valid = false;
	}

	YYCC_IMPL_MOVE_OPER(EncodingPair, rhs) {
		if (this->inner != nullptr) delete inner;

		this->inner = rhs.inner;
		this->is_valid = rhs.is_valid;
		this->encoding_name = rhs.encoding_name;

		rhs.inner = nullptr;
		rhs.is_valid = false;

		return *this;
	}

	bool EncodingPair::ToUTF8(const std::string_view &src, std::u8string &dst) {
		return inner->ToUTF8(src, dst);
	}

	bool EncodingPair::ToOrdinary(const std::u8string_view &src, std::string &dst) {
		return inner->ToOrdinary(src, dst);
	}

	bool EncodingPair::IsValid() const {
		return this->is_valid;
	}

	std::u8string_view EncodingPair::GetEncodingName() const {
		return this->encoding_name;
	}

} // namespace LibCmo
