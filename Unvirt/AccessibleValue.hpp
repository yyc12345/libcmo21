#pragma once

#include <VTAll.hpp>
#include <vector>
#include <string>
#include <type_traits>

namespace Unvirt {
	namespace AccessibleValue {

		constexpr char8_t c_InvalidEnumName[] = u8"[undefined]";

#pragma region Size Formatter

		std::u8string GetReadableFileSize(uint64_t _size);

#pragma endregion

#pragma region CKERROR CK_CLASSID Data

		std::u8string GetCkErrorName(LibCmo::CK2::CKERROR err);
		std::u8string GetCkErrorDescription(LibCmo::CK2::CKERROR err);

		std::u8string GetClassIdName(LibCmo::CK2::CK_CLASSID cls);
		std::u8string GetClassIdHierarchy(LibCmo::CK2::CK_CLASSID cls);

#pragma endregion

#pragma region Other Enums

		struct GeneralReflection { const char8_t* mName; };
		template<typename _Ty, std::enable_if_t<std::is_enum_v<_Ty>, int> = 0>
		using GeneralReflectionArray = std::vector<std::pair<_Ty, GeneralReflection>>;

		template<typename _Ty, std::enable_if_t<std::is_enum_v<_Ty>, int> = 0>
		std::u8string GetEnumName(_Ty val, const GeneralReflectionArray<_Ty>& desc) {
			std::u8string strl;
			for (auto& item : desc) {
				if (item.first == val) {
					strl = item.second.mName;
					return strl;
				}
			}
			YYCC::StringHelper::Printf(strl, u8"%s (0x%08" PRIXCKDWORD ")",
				c_InvalidEnumName,
				static_cast<LibCmo::CKDWORD>(val)
			);
			return strl;
		}
		template<typename _Ty, std::enable_if_t<std::is_enum_v<_Ty>, int> = 0>
		std::u8string GetFlagEnumName(_Ty val, const GeneralReflectionArray<_Ty>& desc, const char8_t* splitor = u8" ", const char8_t* indent = u8"") {
			std::u8string strl, cache;
			for (auto& item : desc) {
				// if it have exacelt same entry, return directly
				if (item.first == val) {
					YYCC::StringHelper::Printf(strl, u8"%s (0x%08" PRIXCKDWORD ")",
						item.second.mName,
						static_cast<LibCmo::CKDWORD>(item.first)
					);
					return strl;
				}

				// check flag match
				if (YYCC::EnumHelper::Has(val, item.first)) {
					// add splittor if it not the first entry
					if (strl.size() != 0u && splitor != nullptr) {
						strl += splitor;
					}

					// add indent if possible
					if (indent != nullptr) {
						strl += indent;
					}

					// add value self.
					YYCC::StringHelper::Printf(cache, u8"%s (0x%08" PRIXCKDWORD ")",
						item.second.mName,
						static_cast<LibCmo::CKDWORD>(item.first)
					);
					strl += cache;
				}
			}

			// if nothing was gotten. set to undefined
			if (strl.size() == 0u) {
				YYCC::StringHelper::Printf(strl, u8"%s (0x%08" PRIXCKDWORD ")",
					c_InvalidEnumName,
					static_cast<LibCmo::CKDWORD>(val)
				);
			}

			return strl;
		}

		namespace EnumDesc {
			extern const GeneralReflectionArray<LibCmo::CK2::CK_FILE_WRITEMODE> CK_FILE_WRITEMODE;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_FO_OPTIONS> CK_FO_OPTIONS;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS> CK_STATECHUNK_CHUNKOPTIONS;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_STATECHUNK_DATAVERSION> CK_STATECHUNK_DATAVERSION;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION> CK_STATECHUNK_CHUNKVERSION;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_OBJECT_FLAGS> CK_OBJECT_FLAGS;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_3DENTITY_FLAGS> CK_3DENTITY_FLAGS;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS> CK_TEXTURE_SAVEOPTIONS;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_BITMAPDATA_FLAGS> CK_BITMAPDATA_FLAGS;
			extern const GeneralReflectionArray<LibCmo::CK2::CK_CAMERA_PROJECTION> CK_CAMERA_PROJECTION;

			extern const GeneralReflectionArray<LibCmo::VxMath::VX_PIXELFORMAT> VX_PIXELFORMAT;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXLIGHT_TYPE> VXLIGHT_TYPE;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXTEXTURE_BLENDMODE> VXTEXTURE_BLENDMODE;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXTEXTURE_FILTERMODE> VXTEXTURE_FILTERMODE;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXBLEND_MODE> VXBLEND_MODE;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXTEXTURE_ADDRESSMODE> VXTEXTURE_ADDRESSMODE;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXFILL_MODE> VXFILL_MODE;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXSHADE_MODE> VXSHADE_MODE;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXCMPFUNC> VXCMPFUNC;
			extern const GeneralReflectionArray<LibCmo::VxMath::VX_EFFECT> VX_EFFECT;
			extern const GeneralReflectionArray<LibCmo::VxMath::VX_MOVEABLE_FLAGS> VX_MOVEABLE_FLAGS;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXMESH_FLAGS> VXMESH_FLAGS;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXMESH_LITMODE> VXMESH_LITMODE;
			extern const GeneralReflectionArray<LibCmo::VxMath::VXTEXTURE_WRAPMODE> VXTEXTURE_WRAPMODE;
		}

#pragma endregion

	}
}
