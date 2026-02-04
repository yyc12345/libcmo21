#pragma once
#include <VTAll.hpp>
#include <vector>
#include <array>

namespace BMapInspector::Rule::Shared {

	namespace L = LibCmo;
	namespace C = LibCmo::CK2;
	namespace O = LibCmo::CK2::ObjImpls;

#pragma region Constant Values

	namespace GroupNames {
		// clang-format off
		constexpr char8_t PHYS_FLOORS[] = u8"Phys_Floors";
		constexpr char8_t PHYS_FLOORRAILS[] = u8"Phys_FloorRails";
		constexpr char8_t PHYS_FLOORSTOPPER[] = u8"Phys_FloorStopper";
		// clang-format on
	} // namespace GroupNames

	namespace TextureNames {
		// clang-format off
		constexpr char8_t RAIL_ENVIRONMENT[] = u8"Rail_Environment.bmp";
		// clang-format on

	} // namespace TextureNames

#pragma endregion

#pragma region Check Functions

	/**
	 * @brief 
	 * @param[in] ctx Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return 
	 */
	O::CKGroup* FetchGroup(C::CKContext* ctx, L::CKSTRING name);
	std::vector<O::CK3dObject*> FetchPhysicalized3dObjects(C::CKContext* ctx);
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
	std::vector<O::CK3dObject*> Iter3dObjects(O::CKGroup* group);
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
