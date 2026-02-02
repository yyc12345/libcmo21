#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {

	// Reference: https://tieba.baidu.com/p/5913556704

	/**
	 * @brief Chirs241097 Rule 1
	 * @details
	 * This rule will make sure that there is only 1 texture named Laterne_Verlauf in map,
	 * which represent the ray of latern.
	 */
	class Chirs1Rule : public IRule {
	public:
		Chirs1Rule();
		virtual ~Chirs1Rule();
		YYCC_DELETE_COPY_MOVE(Chirs1Rule)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& ctx) const override;
	};
}
