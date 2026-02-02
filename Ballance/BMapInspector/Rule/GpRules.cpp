#include "GpRules.hpp"

namespace BMapInspector::Rule {

#pragma region GP1 Rule

	Gp1Rule::Gp1Rule() : IRule() {}

	Gp1Rule::~Gp1Rule() {}

	std::u8string_view Gp1Rule::GetRuleName() const {
		return u8"GP1";
	}

	void Gp1Rule::Check(Reporter::Reporter& reporter, Map::Level& ctx) const {}

#pragma endregion

#pragma region GP2 Rule

	Gp2Rule::Gp2Rule() : IRule() {}

	Gp2Rule::~Gp2Rule() {}

	std::u8string_view Gp2Rule::GetRuleName() const {
		return u8"GP2";
	}

	void Gp2Rule::Check(Reporter::Reporter& reporter, Map::Level& ctx) const {}

#pragma endregion

#pragma region GP3 Rule

	Gp3Rule::Gp3Rule() : IRule() {}

	Gp3Rule::~Gp3Rule() {}

	std::u8string_view Gp3Rule::GetRuleName() const {
		return u8"GP3";
	}

	void Gp3Rule::Check(Reporter::Reporter& reporter, Map::Level& ctx) const {
		// TODO: Mesh hash is not implemented.
	}

#pragma endregion

} // namespace BMapInspector::Rule
