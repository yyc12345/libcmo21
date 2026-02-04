#include "GpRules.hpp"

namespace BMapInspector::Rule {

#pragma region GP1 Rule

	GpRule1::GpRule1() : IRule() {}

	GpRule1::~GpRule1() {}

	std::u8string_view GpRule1::GetRuleName() const {
		return u8"GP1";
	}

	void GpRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {}

#pragma endregion

#pragma region GP2 Rule

	constexpr char8_t GP2[] = u8"GP2";

	GpRule2::GpRule2() : IRule() {}

	GpRule2::~GpRule2() {}

	std::u8string_view GpRule2::GetRuleName() const {
		return GP2;
	}

	void GpRule2::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		
	}

#pragma endregion

#pragma region GP3 Rule
//
//	Gp3Rule::Gp3Rule() : IRule() {}
//
//	Gp3Rule::~Gp3Rule() {}
//
//	std::u8string_view Gp3Rule::GetRuleName() const {
//		return u8"GP3";
//	}
//
//	void Gp3Rule::Check(Reporter::Reporter& reporter, Map::Level& level) const {
//		// TODO: Mesh hash is not implemented.
//	}
//
//#pragma endregion

} // namespace BMapInspector::Rule
