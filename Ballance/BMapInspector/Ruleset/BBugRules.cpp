#include "BBugRules.hpp"

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Ruleset {

#pragma region BBug Rule 1

	BBugRule1::BBugRule1() : Rule::IRule() {}

	BBugRule1::~BBugRule1() {}

	std::u8string_view BBugRule1::GetRuleName() const {
		return u8"BBUG1";
	}

	void BBugRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		if (!level.GetTargetLights().empty()) {
			reporter.WriteInfo(u8"Using light in map is not suggested.");
		}
	}

#pragma endregion

#pragma region BBug Rule 2

	BBugRule2::BBugRule2() : Rule::IRule() {}

	BBugRule2::~BBugRule2() {}

	std::u8string_view BBugRule2::GetRuleName() const {
		return u8"BBUG2";
	}

	void BBugRule2::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		if (!level.GetTargetCameras().empty()) {
			reporter.WriteInfo(u8"Using camera in map is not suggested.");
		}
	}

#pragma endregion

#pragma region BBug Rule 3

	BBugRule3::BBugRule3() : Rule::IRule() {}

	BBugRule3::~BBugRule3() {}

	std::u8string_view BBugRule3::GetRuleName() const {
		return u8"BBUG3";
	}

	void BBugRule3::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		// TODO:
		// This function is already presented in Ballance Blender Plugin,
		// so I don't want write it in there now.
		// Write this if I have spare time in future.
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
