#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Ruleset {
    
    /**
     * @brief ZZQ Rule 1
	 * @details
	 * Only the first object grouped into "Phys_FloorStopper" can make sound in game.
	 * So it would be better to make "Phys_FloorStopper" only have one item.
	 * 
	 * At the same time, show which object is the first object in "Phys_FloorStopper" 
	 * to know which object can make sound in game, 
	 * if mapper require the stopper which can not make sound them by design.
     */
    class ZZQRule1 : public Rule::IRule {
	public:
		ZZQRule1();
		virtual ~ZZQRule1();
		YYCC_DELETE_COPY_MOVE(ZZQRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief ZZQ Rule 2
	 * @details
	 * The Ballance should only be included only one group.
	 * This rule will check whether there is intersection between different sector group.
	 */
	class ZZQRule2 : public Rule::IRule {
	public:
		ZZQRule2();
		virtual ~ZZQRule2();
		YYCC_DELETE_COPY_MOVE(ZZQRule2)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief ZZQ Rule 3
	 * @details
	 * A minimalist level must contains following items:
	 * \li One start point.
	 * \li One end point (spaceship).
	 * \li One reset point.
	 * \li "Sector_01" group.
	 */
	class ZZQRule3 : public Rule::IRule {
	public:
		ZZQRule3();
		virtual ~ZZQRule3();
		YYCC_DELETE_COPY_MOVE(ZZQRule3)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

}
