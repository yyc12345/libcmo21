#include "SSBRules.hpp"
#include "Shared.hpp"

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace V = LibCmo::VxMath;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Rule {
#pragma region SSB Rule 1

	constexpr char8_t SSB1[] = u8"SSB1";
	constexpr L::CKFLOAT TOLERANCE = 0.001f;

	SSBRule1::SSBRule1() : IRule() {}

	SSBRule1::~SSBRule1() {}

	std::u8string_view SSBRule1::GetRuleName() const {
		return SSB1;
	}

	void SSBRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		auto physicalized_3dobjects = Shared::FetchPhysicalized3dObjects(ctx);

		// Iterate all physicalized 3dobject
		for (auto* physicalized_3dobject : physicalized_3dobjects) {
			// Get its world matrix
			const auto& matrix = physicalized_3dobject->GetWorldMatrix();

			// YYC MARK:
			// Following method is the bad way to check scale factor,
			// because it rely on some premise.
			// But it is simple, especially we do not have fully implement VxMatrix,
			// or have any linear algebra library.
			
			// Extract 3 columns
			V::VxVector3 col1(matrix[0][0], matrix[1][0], matrix[2][0]);
			V::VxVector3 col2(matrix[0][1], matrix[1][1], matrix[2][1]);
			V::VxVector3 col3(matrix[0][2], matrix[1][2], matrix[2][2]);
			// Compute their length, then check their value with tolerance.
			bool has_scale = false;
			if (!Shared::FPEqual(col1.Length(), 1.0f, TOLERANCE)) has_scale = true;
			if (!Shared::FPEqual(col2.Length(), 1.0f, TOLERANCE)) has_scale = true;
			if (!Shared::FPEqual(col3.Length(), 1.0f, TOLERANCE)) has_scale = true;
			// If it has scale factor, report error
			if (has_scale) {
				reporter.FormatError(
				    SSB1,
				    u8R"(Object %s grouped into physicalization groups has scale factor. This will cause its collision shape is different with its render shape.)",
				    Shared::QuoteObjectName(physicalized_3dobject).c_str());
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
