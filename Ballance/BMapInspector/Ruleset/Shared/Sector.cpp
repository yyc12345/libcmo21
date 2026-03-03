#include "Sector.hpp"
#include <yycc.hpp>
#include <yycc/string/op.hpp>
#include <VTAll.hpp>

namespace strop = yycc::string::op;
namespace L = LibCmo;

namespace BMapInspector::Ruleset::Shared::Sector {

	SectorNameBuilder::SectorNameBuilder() {}

	SectorNameBuilder::~SectorNameBuilder() {}

	SectorName SectorNameBuilder::get_name(L::CKDWORD sector) const {
		if (sector < MIN_SECTOR || sector > MAX_SECTOR) {
			throw std::logic_error("invalid sector number");
		} else {
			if (sector < 9) {
				return strop::printf(u8"Sector_%02" PRIuCKDWORD, sector);
			} else {
				return strop::printf(u8"Sector_%" PRIuCKDWORD, sector);
			}
		}
	}

	Sector9Names SectorNameBuilder::get_sector9_names() const {
		return Sector9Names{.legacy_name = u8"Sector_9", .intuitive_name = u8"Sector_09"};
	}

}
