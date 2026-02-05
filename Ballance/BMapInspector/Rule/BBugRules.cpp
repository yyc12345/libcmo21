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

} // namespace BMapInspector::Rule
