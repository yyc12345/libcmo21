#pragma once

#include <VTAll.hpp>
#include "StringHelper.hpp"
#include <vector>
#include <string>

namespace Unvirt {
	namespace AccessibleValue {

		constexpr const char c_InvalidEnumName[] = "[undefined]";

		struct GeneralReflection { const char* mName; };
		template<typename _Ty>
		using GeneralReflectionArray = std::vector<std::pair<_Ty, GeneralReflection>>;

		template<typename _Ty>
		std::string GetEnumName(_Ty val, const GeneralReflectionArray<_Ty>& desc) {
			std::string strl;
			for (auto& item : desc) {
				if (item.first == val) {
					strl = item.second.mName;
					return strl;
				}
			}
			StringHelper::StdstringPrintf(strl, "%s (0x%08" PRIXCKDWORD ")", 
				c_InvalidEnumName, 
				static_cast<LibCmo::CKDWORD>(val)
			);
			return strl;
		}
		template<typename _Ty>
		std::string GetFlagEnumName(_Ty val, const GeneralReflectionArray<_Ty>& desc, const char* splitor) {
			std::string strl, cache;
			for (auto& item : desc) {
				// if it have exacelt same entry, return directly
				if (item.first == val) {
					StringHelper::StdstringPrintf(strl, "%s (0x%08" PRIXCKDWORD ")",
						item.second.mName,
						static_cast<LibCmo::CKDWORD>(item.first)
					);
					return strl;
				}

				// check flag match
				if (LibCmo::EnumsHelper::Has(val, item.first)) {
					if (strl.size() != 0u && splitor != nullptr) {
						strl += splitor;
					}

					StringHelper::StdstringPrintf(cache, "%s (0x%08" PRIXCKDWORD ")",
						item.second.mName,
						static_cast<LibCmo::CKDWORD>(item.first)
					);
					strl += cache;
				}
			}

			// if nothing was gotten. set to undefined
			if (strl.size() == 0u) {
				StringHelper::StdstringPrintf(strl, "%s (0x%08" PRIXCKDWORD ")", 
					c_InvalidEnumName, 
					static_cast<LibCmo::CKDWORD>(val)
				);
			}

			return strl;
		}

		std::string GetClassIdName(LibCmo::CK2::CK_CLASSID cls);
		std::string GetCkErrorName(LibCmo::CK2::CKERROR err);
		std::string GetClassIdHierarchy(LibCmo::CK2::CK_CLASSID cls);
		std::string GetCkErrorDescription(LibCmo::CK2::CKERROR err);

		std::string GetReadableFileSize(uint64_t size);

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

			extern const GeneralReflectionArray<LibCmo::VxMath::VX_PIXELFORMAT> VX_PIXELFORMAT;
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
			extern const GeneralReflectionArray<LibCmo::VxMath::VXTEXTURE_WRAPMODE> VXTEXTURE_WRAPMODE;
		}

	}
}
