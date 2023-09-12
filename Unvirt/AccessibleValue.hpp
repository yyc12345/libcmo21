#pragma once

#include <VTAll.hpp>
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
			strl = c_InvalidEnumName;
			return strl;
		}
		template<typename _Ty>
		std::string GetFlagEnumName(_Ty val, const GeneralReflectionArray<_Ty>& desc) {
			std::string strl;
			for (auto& item : desc) {
				// if it have exacelt same entry, return directly
				if (item.first == val) {
					strl = item.second.mName;
					return strl;
				}

				// check flag match
				if (LibCmo::EnumsHelper::Has(val, item.first)) {
					if (strl.size() != 0u) strl += ", ";
					strl += item.second.mName;
				}
			}

			// if nothing was gotten. set to undefined
			if (strl.size() == 0u) {
				strl = c_InvalidEnumName;
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
		}

	}
}
