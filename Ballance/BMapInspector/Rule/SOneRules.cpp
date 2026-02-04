#include "SOneRules.hpp"
#include "Shared.hpp"

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Rule {

#pragma region SOne Rule 1

	constexpr char8_t SONE1[] = u8"SONE1";

	SOneRule1::SOneRule1() : IRule() {}

	SOneRule1::~SOneRule1() {}

	std::u8string_view SOneRule1::GetRuleName() const {
		return SONE1;
	}

	void SOneRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		auto physicalized_3dobjects = Shared::FetchPhysicalized3dObjects(ctx);

		for (auto* physicalized_3dobject : physicalized_3dobjects) {
			auto* mesh = physicalized_3dobject->GetCurrentMesh();
			if (mesh == nullptr) {
				reporter.FormatError(
				    SONE1,
				    u8R"(Object "%s" is grouped into physicalization group, but it doesn't have any associated mesh. This will cause itself and following objects can not be physicalized.)",
				    Shared::RenderObjectName(physicalized_3dobject));
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
