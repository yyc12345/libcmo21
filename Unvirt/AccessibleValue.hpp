#pragma once

#include <VTConstants.hpp>
#include <vector>
#include <string>

namespace Unvirt {
	namespace AccessibleValue {

		extern const char c_InvalidEnumName[];

#pragma region universal enum name

		template<typename TEnum>
		using EnumDescPairArray = std::vector<std::pair<TEnum, const char*>>;

		namespace EnumDesc {
			extern const EnumDescPairArray<LibCmo::CK_FILE_WRITEMODE> CK_FILE_WRITEMODE;
			extern const EnumDescPairArray<LibCmo::CK_LOAD_FLAGS> CK_LOAD_FLAGS;
			extern const EnumDescPairArray<LibCmo::CK_FO_OPTIONS> CK_FO_OPTIONS;
		}

		template<typename TEnum>
		void GetEnumName(const EnumDescPairArray<TEnum>& desc, std::string& strl, TEnum val) {
			for (auto it = desc.begin(); it != desc.end(); ++it) {
				if ((*it).first == val) {
					strl = (*it).second;
					return;
				}
			}
			strl = c_InvalidEnumName;
		}
		template<typename TEnum>
		void GetFlagEnumName(const EnumDescPairArray<TEnum>& desc, std::string& strl, TEnum val) {
			strl.clear();
			for (auto it = desc.begin(); it != desc.end(); ++it) {
				// if it have exacelt same entry, return directly
				if ((*it).first == val) {
					strl = (*it).second;
					return;
				}

				// check flag match
				if (LibCmo::EnumHelper::FlagEnumHas(val, (*it).first)) {
					// matched, add it
					if (strl.size() != 0u) strl += ", ";
					strl += (*it).second;
				}
			}

			if (strl.size() == 0u) {
				// nothing was gotten. set to undefined
				strl = c_InvalidEnumName;
			} // otherwise return directly
		}

#pragma endregion

		void GetClassIdName(std::string& strl, LibCmo::CK_CLASSID cls);
		void GetCkErrorName(std::string& strl, LibCmo::CKERROR err);
		void GetClassIdHierarchy(std::string& strl, LibCmo::CK_CLASSID cls);
		void GetCkErrorDescription(std::string& strl, LibCmo::CKERROR err);

		void GetAccessibleFileSize(std::string& strl, uint64_t size);

	}
}
