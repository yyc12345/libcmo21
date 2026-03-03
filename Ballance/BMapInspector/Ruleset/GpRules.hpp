#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Ruleset {
    
    /**
     * @brief Gamepiaynmo Rule 1
	 * @details
	 * The most comprehensive group checker inspired from Ballance Blender Plugin.
     */
    class GpRule1 : public Rule::IRule {
	public:
		GpRule1();
		virtual ~GpRule1();
		YYCC_DELETE_COPY_MOVE(GpRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief Gamepiaynmo Rule 2
	 * @details
	 * This rule will:
	 * \li Show how many sector located in given map.
	 * \li Check whether there is sector group.
	 * \li Check whether use intuitive sector name for sector 9.
	 * \li Warn for sector count is equal to 1. It will cause mosaic issue on the flames of checkpoint.
	 * \li Warn for sector count greater than 8. It will cause vanilla game freezed without 999 sector loader.
	 * \li Check whether sector group is successive.
	 */
	class GpRule2 : public Rule::IRule {
	public:
		GpRule2();
		virtual ~GpRule2();
		YYCC_DELETE_COPY_MOVE(GpRule2)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

}
