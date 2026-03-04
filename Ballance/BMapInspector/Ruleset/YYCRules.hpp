#pragma once
#include "../Rule.hpp"

namespace BMapInspector::Ruleset {

	/**
	 * @brief YYC12345 Rule 1
	 * @details
	 * The object grouped into "Phys_FloorRails" should only be rails, otherwise their meshes' UV will be smooth.
	 * Additionally, these smooth UV meshes will also affect those objects refering them.
	 */
	class YYCRule1 : public Rule::IRule {
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
	class YYCRule2 : public Rule::IRule {
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
	class YYCRule3 : public Rule::IRule {
	public:
		YYCRule3();
		virtual ~YYCRule3();
		YYCC_DELETE_COPY_MOVE(YYCRule3)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief YYC12345 Rule 4
	 * @details
	 * Check the video format for Ballance and user-defined texture respectively.
	 * Report if there is non-vanilla Ballance settings.
	 */
	class YYCRule4 : public Rule::IRule {
	public:
		YYCRule4();
		virtual ~YYCRule4();
		YYCC_DELETE_COPY_MOVE(YYCRule4)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief YYC12345 Rule 5
	 * @details
	 * Check the save options for Ballance and user-defined texture respectively
	 * for reducing map size and avoid ambiguity.
	 */
	class YYCRule5 : public Rule::IRule {
	public:
		YYCRule5();
		virtual ~YYCRule5();
		YYCC_DELETE_COPY_MOVE(YYCRule5)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

	/**
	 * @brief YYC12345 Rule 6
	 * @details
	 * Show info hint for the skip of progressbar when loading map.
	 */
	class YYCRule6 : public Rule::IRule {
	public:
		YYCRule6();
		virtual ~YYCRule6();
		YYCC_DELETE_COPY_MOVE(YYCRule6)

	public:
		std::u8string_view GetRuleName() const override;
		void Check(Reporter::Reporter& reporter, Map::Level& level) const override;
	};

}
