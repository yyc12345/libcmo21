#include "Rule.hpp"

namespace BMapInspector::Rule {

#pragma region IRule

	IRule::IRule() {}

	IRule::~IRule() {}

#pragma endregion

#pragma region Ruleset

	Ruleset::Ruleset() : rules() {
		// TODO: create instance for each rules.
	}

	Ruleset::~Ruleset() {}

	size_t Ruleset::GetRuleCount() const {
		return this->rules.size();
	}

	const std::vector<IRule*>& Ruleset::GetRules() const {
		return this->rules;
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
