#include "ZZQRules.hpp"
#include "Shared.hpp"

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Rule {

#pragma region ZZQ Rule 1

	constexpr char8_t ZZQ1[] = u8"ZZQ1";

	ZZQRule1::ZZQRule1() : IRule() {}

	ZZQRule1::~ZZQRule1() {}

	std::u8string_view ZZQRule1::GetRuleName() const {
		return ZZQ1;
	}

	void ZZQRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();

		// We get "Phys_FloorStopper" group first.
		auto* phys_floorstopper = Shared::FetchGroup(ctx, Shared::GroupNames::PHYS_FLOORSTOPPER);
		if (phys_floorstopper == nullptr) return;
		// We iterate all object grouped into it.
		auto group_3dobjects = Shared::Iter3dObjects(phys_floorstopper);

		// Show the first object if it have.
		if (!group_3dobjects.empty()) {
			auto* first_3dobjects = group_3dobjects.front();
			reporter.FormatInfo(
			    ZZQ1,
			    u8R"(Object "%s" is the first object grouped into "Phys_FloorStopper". It is the only stopper which can make sound in game.)",
			    Shared::RenderObjectName(first_3dobjects));
		}

		// Warning for other objects
		for (size_t i = 1; i < group_3dobjects.size(); ++i) {
			auto* other_3dobject = group_3dobjects[i];
			reporter.FormatWarning(
			    ZZQ1,
			    u8R"(Object "%s" is grouped into "Phys_FloorStopper" but it is not the only object. This will cause it can not make sound in game. Please confirm this is by your intention.)",
			    Shared::RenderObjectName(other_3dobject));
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
