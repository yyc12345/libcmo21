#include "Shared.hpp"
#include <yycc.hpp>
#include <yycc/carton/termcolor.hpp>
#include <filesystem>

namespace termcolor = yycc::carton::termcolor;

namespace BMapInspector::Rule::Shared {

#pragma region Check Functions

	/**
	 * @brief 
	 * @param[in] ctx Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return Found pointer to CKGroup, otherwise nullptr.
	 */
	O::CKGroup* FetchGroup(C::CKContext* ctx, L::CKSTRING name) {
		return static_cast<O::CKGroup*>(ctx->GetObjectByNameAndClass(name, C::CK_CLASSID::CKCID_GROUP, nullptr));
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

	std::vector<O::CK3dEntity*> Iter3dEntities(O::CKGroup* group) {
		std::vector<O::CK3dEntity*> rv;
		for (L::CKDWORD obj_idx = 0; obj_idx < group->GetObjectCount(); ++obj_idx) {
			auto* group_object = group->GetObject(obj_idx);
			if (!C::CKIsChildClassOf(group_object->GetClassID(), C::CK_CLASSID::CKCID_3DENTITY)) continue;
			auto* group_3dentity = static_cast<O::CK3dEntity*>(group_object);
			rv.emplace_back(group_3dentity);
		}
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

	const char8_t* RenderObjectName(O::CKObject* obj) {
		static std::u8string ANONYMOUS = termcolor::colored(u8"<anonymous>", termcolor::Color::LightMagenta);
		auto name = obj->GetName();
		if (name == nullptr) {
			return ANONYMOUS.c_str();
		} else {
			return name;
		}
	}

#pragma endregion

} // namespace BMapInspector::Rule::Shared
