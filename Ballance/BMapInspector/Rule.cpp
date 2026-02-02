#include "Rule.hpp"

#include "Rule/GpRules.hpp"
#include "Rule/ChirsRules.hpp"
#include "Rule/YYCRules.hpp"
#include "Rule/BBugRules.hpp"
#include "Rule/ZZQRules.hpp"

namespace BMapInspector::Rule {

#pragma region IRule

	IRule::IRule() {}

	IRule::~IRule() {}

#pragma endregion

#pragma region Ruleset

	Ruleset::Ruleset() : rules() {
		// Add rule into list.
		rules.emplace_back(new Gp1Rule());
		rules.emplace_back(new Gp2Rule());
		rules.emplace_back(new Gp3Rule());
		rules.emplace_back(new Chirs1Rule());
		// Add more rules...
	}

	Ruleset::~Ruleset() {
		// Free rule from list.
		for (const auto* rule : this->rules) {
			delete rule;
		}
	}

	size_t Ruleset::GetRuleCount() const {
		return this->rules.size();
	}

	const std::vector<IRule*>& Ruleset::GetRules() const {
		return this->rules;
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
