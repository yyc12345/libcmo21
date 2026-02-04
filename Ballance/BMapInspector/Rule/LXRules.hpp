#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {

	/**
	 * @brief LengXi Rule 1
	 * @details
	 * All meshes, materials and textures used by Ballance elements should not be used by any other objects.
	 */
	class LXRule1 : public IRule {
	public:
		LXRule1();
		virtual ~LXRule1();
		YYCC_DELETE_COPY_MOVE(LXRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

}
