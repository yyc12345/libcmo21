#pragma once
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/string/op.hpp>
#include <vector>
#include <type_traits>

#define BMAPINSP_L LibCmo
#define BMAPINSP_C LibCmo::CK2
#define BMAPINSP_O LibCmo::CK2::ObjImpls

namespace BMapInspector::Ruleset::Shared::Utility {

#pragma region Utilities

	/**
	 * @brief Check whether given 2 float point values are equal with given tolerance.
	 * @param[in] lhs The left value to compare.
	 * @param[in] rhs The right value to compare.
	 * @param[in] tolerance The tolerance to compare.
	 * @return True if they are equal with given tolerance, otherwise false.
	 */
	bool FPEqual(BMAPINSP_L::CKFLOAT lhs, BMAPINSP_L::CKFLOAT rhs, BMAPINSP_L::CKFLOAT tolerance);

#pragma endregion

#pragma region Virtools Stuff

	/**
	 * @brief 
	 * @param[in] ctx Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return Found pointer to CKGroup, otherwise nullptr.
	 */
	BMAPINSP_O::CKGroup* FetchGroup(BMAPINSP_C::CKContext* ctx, BMAPINSP_L::CKSTRING name);
	/**
	 * @brief 
	 * @param[in] ctx Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return Found pointer to CKMaterial, otherwise nullptr.
	 */
	BMAPINSP_O::CKMaterial* FetchMaterial(BMAPINSP_C::CKContext* ctx, BMAPINSP_L::CKSTRING name);
	std::vector<BMAPINSP_O::CK3dObject*> FetchPhysicalized3dObjects(BMAPINSP_C::CKContext* ctx);

	/**
	 * @brief Extract the file name part of the texture slot associated file path in given CKTexture.
	 * @param[in] tex The texture for extracting. Can not be nullptr.
	 * @return Extracted file name part or nothing (there is no associated file path).
	 */
	std::optional<std::u8string> ExtractTextureFileName(BMAPINSP_O::CKTexture* tex);
	/**
	 * @brief Check whether given CKTexture has the given file name (case-insensitive).
	 * @param[in] tex Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return True if it is, otherwise false.
	 */
	bool CheckTextureFileName(BMAPINSP_O::CKTexture* tex, BMAPINSP_L::CKSTRING name);

	/**
	 * @brief 
	 * @param[in] group Can not be nullptr.
	 * @return All objects is the child class of CK3dEntity.
	 */
	std::vector<BMAPINSP_O::CK3dObject*> Iter3dObjects(BMAPINSP_O::CKGroup* group);
	/**
	 * @brief 
	 * @param[in] mesh Can not be nullptr.
	 * @return All nullptr reference are removed.
	 */
	std::vector<BMAPINSP_O::CKMaterial*> IterMaterial(BMAPINSP_O::CKMesh* mesh);

#pragma endregion

#pragma region Presentation

	/**
	 * @brief Quote given string.
	 * @param[in] words The words to quote.
	 * @return The quoted words.
	 */
	std::u8string QuoteText(const std::u8string_view& words);
	/**
	 * @brief Quote given CKObject's name.
	 * @param[in] obj The CKObject for quoting. Can not be nullptr.
	 * @remarks If there is no name for given CKObject, a colorful name will be quoted and return.
	 * @return The quoted name.
	 */
	std::u8string QuoteObjectName(BMAPINSP_O::CKObject* obj);
	/**
	 * @brief Quote given range of CKObject's names.
	 * @tparam InputIt The type of iterator which iterate non-nullptr pointer to CKObject.
	 * @param[in] first The iterator to the first element.
	 * @param[in] last The iterator to the last element.
	 * @return The quoted names with quote as separator.
	 */
	template<std::input_iterator InputIt>
	    requires std::is_pointer_v<std::iter_value_t<InputIt>>
	             && std::is_base_of_v<BMAPINSP_O::CKObject, std::remove_pointer_t<std::iter_value_t<InputIt>>>
	std::u8string QuoteObjectNames(InputIt first, InputIt last) {
		std::u8string cache;
		return yycc::string::op::join(
		    [&cache, &first, &last]() -> std::optional<std::u8string_view> {
			    if (first == last) return std::nullopt;
			    // YYC MARK:
			    // We must use "cache", otherwise "use after free" will occur.
			    cache = QuoteObjectName(*(first++));
			    return cache;
		    },
		    u8", ");
	}

#pragma endregion

} // namespace BMapInspector::Ruleset::Shared::Utility

#undef BMAPINSP_O
#undef BMAPINSP_C
#undef BMAPINSP_L
