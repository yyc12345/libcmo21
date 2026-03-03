#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Ruleset {

	/**
	 * @brief SomeOne_001 Rule 1
	 * @details
	 * If there is a physicalized object without any mesh,
	 * itself and following objects will not be physicalized.
	 */
	class SOneRule1 : public Rule::IRule {
	public:
		SOneRule1();
		virtual ~SOneRule1();
		YYCC_DELETE_COPY_MOVE(SOneRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

} // namespace BMapInspector::Ruleset
