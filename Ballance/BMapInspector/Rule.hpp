#pragma once
#include "Utils.hpp"
#include "Reporter.hpp"
#include "Map.hpp"
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace BMapInspector::Rule {

	class IRule {
	public:
		IRule();
		virtual ~IRule();
		YYCC_DELETE_COPY_MOVE(IRule)

	public:
		virtual std::u8string_view GetRuleName() const = 0;
		virtual void Check(Reporter::Reporter& reporter, Map::Level& ctx) const = 0;
	};

	class Ruleset {
	public:
		Ruleset();
		~Ruleset();
		YYCC_DELETE_COPY_MOVE(Ruleset)

	public:
		size_t GetRuleCount() const;
		const std::vector<IRule *> &GetRules() const;

	private:
		std::vector<IRule *> rules;
	};

} // namespace BMapInspector::Ruleset
