#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {
    
	// Reference: https://tieba.baidu.com/p/3182981807

    /**
     * @brief Gamepiaynmo Rule 1
	 * @details
	 * The most comprehensive group checker inspired from Ballance Blender Plugin.
     */
    class Gp1Rule : public IRule {
	public:
		Gp1Rule();
		virtual ~Gp1Rule();
		YYCC_DELETE_COPY_MOVE(Gp1Rule)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& ctx) const override;
	};

	/**
	 * @brief Gamepiaynmo Rule 2
	 * @details
	 * This rule make sure that one Ballance element must be grouped into only one sector group.
	 * Multiple grouping and none grouping will throw error.
	 */
	class Gp2Rule : public IRule {
	public:
		Gp2Rule();
		virtual ~Gp2Rule();
		YYCC_DELETE_COPY_MOVE(Gp2Rule)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& ctx) const override;
	};

	/**
	 * @brief Gamepiaynmo Rule 3
	 * @details
	 * This rule make sure that all Ballance element is grouped into correct element group.
	 * This rule will check the mesh of PH and guess which element it is.
	 */
	class Gp3Rule : public IRule {
	public:
		Gp3Rule();
		virtual ~Gp3Rule();
		YYCC_DELETE_COPY_MOVE(Gp3Rule)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& ctx) const override;
	};

}
