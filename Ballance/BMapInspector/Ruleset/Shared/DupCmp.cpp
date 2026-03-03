#include "DupCmp.hpp"
#include <yycc.hpp>
#include <yycc/string/op.hpp>
#include <algorithm>

namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace V = LibCmo::VxMath;
namespace O = LibCmo::CK2::ObjImpls;
namespace strop = yycc::string::op;

namespace std {

#pragma region Primitive CK Hasher

	using BMapInspector::Ruleset::Shared::DupCmp::Hasher;

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

#pragma endregion

} // namespace std

namespace BMapInspector::Ruleset::Shared::DupCmp {

#pragma region Hash Combiner

	Hasher::Hasher() : seed(FNV_OFFSET_BASIS) {}

	Hasher::~Hasher() {}

	void Hasher::combine(ValueType h) {
		this->seed ^= h;
		this->seed *= FNV_PRIME;
	}

#pragma endregion

#pragma region CKObject Hash and Equal

	size_t CKTextureHash::operator()(const O::CKTexture* tex) const noexcept {
		const auto& texdata = tex->GetUnderlyingData();
		Hasher combiner;

		auto filename = texdata.GetSlotFileName(0);
		if (filename == nullptr) {
			combiner.update(nullptr);
		} else {
			auto lower_filename = strop::to_lower(filename);
			combiner.update(lower_filename);
		}
		combiner.update(texdata.GetSaveOptions());
		combiner.update(tex->GetVideoFormat());

		return combiner.finish();
	}

	size_t CKMaterialHash::operator()(const O::CKMaterial* mtl) const noexcept {
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

	size_t CKMeshHash::operator()(const O::CKMesh* _mesh) const noexcept {
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

	bool CKTextureEqualTo::operator()(const O::CKTexture* lhs, const O::CKTexture* rhs) const {
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

	bool CKMaterialEqualTo::operator()(const O::CKMaterial* lhs, const O::CKMaterial* rhs) const {
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

	bool CKMeshEqualTo::operator()(const O::CKMesh* _lhs, const O::CKMesh* _rhs) const {
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
		if (!std::equal(lhs->GetFaceMaterialSlotIndexs(), lhs->GetFaceMaterialSlotIndexs() + face_count, rhs->GetFaceMaterialSlotIndexs()))
			return false;

		// Compare material slot count
		auto material_slot_count = lhs->GetMaterialSlotCount();
		if (material_slot_count != rhs->GetMaterialSlotCount()) return false;

		// Compare material slots array
		if (!std::equal(lhs->GetMaterialSlots(), lhs->GetMaterialSlots() + material_slot_count, rhs->GetMaterialSlots())) return false;

		return true;
	}

#pragma endregion

#pragma region CKObject Wrapper

	CKTextureWrapper::CKTextureWrapper(O::CKTexture* texture) : texture(texture), hasher(), hash(std::nullopt) {}

	CKTextureWrapper::~CKTextureWrapper() {}

	O::CKTexture* CKTextureWrapper::GetTexture() const { return texture; }

	size_t CKTextureWrapper::GetHash() const {
		if (!hash.has_value()) hash = hasher(texture);
		return hash.value();
	}

	CKMaterialWrapper::CKMaterialWrapper(O::CKMaterial* material) : material(material), hasher(), hash(std::nullopt) {}

	CKMaterialWrapper::~CKMaterialWrapper() {}

	O::CKMaterial* CKMaterialWrapper::GetMaterial() const { return material; }

	size_t CKMaterialWrapper::GetHash() const {
		if (!hash.has_value()) hash = hasher(material);
		return hash.value();
	}

	CKMeshWrapper::CKMeshWrapper(O::CKMesh* mesh) : mesh(mesh), hasher(), hash(std::nullopt) {}

	CKMeshWrapper::~CKMeshWrapper() {}

	O::CKMesh* CKMeshWrapper::GetMesh() const { return mesh; }

	size_t CKMeshWrapper::GetHash() const {
		if (!hash.has_value()) hash = hasher(mesh);
		return hash.value();
	}

#pragma endregion

#pragma region CKObject Wrapper Hash and Equal

	size_t CKTextureWrapperHash::operator()(const CKTextureWrapper& tex) const noexcept { return tex.GetHash(); }

	size_t CKMaterialWrapperHash::operator()(const CKMaterialWrapper& mtl) const noexcept { return mtl.GetHash(); }

	size_t CKMeshWrapperHash::operator()(const CKMeshWrapper& mesh) const noexcept { return mesh.GetHash(); }

	bool CKTextureWrapperEqualTo::operator()(const CKTextureWrapper& lhs, const CKTextureWrapper& rhs) const {
		if (lhs.GetHash() != rhs.GetHash()) return false;
		return equal_to(lhs.GetTexture(), rhs.GetTexture());
	}

	bool CKMaterialWrapperEqualTo::operator()(const CKMaterialWrapper& lhs, const CKMaterialWrapper& rhs) const {
		if (lhs.GetHash() != rhs.GetHash()) return false;
		return equal_to(lhs.GetMaterial(), rhs.GetMaterial());
	}

	bool CKMeshWrapperEqualTo::operator()(const CKMeshWrapper& lhs, const CKMeshWrapper& rhs) const {
		if (lhs.GetHash() != rhs.GetHash()) return false;
		return equal_to(lhs.GetMesh(), rhs.GetMesh());
	}

#pragma endregion

} // namespace BMapInspector::Ruleset::Shared::DupCmp
