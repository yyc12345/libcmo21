#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Rule {

	/**
	 * @brief YYC12345 Rule 1
	 * @details
	 * The object grouped into "Phys_FloorRails" should only be rails, otherwise their meshes' UV will be smooth.
	 * Additionally, these smooth UV meshes will also affect those objects refering them.
	 */
	class YYCRule1 : public IRule {
	public:
		YYCRule1();
		virtual ~YYCRule1();
		YYCC_DELETE_COPY_MOVE(YYCRule1)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief YYC12345 Rule 2
	 * @details
	 * The object grouped into physicalization group should not have isolated vertex,
	 * otherwise it will fail to be physicalized.
	 */
	class YYCRule2 : public IRule {
	public:
		YYCRule2();
		virtual ~YYCRule2();
		YYCC_DELETE_COPY_MOVE(YYCRule2)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief YYC12345 Rule 3
	 * @details
	 * Exactly same mesh, material and texture can be merged.
	 */
	class YYCRule3 : public IRule {
	public:
		YYCRule3();
		virtual ~YYCRule3();
		YYCC_DELETE_COPY_MOVE(YYCRule3)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

}
