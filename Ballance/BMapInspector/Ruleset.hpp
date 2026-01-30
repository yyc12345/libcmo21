#pragma once
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace BMapInspector::Ruleset {

	class IRule {
	public:
		IRule();
		virtual ~IRule();
		YYCC_DELETE_COPY_MOVE(IRule)

	public:
		virtual std::u8string_view GetRuleName() const = 0;
		virtual void Check() const = 0;
	};

	class RuleCollection {
	public:
		RuleCollection();
		~RuleCollection();
		YYCC_DELETE_COPY_MOVE(RuleCollection)

	public:
		const std::vector<IRule *> &GetRules() const;

	private:
		std::vector<IRule *> rules;
	};

} // namespace BMapInspector::Ruleset
