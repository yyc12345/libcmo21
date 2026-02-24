#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {

	/**
	 * @brief chirs241097 Rule 1
	 * @details
	 * This rule will make sure that there is only 1 texture named Laterne_Verlauf in map,
	 * which represent the ray of latern.
	 */
	class ChirsRule1 : public IRule {
	public:
		ChirsRule1();
		virtual ~ChirsRule1();
		YYCC_DELETE_COPY_MOVE(ChirsRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};
}
