#include "Rule.hpp"

#include "Rule/GpRules.hpp"
#include "Rule/ChirsRules.hpp"
#include "Rule/YYCRules.hpp"
#include "Rule/BBugRules.hpp"
#include "Rule/ZZQRules.hpp"
#include "Rule/SOneRules.hpp"
#include "Rule/SSBRules.hpp"
#include "Rule/LXRules.hpp"

namespace BMapInspector::Rule {

#pragma region IRule

	IRule::IRule() {}

	IRule::~IRule() {}

#pragma endregion

#pragma region Ruleset

	Ruleset::Ruleset() : rules() {
		// Add rule into list.
		rules.emplace_back(new GpRule1());
		rules.emplace_back(new GpRule2());
		rules.emplace_back(new ChirsRule1());
		rules.emplace_back(new YYCRule1());
		rules.emplace_back(new YYCRule2());
		rules.emplace_back(new YYCRule3());
		rules.emplace_back(new YYCRule4());		
		rules.emplace_back(new BBugRule1());
		rules.emplace_back(new BBugRule2());
		rules.emplace_back(new BBugRule3());
		rules.emplace_back(new ZZQRule1());
		rules.emplace_back(new ZZQRule2());
		rules.emplace_back(new ZZQRule3());
		rules.emplace_back(new SOneRule1());
		rules.emplace_back(new SSBRule1());
		rules.emplace_back(new LXRule1());
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
