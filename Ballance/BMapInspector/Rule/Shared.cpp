#include "Shared.hpp"
#include <yycc/carton/termcolor.hpp>
#include <filesystem>
#include <stdexcept>
#include <cmath>

namespace strop = yycc::string::op;
namespace termcolor = yycc::carton::termcolor;

namespace BMapInspector::Rule::Shared {

#pragma region Utility Classes

#pragma region Sector Name Builder

	SectorNameBuilder::SectorNameBuilder() {}

	SectorNameBuilder::~SectorNameBuilder() {}

	SectorName SectorNameBuilder::get_name(L::CKDWORD sector) const {
		if (sector < MIN_SECTOR || sector > MAX_SECTOR) {
			throw std::logic_error("invalid sector number");
		} else {
			if (sector < 9) {
				return strop::printf(u8"Sector_%02" PRIuCKDWORD, sector);
			} else {
				return strop::printf(u8"Sector_%" PRIuCKDWORD, sector);
			}
		}
	}

	Sector9Names SectorNameBuilder::get_sector9_names() const {
		return Sector9Names{.legacy_name = u8"Sector_9", .intuitive_name = u8"Sector_09"};
	}

#pragma endregion

#pragma endregion

#pragma region Check Functions

	bool FPEqual(L::CKFLOAT lhs, L::CKFLOAT rhs, L::CKFLOAT tolerance) {
		auto diff = lhs - rhs;
		auto absolute_diff = std::fabs(diff);
		return absolute_diff <= tolerance;
	}

	O::CKGroup* FetchGroup(C::CKContext* ctx, L::CKSTRING name) {
		return static_cast<O::CKGroup*>(ctx->GetObjectByNameAndClass(name, C::CK_CLASSID::CKCID_GROUP, nullptr));
	}

	static void Iter3dObjectsEx(std::vector<O::CK3dObject*>& container, O::CKGroup* group) {
		for (L::CKDWORD obj_idx = 0; obj_idx < group->GetObjectCount(); ++obj_idx) {
			auto* group_beobject = group->GetObject(obj_idx);
			if (!C::CKIsChildClassOf(group_beobject->GetClassID(), C::CK_CLASSID::CKCID_3DOBJECT)) continue;
			auto* group_3dobject = static_cast<O::CK3dObject*>(group_beobject);
			container.emplace_back(group_3dobject);
		}
	}

	O::CKMaterial* FetchMaterial(C::CKContext* ctx, L::CKSTRING name) {
		return static_cast<O::CKMaterial*>(ctx->GetObjectByNameAndClass(name, C::CK_CLASSID::CKCID_MATERIAL, nullptr));
	}

	std::vector<O::CK3dObject*> FetchPhysicalized3dObjects(C::CKContext* ctx) {
		std::vector<O::CK3dObject*> rv;

		auto* phys_floors = FetchGroup(ctx, GroupNames::PHYS_FLOORS);
		if (phys_floors != nullptr) Iter3dObjectsEx(rv, phys_floors);
		auto* phys_floorrails = FetchGroup(ctx, GroupNames::PHYS_FLOORRAILS);
		if (phys_floorrails != nullptr) Iter3dObjectsEx(rv, phys_floorrails);
		auto* phys_floorstopper = FetchGroup(ctx, GroupNames::PHYS_FLOORSTOPPER);
		if (phys_floorstopper != nullptr) Iter3dObjectsEx(rv, phys_floorstopper);

		return rv;
	}

	bool CheckTextureFileName(O::CKTexture* tex, L::CKSTRING name) {
		// Get file name
		auto filename = tex->GetUnderlyingData().GetSlotFileName(0);
		if (filename == nullptr) return false;
		// Extract file name part
		std::filesystem::path filepath(filename);
		auto filename_part = filepath.filename().u8string();
		// Return result.
		return C::CKStrEqualI(filename_part.c_str(), name);
	}

	std::vector<O::CK3dObject*> Iter3dObjects(O::CKGroup* group) {
		std::vector<O::CK3dObject*> rv;
		Iter3dObjectsEx(rv, group);
		return rv;
	}

	std::vector<O::CKMaterial*> IterMaterial(O::CKMesh* mesh) {
		std::vector<O::CKMaterial*> rv;
		auto* mtls = mesh->GetMaterialSlots();
		for (L::CKDWORD mtl_idx = 0; mtl_idx < mesh->GetMaterialSlotCount(); ++mtl_idx) {
			auto* mtl = mtls[mtl_idx];
			if (mtl == nullptr) continue;
			rv.emplace_back(mtl);
		}
		return rv;
	}

	static const char8_t* RenderObjectName(O::CKObject* obj) {
		static std::u8string ANONYMOUS = termcolor::colored(u8"<anonymous>", termcolor::Color::LightMagenta);
		auto name = obj->GetName();
		if (name == nullptr) {
			return ANONYMOUS.c_str();
		} else {
			return name;
		}
	}

	std::u8string QuoteObjectName(O::CKObject* obj) {
		std::u8string rv;
		rv.push_back(u8'"');
		rv.append(RenderObjectName(obj));
		rv.push_back(u8'"');
		return rv;
	}

#pragma endregion

} // namespace BMapInspector::Rule::Shared
