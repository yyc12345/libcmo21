#pragma once

#include <CKEnums.hpp>
#include <VTUtils.hpp>
#include <vector>
#include <string>

namespace Unvirt {
	namespace AccessibleValue {

		constexpr const char c_InvalidEnumName[] = "[undefined]";

#pragma region universal enum name

		template<typename TEnum>
		using EnumDescPairArray = std::vector<std::pair<TEnum, const char*>>;

		namespace EnumDesc {
			extern const EnumDescPairArray<LibCmo::CK2::CK_FILE_WRITEMODE> CK_FILE_WRITEMODE;
			extern const EnumDescPairArray<LibCmo::CK2::CK_LOAD_FLAGS> CK_LOAD_FLAGS;
			extern const EnumDescPairArray<LibCmo::CK2::CK_FO_OPTIONS> CK_FO_OPTIONS;
			extern const EnumDescPairArray<LibCmo::CK2::CK_PLUGIN_TYPE> CK_PLUGIN_TYPE;
		}

		template<typename TEnum>
		std::string GetEnumName(const EnumDescPairArray<TEnum>& desc, TEnum val) {
			std::string strl;
			for (auto it = desc.begin(); it != desc.end(); ++it) {
				if ((*it).first == val) {
					strl = (*it).second;
					return strl;
				}
			}
			strl = c_InvalidEnumName;
			return strl;
		}
		template<typename TEnum>
		std::string GetFlagEnumName(const EnumDescPairArray<TEnum>& desc, TEnum val) {
			std::string strl;
			for (auto it = desc.begin(); it != desc.end(); ++it) {
				// if it have exacelt same entry, return directly
				if ((*it).first == val) {
					strl = (*it).second;
					return strl;
				}

				// check flag match
				if (LibCmo::EnumsHelper::FlagEnumHas(val, (*it).first)) {
					// matched, add it
					if (strl.size() != 0u) strl += ", ";
					strl += (*it).second;
				}
			}

			if (strl.size() == 0u) {
				// nothing was gotten. set to undefined
				strl = c_InvalidEnumName;
			} // otherwise return directly
			return strl;
		}

#pragma endregion

		std::string GetClassIdName(LibCmo::CK2::CK_CLASSID cls);
		std::string GetCkErrorName(LibCmo::CK2::CKERROR err);
		std::string GetClassIdHierarchy(LibCmo::CK2::CK_CLASSID cls);
		std::string GetCkErrorDescription(LibCmo::CK2::CKERROR err);

		std::string GetAccessibleFileSize(uint64_t size);

	}
}
