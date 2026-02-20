#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {
    
	// Reference: https://tieba.baidu.com/p/3182981807

    /**
     * @brief Gamepiaynmo Rule 1
	 * @details
	 * The most comprehensive group checker inspired from Ballance Blender Plugin.
     */
    class GpRule1 : public IRule {
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
	class GpRule2 : public IRule {
	public:
		GpRule2();
		virtual ~GpRule2();
		YYCC_DELETE_COPY_MOVE(GpRule2)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	///**
	// * @brief Gamepiaynmo Rule 2
	// * @details
	// * Every Ballance group should not have any groups with same name.
	// */
	//class GpRule2 : public IRule {
	//public:
	//	GpRule2();
	//	virtual ~GpRule2();
	//	YYCC_DELETE_COPY_MOVE(GpRule2)

	//public:
	//	std::u8string_view GetRuleName() const override;
	//	void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	//};

	///**
	// * @brief Gamepiaynmo Rule 2
	// * @details
	// * This rule make sure that one Ballance element must be grouped into only one sector group.
	// * Multiple grouping and none grouping will throw error.
	// */
	///**
	// * @brief Gamepiaynmo Rule 3
	// * @details
	// * This rule make sure that all Ballance element is grouped into correct element group.
	// * This rule will check the mesh of PH and guess which element it is.
	// */
	//class Gp3Rule : public IRule {
	//public:
	//	Gp3Rule();
	//	virtual ~Gp3Rule();
	//	YYCC_DELETE_COPY_MOVE(Gp3Rule)

	//public:
	//	std::u8string_view GetRuleName() const override;
	//	void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	//};

}
