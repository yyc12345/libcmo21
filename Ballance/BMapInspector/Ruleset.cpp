#include "Ruleset.hpp"

namespace BMapInspector::Ruleset {

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
