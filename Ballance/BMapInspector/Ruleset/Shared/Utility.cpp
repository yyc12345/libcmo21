#include "Utility.hpp"
#include "Name.hpp"
#include <yycc.hpp>
#include <yycc/carton/termcolor.hpp>
#include <filesystem>
#include <stdexcept>
#include <cmath>

namespace strop = yycc::string::op;
namespace termcolor = yycc::carton::termcolor;
namespace L = LibCmo;
namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Ruleset::Shared::Utility {

	#pragma region Utilities


	bool FPEqual(L::CKFLOAT lhs, L::CKFLOAT rhs, L::CKFLOAT tolerance) {
		auto diff = lhs - rhs;
		auto absolute_diff = std::fabs(diff);
		return absolute_diff <= tolerance;
	}

#pragma endregion

	#pragma region Virtools Stuff

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

		auto* phys_floors = FetchGroup(ctx, Name::Group::PHYS_FLOORS);
		if (phys_floors != nullptr) Iter3dObjectsEx(rv, phys_floors);
		auto* phys_floorrails = FetchGroup(ctx, Name::Group::PHYS_FLOORRAILS);
		if (phys_floorrails != nullptr) Iter3dObjectsEx(rv, phys_floorrails);
		auto* phys_floorstopper = FetchGroup(ctx, Name::Group::PHYS_FLOORSTOPPER);
		if (phys_floorstopper != nullptr) Iter3dObjectsEx(rv, phys_floorstopper);

		return rv;
	}

	std::optional<std::u8string> ExtractTextureFileName(O::CKTexture* tex) {
		// Get file name
		auto filename = tex->GetUnderlyingData().GetSlotFileName(0);
		if (filename == nullptr) return std::nullopt;
		// Extract file name part
		std::filesystem::path filepath(filename);
		auto filename_part = filepath.filename().u8string();
		return filename_part;
	}

	bool CheckTextureFileName(O::CKTexture* tex, L::CKSTRING name) {
		// Get file name part
		auto filename_part = ExtractTextureFileName(tex);
		if (!filename_part.has_value()) return false;
		// Return result.
		return C::CKStrEqualI(filename_part.value().c_str(), name);
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

#pragma endregion

	#pragma region Presentation

	std::u8string QuoteText(const std::u8string_view& words) {
		std::u8string rv;
		rv.reserve(words.size() + 2);

		rv.push_back('"');
		rv.append(words);
		rv.push_back('"');

		return rv;
	}

	std::u8string QuoteObjectName(O::CKObject* obj) {
		static std::u8string ANONYMOUS = termcolor::colored(u8"<anonymous>", termcolor::Color::LightMagenta);
		auto name = obj->GetName();
		if (name == nullptr) {
			return QuoteText(ANONYMOUS);
		} else {
			return QuoteText(name);
		}
	}

#pragma endregion

} // namespace BMapInspector::Ruleset::Shared::Utility
