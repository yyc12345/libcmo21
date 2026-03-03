#include "BBugRules.hpp"
#include "Shared.hpp"

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Rule {

#pragma region BBug Rule 1

	constexpr char8_t BBUG1[] = u8"BBUG1";

	BBugRule1::BBugRule1() : IRule() {}

	BBugRule1::~BBugRule1() {}

	std::u8string_view BBugRule1::GetRuleName() const {
		return BBUG1;
	}

	void BBugRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		if (!level.GetTargetLights().empty()) {
			reporter.WriteInfo(BBUG1, u8"Using light in map is not suggested.");
		}
	}

#pragma endregion

#pragma region BBug Rule 2

	constexpr char8_t BBUG2[] = u8"BBUG2";

	BBugRule2::BBugRule2() : IRule() {}

	BBugRule2::~BBugRule2() {}

	std::u8string_view BBugRule2::GetRuleName() const {
		return BBUG2;
	}

	void BBugRule2::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		if (!level.GetTargetCameras().empty()) {
			reporter.WriteInfo(BBUG2, u8"Using camera in map is not suggested.");
		}
	}

#pragma endregion

#pragma region BBug Rule 3

	constexpr char8_t BBUG3[] = u8"BBUG3";

	BBugRule3::BBugRule3() : IRule() {}

	BBugRule3::~BBugRule3() {}

	std::u8string_view BBugRule3::GetRuleName() const {
		return BBUG3;
	}

	void BBugRule3::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		// TODO:
		// This function is already presented in Ballance Blender Plugin,
		// so I don't want write it in there now.
		// Write this if I have spare time in future.
	}

#pragma endregion

} // namespace BMapInspector::Rule
