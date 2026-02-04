#pragma once
#include <VTAll.hpp>
#include <vector>

namespace BMapInspector::Rule::Shared {

	namespace L = LibCmo;
	namespace C = LibCmo::CK2;
	namespace O = LibCmo::CK2::ObjImpls;

#pragma region Constant Values

	namespace GroupNames {
		constexpr char8_t PHYS_FLOORRAILS[] = u8"Phys_FloorRails";
	}

	namespace TextureNames {
		constexpr char8_t RAIL_ENVIRONMENT[] = u8"Rail_Environment.bmp";
	}

#pragma endregion

#pragma region Check Functions

	/**
	 * @brief 
	 * @param[in] ctx Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return 
	 */
	O::CKGroup* FetchGroup(C::CKContext* ctx, L::CKSTRING name);
	/**
	 * @brief Check whether given CKTexture has the given file name (case-insensitive).
	 * @param[in] tex Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return True if it is, otherwise false.
	 */
	bool CheckTextureFileName(O::CKTexture* tex, L::CKSTRING name);

	/**
	 * @brief 
	 * @param[in] group Can not be nullptr.
	 * @return All objects is the child class of CK3dEntity.
	 */
	std::vector<O::CK3dEntity*> Iter3dEntities(O::CKGroup* group);
	/**
	 * @brief 
	 * @param[in] mesh Can not be nullptr.
	 * @return All nullptr reference are removed.
	 */
	std::vector<O::CKMaterial*> IterMaterial(O::CKMesh* mesh);

	/**
	 * @brief 
	 * @param[in] obj Can not be nullptr.
	 * @return 
	 */
	const char8_t* RenderObjectName(O::CKObject* obj);

#pragma endregion

} // namespace BMapInspector::Rule::Shared
