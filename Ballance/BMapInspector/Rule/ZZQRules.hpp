#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {
    
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
    class ZZQRule1 : public IRule {
	public:
		ZZQRule1();
		virtual ~ZZQRule1();
		YYCC_DELETE_COPY_MOVE(ZZQRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

}
