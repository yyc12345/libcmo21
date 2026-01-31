#pragma once
#include "Utils.hpp"
#include "Reporter.hpp"
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace BMapInspector::Ruleset {

	class RuleContext {
	public:
		RuleContext();
		~RuleContext();
		YYCC_DELETE_COPY(RuleContext)
		YYCC_DECL_MOVE(RuleContext)

	public:
		LibCmo::CK2::CKContext *GetCKContext();

	private:
		LibCmo::CK2::CKContext *m_Ctx;
	};

	class IRule {
	public:
		IRule();
		virtual ~IRule();
		YYCC_DELETE_COPY_MOVE(IRule)

	public:
		virtual std::u8string_view GetRuleName() const = 0;
		virtual void Check(Reporter::Reporter& reporter, RuleContext& ctx) const = 0;
	};

	class RuleCollection {
	public:
		RuleCollection();
		~RuleCollection();
		YYCC_DELETE_COPY_MOVE(RuleCollection)

	public:
		size_t GetRuleCount() const;
		const std::vector<IRule *> &GetRules() const;

	private:
		std::vector<IRule *> rules;
	};

} // namespace BMapInspector::Ruleset
