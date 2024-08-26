#pragma once
#include "VTUtils.hpp"
#include <string>
#include <string_view>

/**
 * @brief The namespace providing platform independent encoding convertion solution.
 * @details
 * LibCmo is a cross-platform library.
 * In different platform, they have their own encoding solution.
 * So we create a universal encoding solution in this namespace like Python does.
 * User only need input our universal language name,
 * then they can convert same strings in different platforms.
*/
namespace LibCmo::EncodingHelper {

	/**
	 * @brief The token for encoding convertion.
	 * @details
	 * If you want to use any encoding function provided by this namespace,
	 * you should fetch a proper token for it.
	 * This token records the encoding which you want to convert from / to.
	 * 
	 * In underlying implementation, this token type is a pointer
	 * pointing to an allocated memory space recording you specified encoding infomations.
	 * So this token should only be fetched from CreateEncodingToken()
	 * and must be freed by DestroyEncodingToken() if you don't need it.
	 * Otherwise it will cause memory leak.
	*/
	using EncodingToken = void*;
	/**
	 * @brief The invalid value of #EncodingToken.
	 * @details
	 * It represents an invalid encoding token.
	 * Any encoding token which equals to this value should not be used.
	 * In underlying implementation, it actually is \c nullptr.
	 * Because EncodingToken is just a raw pointer.
	*/
	constexpr EncodingToken INVALID_ENCODING_TOKEN = nullptr;

	/**
	 * @brief Create encoding token by given universal encoding name.
	 * @param[in] enc_name Universal encoding name.
	 * @return 
	 * The generated encoding token.
	 * #INVALID_ENCODING_TOKEN if fail to generate encoding token.
	 * Usually it means that your given encoding name is invalid.
	 * Please note any successfully generated encoding token must be freed by DestroyEncodingToken().
	*/
	EncodingToken CreateEncodingToken(const std::u8string_view& enc_name);
	/**
	 * @brief Destroy given encoding token and free its associated resources.
	 * @param[in] token 
	 * The encoding token to be destroyed.
	 * If token is #INVALID_ENCODING_TOKEN, this function does nothing.
	*/
	void DestroyEncodingToken(EncodingToken token);
	/**
	 * @brief Get associated universal encoding name of given encoding token.
	 * @param[in] token The encoding token for getting name.
	 * @return Encoding token associated name (the universal encoding name used to create this token).
	 * Blank if given token is invalid or fail to get.
	*/
	std::u8string GetEncodingTokenAssociatedName(EncodingToken token);
	/**
	 * @brief Check whether given universal encoding name can be used to produce token.
	 * @param[in] enc_name Universal encoding name.
	 * @return True if it is, otherwise false.
	 * @remarks
	 * Please note this function only check whether given encoding name is acceptable by token creator.
	 * Hoewver it doesn't mean that the token must be created if this function return true.
	 * Because there are some runtime issues which can cause that fail to create encoding token.
	*/
	bool IsValidEncodingName(const std::u8string_view& enc_name);

	/**
	 * @brief Convert native string to UTF8 string by given encoding.
	 * @param[in] src The native string view to be converted.
	 * @param[out] dst The variable holding converted UTF8 string result.
	 * @param[in] token 
	 * The encoding used when converting. 
	 * #INVALID_ENCODING_TOKEN causes function returns false.
	 * @return True if success, otherwise false.
	*/
	bool ToOrdinary(const std::u8string_view& src, std::string& dst, EncodingToken token);
	/**
	 * @brief Convert native string to UTF8 string by given encoding.
	 * @param[in] src 
	 * The pointer to native string to be converted.
	 * nullptr is allowed but it will result in that function returns false.
	 * @param[out] dst The variable holding converted UTF8 string result.
	 * @param[in] token 
	 * The encoding used when converting. 
	 * #INVALID_ENCODING_TOKEN causes function returns false.
	 * @return True if success, otherwise false.
	*/
	bool ToOrdinary(const char8_t* src, std::string& dst, EncodingToken token);
	/**
	 * @brief Convert native string to UTF8 string by given encoding.
	 * @param[in] src The native string view to be converted.
	 * @param[in] token 
	 * The encoding used when converting. 
	 * #INVALID_ENCODING_TOKEN causes function returns false.
	 * @return 
	 * Converted UTF8 string result. Empty string if function failed.
	 * Please note empty input will also produce empty result.
	 * For preventing this ambiguity, please use other overloads.
	*/
	std::string ToOrdinary(const std::u8string_view& src, EncodingToken token);
	/**
	 * @brief Convert native string to UTF8 string by given encoding.
	 * @param[in] src 
	 * The pointer to native string to be converted.
	 * nullptr is allowed but it will result in that function returns false.
	 * @param[in] token 
	 * The encoding used when converting. 
	 * #INVALID_ENCODING_TOKEN causes function returns false.
	 * @return 
	 * Converted UTF8 string result. Empty string if function failed.
	 * Please note empty input will also produce empty result.
	 * For preventing this ambiguity, please use other overloads.
	*/
	std::string ToOrdinary(const char8_t* src, EncodingToken token);
	
	/**
	 * @brief Convert UTF8 string to native string by given encoding.
	 * @param[in] src The UTF8 string view to be converted.
	 * @param[out] dst The variable holding converted native string result.
	 * @param[in] token 
	 * The encoding used when converting. 
	 * #INVALID_ENCODING_TOKEN causes function returns false.
	 * @return True if success, otherwise false.
	*/
	bool ToUTF8(const std::string_view& src, std::u8string& dst, EncodingToken token);
	/**
	 * @brief Convert UTF8 string to native string by given encoding.
	 * @param[in] src 
	 * The pointer to UTF8 string to be converted.
	 * nullptr is allowed but it will result in that function returns false.
	 * @param[out] dst The variable holding converted native string result.
	 * @param[in] token 
	 * The encoding used when converting. 
	 * #INVALID_ENCODING_TOKEN causes function returns false.
	 * @return True if success, otherwise false.
	*/
	bool ToUTF8(const char* src, std::u8string& dst, EncodingToken token);
	/**
	 * @brief Convert UTF8 string to native string by given encoding.
	 * @param[in] src The UTF8 string view to be converted.
	 * @param[in] token 
	 * The encoding used when converting. 
	 * #INVALID_ENCODING_TOKEN causes function returns false.
	 * @return 
	 * Converted native string result. Empty string if function failed.
	 * Please note empty input will also produce empty result.
	 * For preventing this ambiguity, please use other overloads.
	*/
	std::u8string ToUTF8(const std::string_view& src, EncodingToken token);
	/**
	 * @brief Convert UTF8 string to native string by given encoding.
	 * @param[in] src 
	 * The pointer to UTF8 string to be converted.
	 * nullptr is allowed but it will result in that function returns false.
	 * @param[in] token 
	 * The encoding used when converting. 
	 * #INVALID_ENCODING_TOKEN causes function returns false.
	 * @return 
	 * Converted native string result. Empty string if function failed.
	 * Please note empty input will also produce empty result.
	 * For preventing this ambiguity, please use other overloads.
	*/
	std::u8string ToUTF8(const char* src, EncodingToken token);

}
