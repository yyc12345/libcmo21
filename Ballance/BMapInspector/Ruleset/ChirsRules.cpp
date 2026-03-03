#include "ChirsRules.hpp"
#include "Shared/Utility.hpp"
#include "Shared/Name.hpp"

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Ruleset {

#pragma region Chirs Rule 1

	// Reference: https://tieba.baidu.com/p/5913556704

	ChirsRule1::ChirsRule1() : Rule::IRule() {}

	ChirsRule1::~ChirsRule1() {}

	std::u8string_view ChirsRule1::GetRuleName() const {
		return u8"CHIRS1";
	}

	void ChirsRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		constexpr char8_t MTL_LATERNE_VERLAUF[] = u8"Laterne_Verlauf";
		auto* ctx = level.GetCKContext();

		// Fetch Laterne_Verlauf first
		auto* latern = Shared::Utility::FetchMaterial(ctx, MTL_LATERNE_VERLAUF);
		if (latern == nullptr) return;

		// Report warning if this material's texture is not Laterne_Verlauf.tga
		auto* latern_tex = latern->GetTexture();
		if (latern_tex == nullptr) {
			reporter.FormatWarning(
			    u8"Find a material named %s but it doesn't have associated texture. "
			    u8"It occupies the magic material %s which affect the ray of latern in game. Please confirm this is your intention.",
			    Shared::Utility::QuoteText(MTL_LATERNE_VERLAUF).c_str(),
			    Shared::Utility::QuoteText(MTL_LATERNE_VERLAUF).c_str());
		} else {
			if (!Shared::Utility::CheckTextureFileName(latern_tex, Shared::Name::Texture::LATERNE_VERLAUF)) {
				reporter.FormatWarning(
				    u8"Find a material named %s but its texture is not %s. "
				    u8"It occupies the magic material %s which affect the ray of latern in game. Please confirm this is your intention.",
				    Shared::Utility::QuoteText(MTL_LATERNE_VERLAUF).c_str(),
				    Shared::Utility::QuoteText(Shared::Name::Texture::LATERNE_VERLAUF).c_str(),
				    Shared::Utility::QuoteText(MTL_LATERNE_VERLAUF).c_str());
			}
		}

		// Report warning if there is multiple Laterne_Verlauf material.
		auto next_latern = ctx->GetObjectByNameAndClass(MTL_LATERNE_VERLAUF, C::CK_CLASSID::CKCID_MATERIAL, latern);
		if (next_latern != nullptr) {
			reporter.FormatWarning(u8"There are multiple materials named %s. This will cause the disappearance of some latern's rays.",
			                       Shared::Utility::QuoteText(MTL_LATERNE_VERLAUF).c_str());
		}

		// Report warning if some materials' texture is Laterne_Verlauf,
		// but its name is not Laterne_Verlauf.
		for (auto* other_mtl : level.GetMaterials()) {
			if (C::CKStrEqual(other_mtl->GetName(), MTL_LATERNE_VERLAUF)) continue;

			auto other_mtl_tex = other_mtl->GetTexture();
			if (other_mtl_tex == nullptr) continue;

			if (Shared::Utility::CheckTextureFileName(other_mtl_tex, Shared::Name::Texture::LATERNE_VERLAUF)) {
				reporter.FormatWarning(
				    u8"Find material %s referring texture %s, but its name is not %s. "
				    u8"Please confirm the usage of this material. If it is used as %s, please rename it into %s to have correct latern ray.",
				    Shared::Utility::QuoteObjectName(other_mtl).c_str(),
				    Shared::Utility::QuoteText(Shared::Name::Texture::LATERNE_VERLAUF).c_str(),
				    Shared::Utility::QuoteText(MTL_LATERNE_VERLAUF).c_str(),
				    Shared::Utility::QuoteText(MTL_LATERNE_VERLAUF).c_str(),
				    Shared::Utility::QuoteText(MTL_LATERNE_VERLAUF).c_str());
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
