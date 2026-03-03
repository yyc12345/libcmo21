#pragma once
#include "Utils.hpp"
#include "Reporter.hpp"
#include "Map.hpp"
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace BMapInspector::Rule {

	/**
	 * @brief The interface of a rule.
	 */
	class IRule {
	public:
		IRule();
		virtual ~IRule();
		YYCC_DELETE_COPY_MOVE(IRule)

	public:
		virtual std::u8string_view GetRuleName() const = 0;
		virtual void Check(Reporter::Reporter& reporter, Map::Level& level) const = 0;
	};

	/**
	 * @brief A collection of rules.
	 */
	class RuleCollection {
	public:
		RuleCollection();
		~RuleCollection();
		YYCC_DELETE_COPY_MOVE(RuleCollection)

	public:
		size_t GetRuleCount() const;
		const std::vector<IRule*>& GetRules() const;

	private:
		std::vector<IRule*> rules;
	};

} // namespace BMapInspector::Rule
