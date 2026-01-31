#include "Ruleset.hpp"

namespace BMapInspector::Ruleset {

#pragma region Rule Context

	RuleContext::RuleContext() : m_Ctx(nullptr) {}

	RuleContext::~RuleContext() {}

	LibCmo::CK2::CKContext* RuleContext::GetCKContext() {
		return this->m_Ctx;
	}

	YYCC_IMPL_MOVE_CTOR(RuleContext, rhs) : m_Ctx(rhs.m_Ctx) {
		rhs.m_Ctx = nullptr;
	}

	YYCC_IMPL_MOVE_OPER(RuleContext, rhs) {
		this->m_Ctx = rhs.m_Ctx;
		rhs.m_Ctx = nullptr;
		return *this;
	}

#pragma endregion

#pragma region IRule

	IRule::IRule() {}

	IRule::~IRule() {}

#pragma endregion

#pragma region Rule Collection

	RuleCollection::RuleCollection() : rules() {
		// TODO: create instance for each rules.
	}

	RuleCollection::~RuleCollection() {}

	size_t RuleCollection::GetRuleCount() const {
		return this->rules.size();
	}

	const std::vector<IRule*>& RuleCollection::GetRules() const {
		return this->rules;
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
