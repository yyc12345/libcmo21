#pragma once

#include <VTConstants.hpp>
#include <vector>
#include <string>

namespace Unvirt {
	namespace AccessibleValue {

		template<typename TEnum>
		using EnumDescPairArray = std::vector<std::pair<TEnum, const char*>>;

		namespace EnumDesc {
			extern const EnumDescPairArray<LibCmo::CK_FILE_WRITEMODE> CK_FILE_WRITEMODE;
			extern const EnumDescPairArray<LibCmo::CK_LOAD_FLAGS> CK_LOAD_FLAGS;
			extern const EnumDescPairArray<LibCmo::CK_FO_OPTIONS> CK_FO_OPTIONS;
		}

		template<typename TEnum>
		void GetEnumName(const EnumDescPairArray<TEnum> desc, std::string& strl, TEnum val);
		template<typename TEnum>
		void GetFlagEnumName(const EnumDescPairArray<TEnum> desc, std::string& strl, TEnum val);

		void GetClassIdName(std::string& strl, LibCmo::CK_CLASSID cls);
		void GetCkErrorName(std::string& strl, LibCmo::CKERROR err);
		void GetClassIdHierarchy(std::string& strl, LibCmo::CK_CLASSID cls);
		void GetCkErrorDescription(std::string& strl, LibCmo::CKERROR err);

		void GetAccessibleFileSize(std::string& strl, uint64_t size);

	}
}
