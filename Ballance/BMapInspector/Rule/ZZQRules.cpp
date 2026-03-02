#include "ZZQRules.hpp"
#include "Shared.hpp"
#include <vector>
#include <set>
#include <algorithm>

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
			    u8R"(Object %s is the first object grouped into "Phys_FloorStopper". It is the only stopper which can make sound in game.)",
			    Shared::QuoteObjectName(first_3dobjects).c_str());
		}

		// Warning for other objects
		for (size_t i = 1; i < group_3dobjects.size(); ++i) {
			auto* other_3dobject = group_3dobjects[i];
			reporter.FormatWarning(
			    ZZQ1,
			    u8R"(Object %s is grouped into "Phys_FloorStopper" but it is not the only object. This will cause it can not make sound in game. Please confirm this is by your intention.)",
			    Shared::QuoteObjectName(other_3dobject).c_str());
		}
	}

#pragma endregion

#pragma region ZZQ Rule 2

	constexpr char8_t ZZQ2[] = u8"ZZQ2";

	ZZQRule2::ZZQRule2() : IRule() {}

	ZZQRule2::~ZZQRule2() {}

	std::u8string_view ZZQRule2::GetRuleName() const {
		return ZZQ2;
	}

	void ZZQRule2::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		Shared::SectorNameBuilder builder;

		// Extract group objects info
		std::vector<std::set<O::CK3dObject*>> sector_objects;
		for (L::CKDWORD i = Shared::MIN_SECTOR; i <= Shared::MAX_SECTOR; ++i) {
			// Prepare inserted object set.
			std::set<O::CK3dObject*> object_set;

			// Build name first with special treat for sector 9
			// and fill objects into set.
			if (i != 9) {
				auto sector_name = builder.get_name(i);
				auto* sector = Shared::FetchGroup(ctx, sector_name.c_str());
				if (sector == nullptr) break;

				auto group_3dobjects = Shared::Iter3dObjects(sector);
				for (auto* group_3dobject : group_3dobjects) {
					object_set.emplace(group_3dobject);
				}
			} else {
				auto sector_names = builder.get_sector9_names();
				auto* legacy_sector = Shared::FetchGroup(ctx, sector_names.legacy_name.c_str());
				auto* intuitive_sector = Shared::FetchGroup(ctx, sector_names.intuitive_name.c_str());
				if (legacy_sector == nullptr && intuitive_sector == nullptr) break;

				if (legacy_sector != nullptr) {
					auto group_3dobjects = Shared::Iter3dObjects(legacy_sector);
					for (auto* group_3dobject : group_3dobjects) {
						object_set.emplace(group_3dobject);
					}
				}
				if (intuitive_sector != nullptr) {
					auto group_3dobjects = Shared::Iter3dObjects(intuitive_sector);
					for (auto* group_3dobject : group_3dobjects) {
						object_set.emplace(group_3dobject);
					}
				}
			}

			// Insert object set
			sector_objects.emplace_back(std::move(object_set));
		}

		// Check the intersection one by one
		for (size_t i = 0; i < sector_objects.size(); ++i) {
			for (size_t j = i + 1; j < sector_objects.size(); ++j) {
				// Fetch 2 set repsectively.
				const auto& left_sector = sector_objects[i];
				const auto& right_sector = sector_objects[j];
				// Check duplicated objects
				std::vector<O::CK3dObject*> intersection;
				std::set_intersection(left_sector.begin(),
				                      left_sector.end(),
				                      right_sector.begin(),
				                      right_sector.end(),
				                      std::back_inserter(intersection));

				// Output if there is intersection
				if (!intersection.empty()) {
					// Get sector index.
					auto left_sector_idx = static_cast<L::CKDWORD>(i + 1);
					auto right_sector_idx = static_cast<L::CKDWORD>(j + 1);

					// Output result.
					reporter.FormatWarning(ZZQ2,
					                       u8"Some objects are grouped into sector %" PRIuCKDWORD " and sector %" PRIuCKDWORD
					                       " represented group bothly. This is not allowed. These objects are: %s",
					                       left_sector_idx,
					                       right_sector_idx,
					                       Shared::QuoteObjectNames(intersection.begin(), intersection.end()).c_str());
				}
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
