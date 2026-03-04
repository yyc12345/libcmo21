#include "YYCRules.hpp"
#include "Shared/Utility.hpp"
#include "Shared/Name.hpp"
#include "Shared/DupCmp.hpp"
#include <yycc.hpp>
#include <yycc/string/op.hpp>
#include <vector>
#include <set>
#include <optional>
#include <algorithm>
#include <unordered_set>

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace V = LibCmo::VxMath;
namespace O = LibCmo::CK2::ObjImpls;
namespace strop = yycc::string::op;

namespace BMapInspector::Ruleset {

#pragma region YYC Rule 1

	YYCRule1::YYCRule1() : Rule::IRule() {}

	YYCRule1::~YYCRule1() {}

	std::u8string_view YYCRule1::GetRuleName() const {
		return u8"YYC1";
	}

	void YYCRule1::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();

		// We get "Phys_FloorRails" group first.
		auto* phys_floorrails = Shared::Utility::FetchGroup(ctx, Shared::Name::Group::PHYS_FLOORRAILS);
		if (phys_floorrails == nullptr) return;

		// Create container holding smooth meshes
		std::set<O::CKMesh*> smooth_meshes;
		// We iterate all object grouped into it.
		auto group_3dobjects = Shared::Utility::Iter3dObjects(phys_floorrails);
		for (auto* group_3dobject : group_3dobjects) {
			// Then we iterate their current meshes
			auto* mesh = group_3dobject->GetCurrentMesh();
			if (mesh == nullptr) continue;

			// Iterate all meshes
			auto mtls = Shared::Utility::IterMaterial(mesh);
			for (auto* mtl : mtls) {
				// Check whether all texture referred by this mesh are "Rail_Environment".
				auto texture = mtl->GetTexture();
				if (texture == nullptr) continue;
				if (!Shared::Utility::CheckTextureFileName(texture, Shared::Name::Texture::RAIL_ENVIRONMENT)) {
					// No, this is not rail texture, throw error.
					reporter.FormatError(
					    u8"Object %s is grouped into %s, but its texture %s (referred by mesh %s and material %s) seems not the rail texture. "
					    u8"This will cause some parts of this object be smooth unexpectly.",
					    Shared::Utility::QuoteObjectName(group_3dobject).c_str(),
					    Shared::Utility::QuoteText(Shared::Name::Group::PHYS_FLOORRAILS).c_str(),
					    Shared::Utility::QuoteObjectName(texture).c_str(),
					    Shared::Utility::QuoteObjectName(mesh).c_str(),
					    Shared::Utility::QuoteObjectName(mtl).c_str());
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
				reporter.FormatError(u8"Object %s is not grouped into %s, but some objects grouped into %s refer its mesh %s. "
				                     u8"This will cause this object be smooth unexpectly.",
				                     Shared::Utility::QuoteObjectName(obj).c_str(),
				                     Shared::Utility::QuoteText(Shared::Name::Group::PHYS_FLOORRAILS).c_str(),
				                     Shared::Utility::QuoteText(Shared::Name::Group::PHYS_FLOORRAILS).c_str(),
				                     Shared::Utility::QuoteObjectName(mesh).c_str());
			}
		}
	}

#pragma endregion

#pragma region YYC Rule 2

	YYCRule2::YYCRule2() : Rule::IRule() {}

	YYCRule2::~YYCRule2() {}

	std::u8string_view YYCRule2::GetRuleName() const {
		return u8"YYC2";
	}

	void YYCRule2::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		auto* ctx = level.GetCKContext();
		auto physicalized_3dobjects = Shared::Utility::FetchPhysicalized3dObjects(ctx);

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
				reporter.FormatError(u8"Object %s is grouped into physicalization groups, and its referred mesh %s has isolated vertex. "
				                     u8"This will cause it can not be physicalized.",
				                     Shared::Utility::QuoteObjectName(physicalized_3dobject).c_str(),
				                     Shared::Utility::QuoteObjectName(mesh).c_str());
			}
		}
	}

#pragma endregion

#pragma region YYC Rule 3

	YYCRule3::YYCRule3() : Rule::IRule() {}

	YYCRule3::~YYCRule3() {}

	std::u8string_view YYCRule3::GetRuleName() const {
		return u8"YYC3";
	}

	void YYCRule3::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		// Using utility structs
		using Shared::DupCmp::CKMaterialWrapper;
		using Shared::DupCmp::CKMaterialWrapperEqualTo;
		using Shared::DupCmp::CKMaterialWrapperHash;
		using Shared::DupCmp::CKMeshWrapper;
		using Shared::DupCmp::CKMeshWrapperEqualTo;
		using Shared::DupCmp::CKMeshWrapperHash;
		using Shared::DupCmp::CKTextureWrapper;
		using Shared::DupCmp::CKTextureWrapperEqualTo;
		using Shared::DupCmp::CKTextureWrapperHash;

		// Check textures
		std::unordered_multiset<CKTextureWrapper, CKTextureWrapperHash, CKTextureWrapperEqualTo> textures;
		for (auto* tex : level.GetTextures()) {
			textures.emplace(CKTextureWrapper(tex));
		}
		// Show result
		for (auto it = textures.begin(); it != textures.end();) {
			size_t count = textures.count(*it);

			// all count elements have equivalent keys
			if (count > 1) {
				std::vector<O::CKTexture*> dup_texs;
				for (size_t i = 0; i < count; ++i) {
					dup_texs.emplace_back(it->GetTexture());
					++it;
				}

				reporter.FormatInfo(u8"Some textures are visually identical. Please consider merging them to reduce the final map size. "
				                    u8"These textures are: %s.",
				                    Shared::Utility::QuoteObjectNames(dup_texs.begin(), dup_texs.end()).c_str());
			} else {
				++it;
			}
		}

		// Check materials
		std::unordered_multiset<CKMaterialWrapper, CKMaterialWrapperHash, CKMaterialWrapperEqualTo> materials;
		for (auto* mat : level.GetMaterials()) {
			materials.emplace(CKMaterialWrapper(mat));
		}
		// Show result
		for (auto it = materials.begin(); it != materials.end();) {
			size_t count = materials.count(*it);

			// all count elements have equivalent keys
			if (count > 1) {
				std::vector<O::CKMaterial*> dup_mtls;
				for (size_t i = 0; i < count; ++i) {
					dup_mtls.emplace_back(it->GetMaterial());
					++it;
				}

				reporter.FormatInfo(u8"Some materials are visually identical. Please consider merging them to reduce the final map size. "
				                    u8"These materials are: %s.",
				                    Shared::Utility::QuoteObjectNames(dup_mtls.begin(), dup_mtls.end()).c_str());
			} else {
				++it;
			}
		}

		// Check meshes
		std::unordered_multiset<CKMeshWrapper, CKMeshWrapperHash, CKMeshWrapperEqualTo> meshes;
		for (auto* mesh : level.GetMeshes()) {
			meshes.emplace(CKMeshWrapper(mesh));
		}
		// Show result
		for (auto it = meshes.begin(); it != meshes.end();) {
			size_t count = meshes.count(*it);

			// all count elements have equivalent keys
			if (count > 1) {
				std::vector<O::CKMesh*> dup_meshes;
				for (size_t i = 0; i < count; ++i) {
					dup_meshes.emplace_back(it->GetMesh());
					++it;
				}

				reporter.FormatInfo(u8"Some meshes are visually identical. Please consider merging them to reduce the final map size. "
				                    u8"These meshes are: %s.",
				                    Shared::Utility::QuoteObjectNames(dup_meshes.begin(), dup_meshes.end()).c_str());
			} else {
				++it;
			}
		}
	}

#pragma endregion

#pragma region YYC Rule 4

	YYCRule4::YYCRule4() : Rule::IRule() {}

	YYCRule4::~YYCRule4() {}

	std::u8string_view YYCRule4::GetRuleName() const {
		return u8"YYC4";
	}

	void YYCRule4::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		// Build lowercase texture name set first.
		std::set<std::u8string> opaque_texs;
		for (const auto* tex_name : Shared::Name::Texture::OPAQUE_TEXS) {
			opaque_texs.emplace(strop::to_lower(tex_name));
		}
		std::set<std::u8string> transparent_texs;
		for (const auto* tex_name : Shared::Name::Texture::TRANSPARENT_TEXS) {
			transparent_texs.emplace(strop::to_lower(tex_name));
		}

		// Check texture one by one
		for (auto& tex : level.GetTextures()) {
			auto tex_filename = Shared::Utility::ExtractTextureFileName(tex);
			if (!tex_filename.has_value()) continue;

			auto lower_tex_filename = strop::to_lower(tex_filename.value());
			if (opaque_texs.contains(lower_tex_filename)) {
				if (tex->GetVideoFormat() != V::VX_PIXELFORMAT::_16_ARGB1555) {
					reporter.FormatInfo(u8"Texture %s is Ballance opaque texture. But its video format is not ARGB1555. "
					                    u8"This is mismatched with vanilla Ballance.",
					                    Shared::Utility::QuoteObjectName(tex).c_str());
				}
			} else if (transparent_texs.contains(lower_tex_filename)) {
				if (tex->GetVideoFormat() != V::VX_PIXELFORMAT::_32_ARGB8888) {
					reporter.FormatInfo(u8"Texture %s is Ballance transparent texture. But its video format is not ARGB8888. "
					                    u8"This is mismatched with vanilla Ballance.",
					                    Shared::Utility::QuoteObjectName(tex).c_str());
				}
			} else {
				switch (tex->GetVideoFormat()) {
					case V::VX_PIXELFORMAT::_16_ARGB1555:
						// Do nothing.
						break;
					case V::VX_PIXELFORMAT::_32_ARGB8888:
						reporter.FormatInfo(u8"Texture %s is not Ballance texture. Its video format is ARGB8888. "
						                    u8"This may cause useless performance consumption if there is no transparent inside it. "
						                    u8"Please check whether this is essential.",
						                    Shared::Utility::QuoteObjectName(tex).c_str());
						break;
					default:
						reporter.FormatInfo(
						    u8"Texture %s is not Ballance texture. Its video format is not ARGB1555 or ARGB8888. "
						    u8"This is mismatched with vanilla Ballance. "
						    u8"Please set it to ARGB1555 for opaque texture, or ARGB8888 for transaprent texture, except special scenario.",
						    Shared::Utility::QuoteObjectName(tex).c_str());
						break;
				}
			}
		}
	}

#pragma endregion

#pragma region YYC Rule 5

	YYCRule5::YYCRule5() : Rule::IRule() {}

	YYCRule5::~YYCRule5() {}

	std::u8string_view YYCRule5::GetRuleName() const {
		return u8"YYC5";
	}

	void YYCRule5::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		// Build lowercase texture name set first.
		std::set<std::u8string> texs;
		for (const auto* tex_name : Shared::Name::Texture::ALL) {
			texs.emplace(strop::to_lower(tex_name));
		}

		// Check texture one by one
		for (auto& tex : level.GetTextures()) {
			auto tex_filename = Shared::Utility::ExtractTextureFileName(tex);
			if (!tex_filename.has_value()) continue;

			auto lower_tex_filename = strop::to_lower(tex_filename.value());
			bool is_ballance_tex = texs.contains(lower_tex_filename);

			using C::CK_TEXTURE_SAVEOPTIONS;
			switch (tex->GetUnderlyingData().GetSaveOptions()) {
				case CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_USEGLOBAL:
					reporter.FormatInfo(u8"The save option of texture %s rely on global Virtools settings. "
					                    u8"This cause ambiguity and different behavior on different Virtools by different user settings. "
					                    u8"Please consider change it to explicit option, such as External or Raw Data.",
					                    Shared::Utility::QuoteObjectName(tex).c_str());
					break;
				case CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_EXTERNAL:
					if (!is_ballance_tex) {
						reporter.FormatWarning(u8"Texture %s is not Ballance texture, but its save option is External. "
						                       u8"This may cause texture loss when rendering in game. Please consider store it inside map.",
						                       Shared::Utility::QuoteObjectName(tex).c_str());
					}
					break;
				default:
					if (is_ballance_tex) {
						reporter.FormatInfo(u8"Texture %s is Ballance texture, but its save option is not External. "
						                    u8"Please consider storing it as External to reduce the final size of map file, "
						                    u8"and let user specified texture pack work.",
						                    Shared::Utility::QuoteObjectName(tex).c_str());
					}
					break;
			}
		}
	}

#pragma endregion

#pragma region YYC Rule 6

	YYCRule6::YYCRule6() : Rule::IRule() {}

	YYCRule6::~YYCRule6() {}

	std::u8string_view YYCRule6::GetRuleName() const {
		return u8"YYC6";
	}

	void YYCRule6::Check(Reporter::Reporter& reporter, Map::Level& level) const {
		// TODO:
		// This rule is not so essential.
		// So we are not urgently to implement it in there.
		// Just make a rule placeholder in there and may finish it in future.
	}

#pragma endregion

} // namespace BMapInspector::Ruleset
