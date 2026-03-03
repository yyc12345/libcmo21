#include "ZZQRules.hpp"
#include "Shared/Utility.hpp"
#include "Shared/Name.hpp"
#include "Shared/Sector.hpp"
#include <vector>
#include <set>
#include <algorithm>

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Ruleset {

#pragma region ZZQ Rule 1

	ZZQRule1::ZZQRule1() : Rule::IRule() {}

	ZZQRule1::~ZZQRule1() {}

	std::u8string_view ZZQRule1::GetRuleName() const {
		return u8"ZZQ1";
	}

	void ZZQRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();

		// We get "Phys_FloorStopper" group first.
		auto* phys_floorstopper = Shared::Utility::FetchGroup(ctx, Shared::Name::Group::PHYS_FLOORSTOPPER);
		if (phys_floorstopper == nullptr) return;
		// We iterate all object grouped into it.
		auto group_3dobjects = Shared::Utility::Iter3dObjects(phys_floorstopper);

		// Show the first object if it have.
		if (!group_3dobjects.empty()) {
			auto* first_3dobjects = group_3dobjects.front();
			reporter.FormatInfo(u8"Object %s is the first object grouped into %s. "
			                    u8"It is the only stopper which can make sound in game.",
			                    Shared::Utility::QuoteObjectName(first_3dobjects).c_str(),
			                    Shared::Utility::QuoteText(Shared::Name::Group::PHYS_FLOORSTOPPER).c_str());
		}

		// Warning for other objects
		for (size_t i = 1; i < group_3dobjects.size(); ++i) {
			auto* other_3dobject = group_3dobjects[i];
			reporter.FormatWarning(u8"Object %s is grouped into %s but it is not the only object. "
			                       u8"This will cause it can not make sound in game. Please confirm this is by your intention. "
			                       u8"If you want it can make sound, please join it into the first object located in that group.",
			                       Shared::Utility::QuoteObjectName(other_3dobject).c_str(),
			                       Shared::Utility::QuoteText(Shared::Name::Group::PHYS_FLOORSTOPPER).c_str());
		}
	}

#pragma endregion

#pragma region ZZQ Rule 2

	ZZQRule2::ZZQRule2() : Rule::IRule() {}

	ZZQRule2::~ZZQRule2() {}

	std::u8string_view ZZQRule2::GetRuleName() const {
		return u8"ZZQ2";
	}

	void ZZQRule2::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		Shared::Sector::SectorNameBuilder builder;

		// Extract group objects info
		std::vector<std::set<O::CK3dObject*>> sector_objects;
		for (L::CKDWORD i = Shared::Sector::MIN_SECTOR; i <= Shared::Sector::MAX_SECTOR; ++i) {
			// Prepare inserted object set.
			std::set<O::CK3dObject*> object_set;

			// Build name first with special treat for sector 9
			// and fill objects into set.
			if (i != 9) {
				auto sector_name = builder.get_name(i);
				auto* sector = Shared::Utility::FetchGroup(ctx, sector_name.c_str());
				if (sector == nullptr) break;

				auto group_3dobjects = Shared::Utility::Iter3dObjects(sector);
				for (auto* group_3dobject : group_3dobjects) {
					object_set.emplace(group_3dobject);
				}
			} else {
				auto sector_names = builder.get_sector9_names();
				auto* legacy_sector = Shared::Utility::FetchGroup(ctx, sector_names.legacy_name.c_str());
				auto* intuitive_sector = Shared::Utility::FetchGroup(ctx, sector_names.intuitive_name.c_str());
				if (legacy_sector == nullptr && intuitive_sector == nullptr) break;

				if (legacy_sector != nullptr) {
					auto group_3dobjects = Shared::Utility::Iter3dObjects(legacy_sector);
					for (auto* group_3dobject : group_3dobjects) {
						object_set.emplace(group_3dobject);
					}
				}
				if (intuitive_sector != nullptr) {
					auto group_3dobjects = Shared::Utility::Iter3dObjects(intuitive_sector);
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
					reporter.FormatWarning(u8"Some objects are grouped into sector %" PRIuCKDWORD " and sector %" PRIuCKDWORD
					                       " represented group bothly. This is not allowed. These objects are: %s.",
					                       left_sector_idx,
					                       right_sector_idx,
					                       Shared::Utility::QuoteObjectNames(intersection.begin(), intersection.end()).c_str());
				}
			}
		}
	}

#pragma endregion

#pragma region ZZQ Rule 3

	ZZQRule3::ZZQRule3() : Rule::IRule() {}

	ZZQRule3::~ZZQRule3() {}

	std::u8string_view ZZQRule3::GetRuleName() const {
		return u8"ZZQ3";
	}

	void ZZQRule3::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		Shared::Sector::SectorNameBuilder builder;

		auto* level_start = Shared::Utility::FetchGroup(ctx, Shared::Name::Group::PS_LEVELSTART);
		if (level_start == nullptr) {
			reporter.FormatError(u8"Incomplete level: can not find %s group.",
			                     Shared::Utility::QuoteText(Shared::Name::Group::PS_LEVELSTART).c_str());
		} else {
			switch (level_start->GetObjectCount()) {
				case 0:
					reporter.FormatError(u8"Incomplete level: there is no object grouped into %s group.",
					                     Shared::Utility::QuoteText(Shared::Name::Group::PS_LEVELSTART).c_str());
					break;
				case 1:
					// OK. Do nothing.
					break;
				default:
					reporter.FormatError(u8"Bad level: there are more than one objects grouped into %s group.",
					                     Shared::Utility::QuoteText(Shared::Name::Group::PS_LEVELSTART).c_str());
					break;
			}
		}

		auto* level_end = Shared::Utility::FetchGroup(ctx, Shared::Name::Group::PE_LEVELENDE);
		if (level_end == nullptr) {
			reporter.FormatError(u8"Incomplete level: can not find %s group.",
			                     Shared::Utility::QuoteText(Shared::Name::Group::PE_LEVELENDE).c_str());
		} else {
			switch (level_end->GetObjectCount()) {
				case 0:
					reporter.FormatError(u8"Incomplete level: there is no object grouped into %s group.",
					                     Shared::Utility::QuoteText(Shared::Name::Group::PE_LEVELENDE).c_str());
					break;
				case 1:
					// OK. Do nothing.
					break;
				default:
					reporter.FormatError(u8"Bad level: there are more than one objects grouped into %s group.",
					                     Shared::Utility::QuoteText(Shared::Name::Group::PE_LEVELENDE).c_str());
					break;
			}
		}

		auto* check_points = Shared::Utility::FetchGroup(ctx, Shared::Name::Group::PC_CHECKPOINTS);
		if (check_points == nullptr) {
			reporter.FormatWarning(u8"Can not find %s group. This will cause bad render of particle at the level start point.",
			                       Shared::Utility::QuoteText(Shared::Name::Group::PC_CHECKPOINTS).c_str());
		}

		auto* reset_points = Shared::Utility::FetchGroup(ctx, Shared::Name::Group::PR_RESETPOINTS);
		if (reset_points == nullptr) {
			reporter.FormatError(u8"Incomplete level: can not find %s group.",
			                     Shared::Utility::QuoteText(Shared::Name::Group::PR_RESETPOINTS).c_str());
		} else {
			if (reset_points->GetObjectCount() == 0) {
				reporter.FormatError(u8"Incomplete level: there is no object grouped into %s group.",
				                     Shared::Utility::QuoteText(Shared::Name::Group::PR_RESETPOINTS).c_str());
			}
		}

		auto sector1_name = builder.get_name(1);
		auto* sector1 = Shared::Utility::FetchGroup(ctx, sector1_name.c_str());
		if (sector1 == nullptr) {
			reporter.FormatError(u8"Incomplete level: can not find %s group.", Shared::Utility::QuoteText(sector1_name).c_str());
		}
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
