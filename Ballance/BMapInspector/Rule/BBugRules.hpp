#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {
    
    /**
     * @brief BBug Rule 1
	 * @details
	 * Using light in map is not suggested.
     */
    class BBugRule1 : public IRule {
	public:
		BBugRule1();
		virtual ~BBugRule1();
		YYCC_DELETE_COPY_MOVE(BBugRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

}
