#include "LXRules.hpp"
#include "Shared.hpp"
#include <set>

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Rule {

#pragma region LX Rule 1

	constexpr char8_t LX1[] = u8"LX1";

	LXRule1::LXRule1() : IRule() {}

	LXRule1::~LXRule1() {}

	std::u8string_view LXRule1::GetRuleName() const {
		return LX1;
	}

	void LXRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();

		// First we fetch all Ballance element and push them into set.
		std::set<O::CK3dObject*> elements;
		for (auto* group_name : Shared::GroupNames::ALL_PH) {
			auto* group = Shared::FetchGroup(ctx, group_name);
			if (group == nullptr) continue;
			auto group_objects = Shared::Iter3dObjects(group);
			for (auto* group_object : group_objects) {
				elements.emplace(group_object);
			}
		}

		// Then we analyse their mesh, material, texture and fill corresponding set.
		std::set<O::CKMesh*> element_meshes;
		std::set<O::CKMaterial*> element_materials;
		std::set<O::CKTexture*> element_textures;
		for (auto* element_object : elements) {
			auto* mesh = element_object->GetCurrentMesh();
			if (mesh == nullptr) continue;

			// Add into mesh set
			auto mesh_insert_rv = element_meshes.emplace(mesh);

			// Only process it if we have inserted it
			// because we do not want to duplicatedly process it.
			if (mesh_insert_rv.second) {
				// Iterate all meshes
				auto mtls = Shared::IterMaterial(mesh);
				for (auto* mtl : mtls) {
					// Add into material set
					auto mtl_insert_rv = element_materials.emplace(mtl);

					// Also only process it if we have inserted it
					if (mtl_insert_rv.second) {
						// Fetch texture
						auto texture = mtl->GetTexture();
						if (texture == nullptr) continue;
						// And insert it
						element_textures.emplace(texture);
					}
				}
			}
		}

		// Now, check any other object whether use these data.
		for (auto* other_object : level.Get3dObjects()) {
			// If it is element, skip it.
			if (elements.contains(other_object)) continue;

			// Get mesh
			auto* mesh = other_object->GetCurrentMesh();
			if (mesh == nullptr) continue;
			// And check mesh
			if (element_meshes.contains(mesh)) {
				reporter.FormatError(
				    LX1,
				    u8R"(Object %s used mesh %s is already used by a Ballance element. This will cause this object can not be rendered correctly in level.)",
				    Shared::QuoteObjectName(other_object).c_str(),
				    Shared::QuoteObjectName(mesh).c_str());
			} else {
				// If not, check material.
				// Iterate all meshes
				auto mtls = Shared::IterMaterial(mesh);
				for (auto* mtl : mtls) {
					if (element_materials.contains(mtl)) {
						reporter.FormatError(
						    LX1,
						    u8R"(Object %s used material %s (referred by mesh %s) is already used by a Ballance element. This will cause this object can not be rendered correctly in level.)",
						    Shared::QuoteObjectName(other_object).c_str(),
						    Shared::QuoteObjectName(mtl).c_str(),
						    Shared::QuoteObjectName(mesh).c_str());
					} else {
						// Still not, check texture.
						// Fetch texture
						auto texture = mtl->GetTexture();
						if (texture == nullptr) continue;
						// And check it
						if (element_textures.contains(texture)) {
							reporter.FormatError(
							    LX1,
							    u8R"(Object %s used texture %s (referred by mesh "%s" and material "%s") is already used by a Ballance element. This will cause this object can not be rendered correctly in level.)",
							    Shared::QuoteObjectName(other_object).c_str(),
							    Shared::QuoteObjectName(texture).c_str(),
							    Shared::QuoteObjectName(mesh).c_str(),
							    Shared::QuoteObjectName(mtl).c_str());
						}
					}
				}
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
