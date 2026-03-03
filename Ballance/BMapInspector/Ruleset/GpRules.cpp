#include "GpRules.hpp"
#include "Shared/Utility.hpp"
#include "Shared/Sector.hpp"
#include <set>

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Ruleset {

#pragma region GP1 Rule

	// Reference: https://tieba.baidu.com/p/3182981807

	GpRule1::GpRule1() : Rule::IRule() {}

	GpRule1::~GpRule1() {}

	std::u8string_view GpRule1::GetRuleName() const {
		return u8"GP1";
	}

	void GpRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		// TODO:
		// Finish this rule.
		// It is so complex that I don't want to implement it now.
	}

#pragma endregion

#pragma region GP2 Rule

	GpRule2::GpRule2() : Rule::IRule() {}

	GpRule2::~GpRule2() {}

	std::u8string_view GpRule2::GetRuleName() const {
		return u8"GP2";
	}

	void GpRule2::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		Shared::Sector::SectorNameBuilder builder;

		// We need collect all group names first,
		// becuase in following code we need frequent visit them
		std::set<std::u8string> group_names;
		for (auto* group : level.GetGroups()) {
			auto group_name = group->GetName();
			if (group_name != nullptr) {
				group_names.emplace(std::u8string(group_name));
			}
		}

		// Check the sector count of this game.
		L::CKDWORD sector_count;
		for (sector_count = Shared::Sector::MIN_SECTOR; sector_count <= Shared::Sector::MAX_SECTOR; ++sector_count) {
			// Build name first with special treat for sector 9
			if (sector_count != 9) {
				auto sector_name = builder.get_name(sector_count);
				if (!group_names.contains(sector_name)) {
					if (sector_count == Shared::Sector::MIN_SECTOR) {
						reporter.WriteError(u8"Can not find any reasonable sector group in your map.");
						return;
					} else {
						break;
					}
				}
			} else {
				auto sector_names = builder.get_sector9_names();
				bool has_legacy_sector = group_names.contains(sector_names.legacy_name);
				bool has_intuitive_sector = group_names.contains(sector_names.intuitive_name);
				if (!has_legacy_sector && !has_intuitive_sector) {
					break;
				}
				if (has_legacy_sector && has_intuitive_sector) {
					reporter.FormatError(u8"Found %s and %s at the same map. This is not allowed.",
					                     Shared::Utility::QuoteText(sector_names.legacy_name).c_str(),
					                     Shared::Utility::QuoteText(sector_names.intuitive_name).c_str());
					return;
				}
			}
		}
		// Now sector_count is the first sector which can not find,
		// so we need minus one on it.
		--sector_count;

		// Report sector count info.
		reporter.FormatInfo(u8"Your map has %" PRIuCKDWORD " sector(s).", sector_count);
		// Report special warning for map which only contain 1 sector.
		if (sector_count == 1) {
			reporter.WriteWarning(u8"Your map only have one sector. "
			                      u8"This is okey but not suggested because it will cause mosaic issue on the flames of checkpoint. "
			                      u8"Consider adding another sector to resolve this issue.");
		}
		// Report warning for sector greater than 8.
		if (sector_count > 8) {
			reporter.WriteWarning(u8"You are creating a map with more than 8 sectors. "
			                      u8"This will cause vanilla Ballance freezed when loading it. "
			                      u8"Please make sure that all players of your map have properly set 999 sector loader up.");
		}
		// If there is sector 9, check its kind and report wanring if it is intuitive kind.
		if (sector_count > 8) {
			auto sector_names = builder.get_sector9_names();
			if (group_names.contains(sector_names.intuitive_name)) {
				reporter.FormatWarning(u8"You are using intuitive sector name, %s, for sector 9. "
				                       u8"This is only accepted by new 999 sector loader.",
				                       Shared::Utility::QuoteText(sector_names.intuitive_name).c_str());
			}
		}

		// We continue check following sectors to make sure that all sector is successive.
		for (L::CKDWORD i = sector_count + 1; i <= Shared::Sector::MAX_SECTOR; ++i) {
			if (i != 9) {
				auto sector_name = builder.get_name(i);
				if (group_names.contains(sector_name)) {
					reporter.FormatError(u8"Found group %s unexpected. "
					                     u8"Please check whether sector groups are successive in your map.",
					                     Shared::Utility::QuoteText(sector_name).c_str());
				}
			} else {
				auto sector_names = builder.get_sector9_names();
				bool has_legacy_sector = group_names.contains(sector_names.legacy_name);
				bool has_intuitive_sector = group_names.contains(sector_names.intuitive_name);
				if (has_legacy_sector || has_intuitive_sector) {
					reporter.FormatError(u8"Found group %s or %s unexpected. "
					                     u8"Please check whether sector groups are successive in your map.",
					                     Shared::Utility::QuoteText(sector_names.legacy_name).c_str(),
					                     Shared::Utility::QuoteText(sector_names.intuitive_name).c_str());
				}
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
