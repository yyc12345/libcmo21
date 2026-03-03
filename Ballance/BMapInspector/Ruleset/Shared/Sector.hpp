#pragma once
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>

namespace BMapInspector::Ruleset::Shared::Sector {

	constexpr LibCmo::CKDWORD MIN_SECTOR = 1;
	constexpr LibCmo::CKDWORD MAX_SECTOR = 999;

	/**
	 * @brief The type for sector name.
	 */
	using SectorName = std::u8string;

	struct Sector9Names {
		/** The Sector 9 name with "Sector_9" pattern which is accepted by all 999 sector loader */
		std::u8string legacy_name;
		/** The Sector 9 name with "Sector_09" pattern which is only accepted by new 999 sector loader */
		std::u8string intuitive_name;
	};

	/**
	 * @brief The class for building Ballance sector group name.
	 */
	class SectorNameBuilder {
	public:
		SectorNameBuilder();
		~SectorNameBuilder();
		YYCC_DEFAULT_COPY_MOVE(SectorNameBuilder)

	public:
		/**
		 * @brief Get the sector name.
		 * @param[in] sector The sector index.
		 * @return Sector name.
		 * @remarks
		 * If you deliver sector index with 9, its return name is "Sector_9" which is accepted by all 999 sector loader.
		 */
		SectorName get_name(LibCmo::CKDWORD sector) const;
		/**
		 * @brief Get the special sector 9 names.
		 * @return Special built sector 9 names.
		 */
		Sector9Names get_sector9_names() const;
	};

}
