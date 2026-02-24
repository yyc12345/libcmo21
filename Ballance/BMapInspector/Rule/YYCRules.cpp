#include "YYCRules.hpp"
#include "Shared.hpp"
#include <vector>
#include <set>
#include <algorithm>

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Rule {

#pragma region YYC Rule 1

	constexpr char8_t YYC1[] = u8"YYC1";

	YYCRule1::YYCRule1() : IRule() {}

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
		auto group_3dobjects = Shared::Iter3dObjects(phys_floorrails);
		for (auto* group_3dobject : group_3dobjects) {
			// Then we iterate their current meshes
			auto* mesh = group_3dobject->GetCurrentMesh();
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
					    u8R"(Object %s is grouped into Phys_FloorRails, but its texture %s (referred by mesh %s and material %s) seems not the rail texture. This will cause some parts of this object be smooth unexpectly.)",
					    Shared::QuoteObjectName(group_3dobject).c_str(),
					    Shared::QuoteObjectName(texture).c_str(),
					    Shared::QuoteObjectName(mesh).c_str(),
					    Shared::QuoteObjectName(mtl).c_str());
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
				    u8R"(Object %s is not grouped into Phys_FloorRails, but some objects grouped into Phys_FloorRails refer its mesh %s. This will cause this object be smooth unexpectly.)",
				    Shared::QuoteObjectName(obj).c_str(),
				    Shared::QuoteObjectName(mesh).c_str());
			}
		}
	}

#pragma endregion

#pragma region YYC Rule 2

	constexpr char8_t YYC2[] = u8"YYC2";

	YYCRule2::YYCRule2() : IRule() {}

	YYCRule2::~YYCRule2() {}

	std::u8string_view YYCRule2::GetRuleName() const {
		return YYC2;
	}

	void YYCRule2::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		auto physicalized_3dobjects = Shared::FetchPhysicalized3dObjects(ctx);

		// Iterate all physicalized 3dobject
		for (auto* physicalized_3dobject : physicalized_3dobjects) {
			// Get its mesh
			auto* mesh = physicalized_3dobject->GetCurrentMesh();
			if (mesh == nullptr) continue;

			// Create a bool vector with vertex count and false init value.
			std::vector<bool> used_vertex(mesh->GetVertexCount(), false);
			// Iterate all face and set their vertex as used.
			auto* face_indices = mesh->GetFaceIndices();
			for (L::CKDWORD face_idx = 0; face_idx < mesh->GetFaceCount(); ++face_idx) {
				used_vertex[face_indices[face_idx * 3]] = true;
				used_vertex[face_indices[face_idx * 3 + 1]] = true;
				used_vertex[face_indices[face_idx * 3 + 2]] = true;
			}
			// Check whether there is unused vertex
			auto has_unused_vertex = std::any_of(used_vertex.begin(), used_vertex.end(), [](bool v) { return v == false; });
			// If there is unused vertex, report error
			if (has_unused_vertex) {
				reporter.FormatError(
				    YYC2,
				    u8R"(Object %s is grouped into physicalization groups, and its referred mesh %s has isolated vertex. This will cause it can not be physicalized.)",
				    Shared::QuoteObjectName(physicalized_3dobject).c_str(),
				    Shared::QuoteObjectName(mesh).c_str());
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
