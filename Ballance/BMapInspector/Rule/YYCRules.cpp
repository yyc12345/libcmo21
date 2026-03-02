#include "YYCRules.hpp"
#include "Shared.hpp"
#include <yycc.hpp>
#include <yycc/macro/ptr_size_detector.hpp>
#include <yycc/string/op.hpp>
#include <vector>
#include <set>
#include <algorithm>
#include <optional>
#include <type_traits>
#include <unordered_set>

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace V = LibCmo::VxMath;
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

#pragma region YYC Rule 3

	/**
	 * @brief FNV-1a Hash Combiner
	 */
	class Hasher {
	public:
		using ValueType = size_t;

	private:
#if defined(YYCC_PTRSIZE_32)
		static constexpr ValueType FNV_OFFSET_BASIS = 2166136261U;
		static constexpr ValueType FNV_PRIME = 16777619U;
#else
		static constexpr ValueType FNV_OFFSET_BASIS = 14695981039346656037ULL;
		static constexpr ValueType FNV_PRIME = 1099511628211ULL;
#endif

	public:
		Hasher() : seed(FNV_OFFSET_BASIS) {}
		~Hasher() {}

	private:
		/**
		 * @brief Update this hash combiner with new hash.
		 * @param h 
		 */
		void combine(ValueType h) {
			this->seed ^= h;
			this->seed *= FNV_PRIME;
		}

	public:
		/**
		 * @brief Get final produced hash.
		 * @return 
		 */
		[[nodiscard]] ValueType finish() const noexcept { return this->seed; }
		template<typename T>
		void update(const T& v) {
			std::hash<T> hasher;
			combine(hasher(v));
		}
		template<typename T>
		void update_array(const T* addr, size_t cnt) {
			std::hash<T> hasher;
			for (size_t i = 0; i < cnt; ++i) {
				combine(hasher(addr[i]));
			}
		}

	private:
		ValueType seed;
	};

#pragma region Primitive CK Hasher
} // namespace BMapInspector::Rule

namespace std {
	using BMapInspector::Rule::Hasher;

	template<>
	struct hash<V::VxColor> {
		[[nodiscard]] size_t operator()(const V::VxColor& color) const noexcept {
			Hasher combiner;
			combiner.update(color.r);
			combiner.update(color.g);
			combiner.update(color.b);
			combiner.update(color.a);
			return combiner.finish();
		}
	};

	template<>
	struct hash<V::VxVector2> {
		[[nodiscard]] size_t operator()(const V::VxVector2& vec) const noexcept {
			Hasher combiner;
			combiner.update(vec.x);
			combiner.update(vec.y);
			return combiner.finish();
		}
	};

	template<>
	struct hash<V::VxVector3> {
		[[nodiscard]] size_t operator()(const V::VxVector3& vec) const noexcept {
			Hasher combiner;
			combiner.update(vec.x);
			combiner.update(vec.y);
			combiner.update(vec.z);
			return combiner.finish();
		}
	};

} // namespace std

namespace BMapInspector::Rule {
#pragma endregion

#pragma region CKObject Hash and Equal

	struct CKTextureHash {
		[[nodiscard]] size_t operator()(const O::CKTexture* tex) const noexcept {
			const auto& texdata = tex->GetUnderlyingData();
			Hasher combiner;

			auto filename = texdata.GetSlotFileName(0);
			if (filename == nullptr) {
				combiner.update(nullptr);
			} else {
				auto lower_filename = yycc::string::op::to_lower(filename);
				combiner.update(lower_filename);
			}
			combiner.update(texdata.GetSaveOptions());
			combiner.update(tex->GetVideoFormat());

			return combiner.finish();
		}
	};

	struct CKMaterialHash {
		[[nodiscard]] size_t operator()(const O::CKMaterial* mtl) const noexcept {
			Hasher combiner;

			combiner.update(mtl->GetDiffuse());
			combiner.update(mtl->GetAmbient());
			combiner.update(mtl->GetSpecular());
			combiner.update(mtl->GetEmissive());
			combiner.update(mtl->GetSpecularPower());

			// TODO:
			// Use raw pointer for hash is dangerous.
			// But who cares? I simply assume that there is no memory reallocation.
			combiner.update(mtl->GetTexture());
			combiner.update(mtl->GetTextureBorderColor());

			combiner.update(mtl->GetTextureBlendMode());
			combiner.update(mtl->GetTextureMinMode());
			combiner.update(mtl->GetTextureMagMode());
			combiner.update(mtl->GetTextureAddressMode());

			combiner.update(mtl->GetSourceBlend());
			combiner.update(mtl->GetDestBlend());
			combiner.update(mtl->GetFillMode());
			combiner.update(mtl->GetShadeMode());

			// TODO:
			// We also need use these "Enabled" variable to switch on/off
			// for some field's hashing according to the Virtools layout.
			// But I am lazy now.
			// I guess there is the same default values for those fields
			// controlled by some disable "Enabled" variable.
			combiner.update(mtl->GetAlphaTestEnabled());
			combiner.update(mtl->GetAlphaBlendEnabled());
			combiner.update(mtl->GetPerspectiveCorrectionEnabled());
			combiner.update(mtl->GetZWriteEnabled());
			combiner.update(mtl->GetTwoSidedEnabled());

			combiner.update(mtl->GetAlphaRef());
			combiner.update(mtl->GetAlphaFunc());
			combiner.update(mtl->GetZFunc());

			return combiner.finish();
		}
	};

	struct CKMeshHash {
		[[nodiscard]] size_t operator()(const O::CKMesh* _mesh) const noexcept {
			O::CKMesh* mesh = const_cast<O::CKMesh*>(_mesh);
			Hasher combiner;

			combiner.update(mesh->GetLitMode());

			auto vertex_count = mesh->GetVertexCount();
			combiner.update(vertex_count);
			combiner.update_array(mesh->GetVertexPositions(), vertex_count);
			combiner.update_array(mesh->GetVertexNormals(), vertex_count);
			combiner.update_array(mesh->GetVertexUVs(), vertex_count);

			// TODO:
			// In theory, we need remap face material slot index to underlying material CKID,
			// but its too complex. I give up.
			auto face_count = mesh->GetFaceCount();
			combiner.update(face_count);
			combiner.update_array(mesh->GetFaceIndices(), face_count * 3);
			combiner.update_array(mesh->GetFaceMaterialSlotIndexs(), face_count);

			auto material_slot_count = mesh->GetMaterialSlotCount();
			combiner.update(material_slot_count);
			// TODO:
			// Same dangerous usage of raw pointer.
			combiner.update_array(mesh->GetMaterialSlots(), material_slot_count);

			return combiner.finish();
		}
	};

	struct CKTextureEqualTo {
		[[nodiscard]] bool operator()(const O::CKTexture* lhs, const O::CKTexture* rhs) const {
			// Compare underlying data
			const auto& lhs_data = lhs->GetUnderlyingData();
			const auto& rhs_data = rhs->GetUnderlyingData();

			// Compare filename (case insensitive)
			auto lhs_filename = lhs_data.GetSlotFileName(0);
			auto rhs_filename = rhs_data.GetSlotFileName(0);
			if (!C::CKStrEqualI(lhs_filename, rhs_filename)) return false;

			// Compare save options
			if (lhs_data.GetSaveOptions() != rhs_data.GetSaveOptions()) return false;
			// Compare video format
			if (lhs->GetVideoFormat() != rhs->GetVideoFormat()) return false;

			return true;
		}
	};

	struct CKMaterialEqualTo {
		[[nodiscard]] bool operator()(const O::CKMaterial* lhs, const O::CKMaterial* rhs) const {
			// Compare color properties
			if (lhs->GetDiffuse() != rhs->GetDiffuse()) return false;
			if (lhs->GetAmbient() != rhs->GetAmbient()) return false;
			if (lhs->GetSpecular() != rhs->GetSpecular()) return false;
			if (lhs->GetEmissive() != rhs->GetEmissive()) return false;
			if (lhs->GetSpecularPower() != rhs->GetSpecularPower()) return false;

			// Compare texture properties
			if (lhs->GetTexture() != rhs->GetTexture()) return false;
			if (lhs->GetTextureBorderColor() != rhs->GetTextureBorderColor()) return false;

			// Compare texture modes
			if (lhs->GetTextureBlendMode() != rhs->GetTextureBlendMode()) return false;
			if (lhs->GetTextureMinMode() != rhs->GetTextureMinMode()) return false;
			if (lhs->GetTextureMagMode() != rhs->GetTextureMagMode()) return false;
			if (lhs->GetTextureAddressMode() != rhs->GetTextureAddressMode()) return false;

			// Compare blend modes
			if (lhs->GetSourceBlend() != rhs->GetSourceBlend()) return false;
			if (lhs->GetDestBlend() != rhs->GetDestBlend()) return false;
			if (lhs->GetFillMode() != rhs->GetFillMode()) return false;
			if (lhs->GetShadeMode() != rhs->GetShadeMode()) return false;

			// Compare enable flags
			if (lhs->GetAlphaTestEnabled() != rhs->GetAlphaTestEnabled()) return false;
			if (lhs->GetAlphaBlendEnabled() != rhs->GetAlphaBlendEnabled()) return false;
			if (lhs->GetPerspectiveCorrectionEnabled() != rhs->GetPerspectiveCorrectionEnabled()) return false;
			if (lhs->GetZWriteEnabled() != rhs->GetZWriteEnabled()) return false;
			if (lhs->GetTwoSidedEnabled() != rhs->GetTwoSidedEnabled()) return false;

			// Compare alpha and z function properties
			if (lhs->GetAlphaRef() != rhs->GetAlphaRef()) return false;
			if (lhs->GetAlphaFunc() != rhs->GetAlphaFunc()) return false;
			if (lhs->GetZFunc() != rhs->GetZFunc()) return false;

			return true;
		}
	};

	struct CKMeshEqualTo {
		[[nodiscard]] bool operator()(const O::CKMesh* _lhs, const O::CKMesh* _rhs) const {
			O::CKMesh* lhs = const_cast<O::CKMesh*>(_lhs);
			O::CKMesh* rhs = const_cast<O::CKMesh*>(_rhs);

			// Compare lit mode
			if (lhs->GetLitMode() != rhs->GetLitMode()) return false;

			// Compare vertex count
			auto vertex_count = lhs->GetVertexCount();
			if (vertex_count != rhs->GetVertexCount()) return false;

			// Compare vertex data arrays
			if (!std::equal(lhs->GetVertexPositions(), lhs->GetVertexPositions() + vertex_count, rhs->GetVertexPositions())) return false;
			if (!std::equal(lhs->GetVertexNormals(), lhs->GetVertexNormals() + vertex_count, rhs->GetVertexNormals())) return false;
			if (!std::equal(lhs->GetVertexUVs(), lhs->GetVertexUVs() + vertex_count, rhs->GetVertexUVs())) return false;

			// Compare face count
			auto face_count = lhs->GetFaceCount();
			if (face_count != rhs->GetFaceCount()) return false;

			// Compare face data arrays
			if (!std::equal(lhs->GetFaceIndices(), lhs->GetFaceIndices() + face_count * 3, rhs->GetFaceIndices())) return false;
			if (!std::equal(lhs->GetFaceMaterialSlotIndexs(),
			                lhs->GetFaceMaterialSlotIndexs() + face_count,
			                rhs->GetFaceMaterialSlotIndexs()))
				return false;

			// Compare material slot count
			auto material_slot_count = lhs->GetMaterialSlotCount();
			if (material_slot_count != rhs->GetMaterialSlotCount()) return false;

			// Compare material slots array
			if (!std::equal(lhs->GetMaterialSlots(), lhs->GetMaterialSlots() + material_slot_count, rhs->GetMaterialSlots())) return false;

			return true;
		}
	};

#pragma endregion

#pragma region CKObject Wrapper

	class CKTextureWrapper {
	public:
		CKTextureWrapper(O::CKTexture* texture) : texture(texture), hasher(), hash(std::nullopt) {}
		~CKTextureWrapper() {}
		YYCC_DEFAULT_COPY_MOVE(CKTextureWrapper)

	public:
		O::CKTexture* GetTexture() const { return texture; }
		size_t GetHash() const {
			if (!hash.has_value()) hash = hasher(texture);
			return hash.value();
		}

	private:
		O::CKTexture* texture;
		CKTextureHash hasher;
		mutable std::optional<size_t> hash;
	};

	class CKMaterialWrapper {
	public:
		CKMaterialWrapper(O::CKMaterial* material) : material(material), hasher(), hash(std::nullopt) {}
		~CKMaterialWrapper() {}
		YYCC_DEFAULT_COPY_MOVE(CKMaterialWrapper)

	public:
		O::CKMaterial* GetMaterial() const { return material; }
		size_t GetHash() const {
			if (!hash.has_value()) hash = hasher(material);
			return hash.value();
		}

	private:
		O::CKMaterial* material;
		CKMaterialHash hasher;
		mutable std::optional<size_t> hash;
	};

	class CKMeshWrapper {
	public:
		CKMeshWrapper(O::CKMesh* mesh) : mesh(mesh), hasher(), hash(std::nullopt) {}
		~CKMeshWrapper() {}
		YYCC_DEFAULT_COPY_MOVE(CKMeshWrapper)

	public:
		O::CKMesh* GetMesh() const { return mesh; }
		size_t GetHash() const {
			if (!hash.has_value()) hash = hasher(mesh);
			return hash.value();
		}

	private:
		O::CKMesh* mesh;
		CKMeshHash hasher;
		mutable std::optional<size_t> hash;
	};

#pragma endregion

#pragma region CKObject Wrapper Hash and Equal

	struct CKTextureWrapperHash {
		[[nodiscard]] size_t operator()(const CKTextureWrapper& tex) const noexcept { return tex.GetHash(); }
	};

	struct CKMaterialWrapperHash {
		[[nodiscard]] size_t operator()(const CKMaterialWrapper& mtl) const noexcept { return mtl.GetHash(); }
	};

	struct CKMeshWrapperHash {
		[[nodiscard]] size_t operator()(const CKMeshWrapper& mesh) const noexcept { return mesh.GetHash(); }
	};

	struct CKTextureWrapperEqualTo {
		CKTextureEqualTo equal_to;
		[[nodiscard]] bool operator()(const CKTextureWrapper& lhs, const CKTextureWrapper& rhs) const {
			if (lhs.GetHash() != rhs.GetHash()) return false;
			return equal_to(lhs.GetTexture(), rhs.GetTexture());
		}
	};

	struct CKMaterialWrapperEqualTo {
		CKMaterialEqualTo equal_to;
		[[nodiscard]] bool operator()(const CKMaterialWrapper& lhs, const CKMaterialWrapper& rhs) const {
			if (lhs.GetHash() != rhs.GetHash()) return false;
			return equal_to(lhs.GetMaterial(), rhs.GetMaterial());
		}
	};

	struct CKMeshWrapperEqualTo {
		CKMeshEqualTo equal_to;
		[[nodiscard]] bool operator()(const CKMeshWrapper& lhs, const CKMeshWrapper& rhs) const {
			if (lhs.GetHash() != rhs.GetHash()) return false;
			return equal_to(lhs.GetMesh(), rhs.GetMesh());
		}
	};

#pragma endregion

	constexpr char8_t YYC3[] = u8"YYC3";

	YYCRule3::YYCRule3() : IRule() {}

	YYCRule3::~YYCRule3() {}

	std::u8string_view YYCRule3::GetRuleName() const {
		return YYC3;
	}

	void YYCRule3::Check(Reporter::Reporter& reporter, Map::Level& level) const {
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

				reporter.FormatInfo(
				    YYC3,
				    u8"Some textures are visually identical. Please consider merging them to reduce the final map size. These textures are: %s.",
				    Shared::QuoteObjectNames(dup_texs.begin(), dup_texs.end()).c_str());
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

				reporter.FormatInfo(
				    YYC3,
				    u8"Some materials are visually identical. Please consider merging them to reduce the final map size. These materials are: %s.",
				    Shared::QuoteObjectNames(dup_mtls.begin(), dup_mtls.end()).c_str());
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

				reporter.FormatInfo(
				    YYC3,
				    u8"Some meshes are visually identical. Please consider merging them to reduce the final map size. These meshes are: %s.",
				    Shared::QuoteObjectNames(dup_meshes.begin(), dup_meshes.end()).c_str());
			} else {
				++it;
			}
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule
