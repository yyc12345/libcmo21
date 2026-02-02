#include "ChirsRules.hpp"

namespace BMapInspector::Rule {
	Chirs1Rule::Chirs1Rule() : IRule() {}
	Chirs1Rule::~Chirs1Rule() {}
	std::u8string_view Chirs1Rule::GetRuleName() const {
		return u8"CHIRS1";
	}
	void Chirs1Rule::Check(Reporter::Reporter& reporter, Map::Level& ctx) const {
		// Report error if there is some material named Laterne_Verlauf 
		// but its texture is not pointed to Laterne_Verlauf texture.

		// Report error if some materials' texture is Laterne_Verlauf,
		// but its name is not Laterne_Verlauf.

		// Report error if there is multiple Laterne_Verlauf material.
		reporter.WriteError(this->GetRuleName(), u8"Fork you!");
	}
} // namespace BMapInspector::Rule
