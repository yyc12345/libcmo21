#pragma once

#include <VTAll.hpp>
#include <vector>
#include <string>
#include <type_traits>
#include <yycc/string/op.hpp>
#include <yycc/cenum.hpp>

namespace Unvirt::Docstring {

	constexpr char8_t INVALID_ENUM_NAME[] = u8"[undefined]";

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

	struct GenericReflection {
		const char8_t* mName;
	};

	template<typename T>
	    requires std::is_enum_v<T>
	using GenericReflectionArray = std::vector<std::pair<T, GenericReflection>>;

	namespace Arrays {
		// clang-format off
		extern const GenericReflectionArray<LibCmo::CK2::CK_FILE_WRITEMODE> CK_FILE_WRITEMODE;
		extern const GenericReflectionArray<LibCmo::CK2::CK_FO_OPTIONS> CK_FO_OPTIONS;
		extern const GenericReflectionArray<LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS> CK_STATECHUNK_CHUNKOPTIONS;
		extern const GenericReflectionArray<LibCmo::CK2::CK_STATECHUNK_DATAVERSION> CK_STATECHUNK_DATAVERSION;
		extern const GenericReflectionArray<LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION> CK_STATECHUNK_CHUNKVERSION;
		extern const GenericReflectionArray<LibCmo::CK2::CK_OBJECT_FLAGS> CK_OBJECT_FLAGS;
		extern const GenericReflectionArray<LibCmo::CK2::CK_3DENTITY_FLAGS> CK_3DENTITY_FLAGS;
		extern const GenericReflectionArray<LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS> CK_TEXTURE_SAVEOPTIONS;
		extern const GenericReflectionArray<LibCmo::CK2::CK_BITMAPDATA_FLAGS> CK_BITMAPDATA_FLAGS;
		extern const GenericReflectionArray<LibCmo::CK2::CK_CAMERA_PROJECTION> CK_CAMERA_PROJECTION;

		extern const GenericReflectionArray<LibCmo::VxMath::VX_PIXELFORMAT> VX_PIXELFORMAT;
		extern const GenericReflectionArray<LibCmo::VxMath::VXLIGHT_TYPE> VXLIGHT_TYPE;
		extern const GenericReflectionArray<LibCmo::VxMath::VXTEXTURE_BLENDMODE> VXTEXTURE_BLENDMODE;
		extern const GenericReflectionArray<LibCmo::VxMath::VXTEXTURE_FILTERMODE> VXTEXTURE_FILTERMODE;
		extern const GenericReflectionArray<LibCmo::VxMath::VXBLEND_MODE> VXBLEND_MODE;
		extern const GenericReflectionArray<LibCmo::VxMath::VXTEXTURE_ADDRESSMODE> VXTEXTURE_ADDRESSMODE;
		extern const GenericReflectionArray<LibCmo::VxMath::VXFILL_MODE> VXFILL_MODE;
		extern const GenericReflectionArray<LibCmo::VxMath::VXSHADE_MODE> VXSHADE_MODE;
		extern const GenericReflectionArray<LibCmo::VxMath::VXCMPFUNC> VXCMPFUNC;
		extern const GenericReflectionArray<LibCmo::VxMath::VX_EFFECT> VX_EFFECT;
		extern const GenericReflectionArray<LibCmo::VxMath::VX_MOVEABLE_FLAGS> VX_MOVEABLE_FLAGS;
		extern const GenericReflectionArray<LibCmo::VxMath::VXMESH_FLAGS> VXMESH_FLAGS;
		extern const GenericReflectionArray<LibCmo::VxMath::VXMESH_LITMODE> VXMESH_LITMODE;
		extern const GenericReflectionArray<LibCmo::VxMath::VXTEXTURE_WRAPMODE> VXTEXTURE_WRAPMODE;
		// clang-format on
	} // namespace Arrays

	template<typename T>
	    requires std::is_enum_v<T>
	const GenericReflectionArray<T>& GetEnumReflectionArray() {
		if constexpr (std::is_same_v<T, LibCmo::CK2::CK_FILE_WRITEMODE>) {
			return Arrays::CK_FILE_WRITEMODE;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_FO_OPTIONS>) {
			return Arrays::CK_FO_OPTIONS;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS>) {
			return Arrays::CK_STATECHUNK_CHUNKOPTIONS;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_STATECHUNK_DATAVERSION>) {
			return Arrays::CK_STATECHUNK_DATAVERSION;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION>) {
			return Arrays::CK_STATECHUNK_CHUNKVERSION;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_OBJECT_FLAGS>) {
			return Arrays::CK_OBJECT_FLAGS;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_3DENTITY_FLAGS>) {
			return Arrays::CK_3DENTITY_FLAGS;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS>) {
			return Arrays::CK_TEXTURE_SAVEOPTIONS;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_BITMAPDATA_FLAGS>) {
			return Arrays::CK_BITMAPDATA_FLAGS;
		} else if constexpr (std::is_same_v<T, LibCmo::CK2::CK_CAMERA_PROJECTION>) {
			return Arrays::CK_CAMERA_PROJECTION;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VX_PIXELFORMAT>) {
			return Arrays::VX_PIXELFORMAT;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXLIGHT_TYPE>) {
			return Arrays::VXLIGHT_TYPE;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXTEXTURE_BLENDMODE>) {
			return Arrays::VXTEXTURE_BLENDMODE;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXTEXTURE_FILTERMODE>) {
			return Arrays::VXTEXTURE_FILTERMODE;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXBLEND_MODE>) {
			return Arrays::VXBLEND_MODE;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXTEXTURE_ADDRESSMODE>) {
			return Arrays::VXTEXTURE_ADDRESSMODE;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXFILL_MODE>) {
			return Arrays::VXFILL_MODE;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXSHADE_MODE>) {
			return Arrays::VXSHADE_MODE;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXCMPFUNC>) {
			return Arrays::VXCMPFUNC;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VX_EFFECT>) {
			return Arrays::VX_EFFECT;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VX_MOVEABLE_FLAGS>) {
			return Arrays::VX_MOVEABLE_FLAGS;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXMESH_FLAGS>) {
			return Arrays::VXMESH_FLAGS;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXMESH_LITMODE>) {
			return Arrays::VXMESH_LITMODE;
		} else if constexpr (std::is_same_v<T, LibCmo::VxMath::VXTEXTURE_WRAPMODE>) {
			return Arrays::VXTEXTURE_WRAPMODE;
		} else {
			static_assert(false, "Unknown enum type");
		}
	}

	template<typename T>
	    requires std::is_enum_v<T>
	std::u8string GetEnumName(T val) {
		std::u8string strl;
		for (auto& item : GetEnumReflectionArray<T>()) {
			if (item.first == val) {
				strl = item.second.mName;
				return strl;
			}
		}
		yycc::string::op::printf(strl, u8"%s (0x%08" PRIXCKDWORD ")", INVALID_ENUM_NAME, static_cast<LibCmo::CKDWORD>(val));
		return strl;
	}

	template<typename T>
	    requires std::is_enum_v<T>
	std::u8string GetFlagEnumName(T val, const char8_t* splitor = u8" ", const char8_t* indent = u8"") {
		std::u8string strl, cache;
		for (auto& item : GetEnumReflectionArray<T>()) {
			// if it have exacelt same entry, return directly
			if (item.first == val) {
				yycc::string::op::printf(strl, u8"%s (0x%08" PRIXCKDWORD ")", item.second.mName, static_cast<LibCmo::CKDWORD>(item.first));
				return strl;
			}

			// check flag match
			if (yycc::cenum::has(val, item.first)) {
				// add splittor if it not the first entry
				if (strl.size() != 0u && splitor != nullptr) {
					strl += splitor;
				}

				// add indent if possible
				if (indent != nullptr) {
					strl += indent;
				}

				// add value self.
				yycc::string::op::printf(cache, u8"%s (0x%08" PRIXCKDWORD ")", item.second.mName, static_cast<LibCmo::CKDWORD>(item.first));
				strl += cache;
			}
		}

		// if nothing was gotten. set to undefined
		if (strl.size() == 0u) {
			yycc::string::op::printf(strl, u8"%s (0x%08" PRIXCKDWORD ")", INVALID_ENUM_NAME, static_cast<LibCmo::CKDWORD>(val));
		}

		return strl;
	}

#pragma endregion

} // namespace Unvirt::Docstring
