#include "YYCRules.hpp"
#include "Shared.hpp"
#include <set>

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Rule {

#pragma region YYC Rule 1

	constexpr char8_t YYC1[] = u8"YYC1";

	YYCRule1::YYCRule1() {}

	YYCRule1::~YYCRule1() {}

	std::u8string_view YYCRule1::GetRuleName() const {
		return YYC1;
	}

	void YYCRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();

		// We get "Phys_FloorRails" group first.
		auto* phys_floorrails = Shared::FetchGroup(ctx, Shared::GroupNames::PHYS_FLOORRAILS);
		if (phys_floorrails == nullptr) return;

		// Create container holding smooth meshes
		std::set<O::CKMesh*> smooth_meshes;
		// We iterate all object grouped into it.
		auto group_3dentities = Shared::Iter3dEntities(phys_floorrails);
		for (auto* group_3dentity : group_3dentities) {
			// Then we iterate their current meshes
			auto* mesh = group_3dentity->GetCurrentMesh();
			if (mesh == nullptr) continue;

			// Iterate all meshes
			auto mtls = Shared::IterMaterial(mesh);
			for (auto* mtl : mtls) {
				// Check whether all texture referred by this mesh are "Rail_Environment".
				auto texture = mtl->GetTexture();
				if (texture == nullptr) continue;
				if (!Shared::CheckTextureFileName(texture, Shared::TextureNames::RAIL_ENVIRONMENT)) {
					// No, this is not rail texture, throw error.
					reporter.FormatError(
					    YYC1,
					    u8R"(Object "%s" is grouped into Phys_FloorRails, but its texture "%s" (referred by mesh %s and material %s) seems not the rail texture. This will cause this object be smooth unexpectly.)",
					    Shared::RenderObjectName(group_3dentity),
					    Shared::RenderObjectName(texture),
					    Shared::RenderObjectName(mesh),
					    Shared::RenderObjectName(mtl));
				}
			}

			// Record this mesh into set.
			smooth_meshes.emplace(mesh);
		}

		// Now we make sure that these smooth mesh is not referred by any other object.
		// We iterate all 3d object first
		auto all_3dobject = level.Get3dObjects();
		for (auto* obj : all_3dobject) {
			// Then we get its current mesh
			auto* mesh = obj->GetCurrentMesh();
			if (mesh == nullptr) continue;

			// Check whether its mesh is in smooth mesh,
			// and itself is not in "Phys_FloorRails" group
			if (!obj->IsInGroup(phys_floorrails) && smooth_meshes.contains(mesh)) {
				// Report error.
				reporter.FormatError(
				    YYC1,
				    u8R"(Object "%s" is not grouped into Phys_FloorRails, but some objects grouped into Phys_FloorRails refer its mesh "%s". This will cause this object be smooth unexpectly.)",
				    Shared::RenderObjectName(obj),
				    Shared::RenderObjectName(mesh));
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
