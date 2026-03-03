#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Ruleset {
    
    /**
     * @brief BBug Rule 1
	 * @details
	 * Using light in map is not suggested.
     */
    class BBugRule1 : public Rule::IRule {
	public:
		BBugRule1();
		virtual ~BBugRule1();
		YYCC_DELETE_COPY_MOVE(BBugRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief BBug Rule 2
	 * @details
	 * Using camera in map is not suggested.
	 */
	class BBugRule2 : public Rule::IRule {
	public:
		BBugRule2();
		virtual ~BBugRule2();
		YYCC_DELETE_COPY_MOVE(BBugRule2)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief BBug Rule 3
	 * @details
	 * Check whether the parameters of all materials is same with Ballance vanilla settings.
	 */
	class BBugRule3 : public Rule::IRule {
	public:
		BBugRule3();
		virtual ~BBugRule3();
		YYCC_DELETE_COPY_MOVE(BBugRule3)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

}
