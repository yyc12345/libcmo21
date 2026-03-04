#include "Rule.hpp"

#include "Ruleset/GpRules.hpp"
#include "Ruleset/ChirsRules.hpp"
#include "Ruleset/YYCRules.hpp"
#include "Ruleset/BBugRules.hpp"
#include "Ruleset/ZZQRules.hpp"
#include "Ruleset/SOneRules.hpp"
#include "Ruleset/SSBRules.hpp"
#include "Ruleset/LXRules.hpp"

namespace BMapInspector::Rule {

#pragma region IRule

	IRule::IRule() {}

	IRule::~IRule() {}

#pragma endregion

#pragma region RuleCollection

	RuleCollection::RuleCollection() : rules() {
		// Add rule into list.
		rules.emplace_back(new Ruleset::GpRule1());
		rules.emplace_back(new Ruleset::GpRule2());
		rules.emplace_back(new Ruleset::ChirsRule1());
		rules.emplace_back(new Ruleset::YYCRule1());
		rules.emplace_back(new Ruleset::YYCRule2());
		rules.emplace_back(new Ruleset::YYCRule3());
		rules.emplace_back(new Ruleset::YYCRule4());
		rules.emplace_back(new Ruleset::YYCRule5());
		rules.emplace_back(new Ruleset::YYCRule6());
		rules.emplace_back(new Ruleset::BBugRule1());
		rules.emplace_back(new Ruleset::BBugRule2());
		rules.emplace_back(new Ruleset::BBugRule3());
		rules.emplace_back(new Ruleset::ZZQRule1());
		rules.emplace_back(new Ruleset::ZZQRule2());
		rules.emplace_back(new Ruleset::ZZQRule3());
		rules.emplace_back(new Ruleset::SOneRule1());
		rules.emplace_back(new Ruleset::SSBRule1());
		rules.emplace_back(new Ruleset::LXRule1());
		// Add more rules...
	}

	RuleCollection::~RuleCollection() {
		// Free rule from list.
		for (const auto* rule : this->rules) {
			delete rule;
		}
	}

	size_t RuleCollection::GetRuleCount() const {
		return this->rules.size();
	}

	const std::vector<IRule*>& RuleCollection::GetRules() const {
		return this->rules;
	}

#pragma endregion

} // namespace BMapInspector::Rule
