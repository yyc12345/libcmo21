#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {

	/**
	 * @brief speedystoneball Rule 1
	 * @details
	 * Pjysicalized object should not have scale factor, especially negative scale factor (mirror).
	 */
	class SSBRule1 : public IRule {
	public:
		SSBRule1();
		virtual ~SSBRule1();
		YYCC_DELETE_COPY_MOVE(SSBRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

} // namespace BMapInspector::Rule
