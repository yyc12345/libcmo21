#include "SOneRules.hpp"
#include "Shared/Utility.hpp"

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Ruleset {

#pragma region SOne Rule 1

	SOneRule1::SOneRule1() : Rule::IRule() {}

	SOneRule1::~SOneRule1() {}

	std::u8string_view SOneRule1::GetRuleName() const {
		return u8"SONE1";
	}

	void SOneRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		auto physicalized_3dobjects = Shared::Utility::FetchPhysicalized3dObjects(ctx);

		for (auto* physicalized_3dobject : physicalized_3dobjects) {
			auto* mesh = physicalized_3dobject->GetCurrentMesh();
			if (mesh == nullptr) {
				reporter.FormatError(u8"Object %s is grouped into physicalization group, but it doesn't have any associated mesh. "
				                     u8"This will cause itself and following objects can not be physicalized.",
				                     Shared::Utility::QuoteObjectName(physicalized_3dobject).c_str());
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
