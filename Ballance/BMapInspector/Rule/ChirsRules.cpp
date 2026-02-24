#include "ChirsRules.hpp"
#include "Shared.hpp"

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Rule {

	// Reference: https://tieba.baidu.com/p/5913556704

#pragma region Chirs Rule 1

	constexpr char8_t CHIRS1[] = u8"CHIRS1";

	ChirsRule1::ChirsRule1() : IRule() {}

	ChirsRule1::~ChirsRule1() {}

	std::u8string_view ChirsRule1::GetRuleName() const {
		return CHIRS1;
	}
	void ChirsRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		constexpr char8_t MTL_NAME[] = u8"Laterne_Verlauf";
		auto* ctx = level.GetCKContext();

		// Fetch Laterne_Verlauf first
		auto* latern = Shared::FetchMaterial(ctx, MTL_NAME);
		if (latern == nullptr) return;

		// Report warning if this material's texture is not Laterne_Verlauf.tga
		auto* latern_tex = latern->GetTexture();
		if (latern_tex == nullptr) {
			reporter.WriteWarning(CHIRS1,
			                      u8R"(Find a material named "Laterne_Verlauf" but it doesn't have associated texture. )"
			                      u8R"(It occupies the magic material "Laterne_Verlauf" which affect the ray of latern in game. Please confirm this is your intention.)");
		} else {
			if (!Shared::CheckTextureFileName(latern_tex, Shared::TextureNames::LATERNE_VERLAUF)) {
				reporter.WriteWarning(CHIRS1,
				                      u8R"(Find a material named "Laterne_Verlauf" but its texture is not "Laterne_Verlauf.tga". )"
				                      u8R"(It occupies the magic material "Laterne_Verlauf" which affect the ray of latern in game. Please confirm this is your intention.)");
			}
		}

		// Report warning if there is multiple Laterne_Verlauf material.
		auto next_latern = ctx->GetObjectByNameAndClass(MTL_NAME, C::CK_CLASSID::CKCID_MATERIAL, latern);
		if (next_latern != nullptr) {
			reporter.WriteWarning(
			    CHIRS1,
			    u8R"(There are multiple materials named "Laterne_Verlauf". This will cause the disappearance of some latern's rays.)");
		}

		// Report warning if some materials' texture is Laterne_Verlauf,
		// but its name is not Laterne_Verlauf.
		for (auto* other_mtl : level.GetMaterials()) {
			if (C::CKStrEqual(other_mtl->GetName(), MTL_NAME)) continue;

			auto other_mtl_tex = other_mtl->GetTexture();
			if (other_mtl_tex == nullptr) continue;

			if (Shared::CheckTextureFileName(other_mtl_tex, Shared::TextureNames::LATERNE_VERLAUF)) {
				reporter.FormatWarning(
				    CHIRS1,
				    u8R"(Find material %s referring texture "Laterne_Verlauf.tga", but its name is not "Laterne_Verlauf". )"
				    u8R"(Please confirm the usage of this material. If it is used as "Laterne_Verlauf", please rename it into "Laterne_Verlauf" to have correct latern ray.)",
				    Shared::QuoteObjectName(other_mtl).c_str());
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
