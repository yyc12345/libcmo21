#pragma once
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/string/op.hpp>
#include <vector>
#include <array>
#include <type_traits>

namespace BMapInspector::Rule::Shared {

	namespace L = LibCmo;
	namespace C = LibCmo::CK2;
	namespace O = LibCmo::CK2::ObjImpls;

#pragma region Constant Values

	namespace GroupNames {
		// clang-format off
		constexpr char8_t PS_LEVELSTART[] = u8"PS_Levelstart";
		constexpr char8_t PE_LEVELENDE[] = u8"PE_Levelende";
		constexpr char8_t PC_CHECKPOINTS[] = u8"PC_Checkpoints";
		constexpr char8_t PR_RESETPOINTS[] = u8"PR_Resetpoints";

		constexpr char8_t PHYS_FLOORS[] = u8"Phys_Floors";
		constexpr char8_t PHYS_FLOORRAILS[] = u8"Phys_FloorRails";
		constexpr char8_t PHYS_FLOORSTOPPER[] = u8"Phys_FloorStopper";

		constexpr std::array ALL_PH{
			u8"P_Extra_Life",
			u8"P_Extra_Point",
			u8"P_Trafo_Paper",
			u8"P_Trafo_Stone",
			u8"P_Trafo_Wood",
			u8"P_Ball_Paper",
			u8"P_Ball_Stone",
			u8"P_Ball_Wood",
			u8"P_Box",
			u8"P_Dome",
			u8"P_Modul_01",
			u8"P_Modul_03",
			u8"P_Modul_08",
			u8"P_Modul_17",
			u8"P_Modul_18",
			u8"P_Modul_19",
			u8"P_Modul_25",
			u8"P_Modul_26",
			u8"P_Modul_29",
			u8"P_Modul_30",
			u8"P_Modul_34",
			u8"P_Modul_37",
			u8"P_Modul_41",
			u8"PS_Levelstart",
			u8"PE_Levelende",
			u8"PC_Checkpoints",
			u8"PR_Resetpoints",
		};

		// clang-format on
	} // namespace GroupNames

	namespace TextureNames {
		// clang-format off
		constexpr char8_t RAIL_ENVIRONMENT[] = u8"Rail_Environment.bmp";
		constexpr char8_t LATERNE_VERLAUF[] = u8"Laterne_Verlauf.tga";
		// clang-format on

	} // namespace TextureNames

#pragma endregion

#pragma region Utility Classes

	constexpr L::CKDWORD MIN_SECTOR = 1;
	constexpr L::CKDWORD MAX_SECTOR = 999;

	using SectorName = std::u8string;

	struct Sector9Names {
		/** The Sector 9 name with "Sector_9" pattern which is accepted by all 999 sector loader */
		std::u8string legacy_name;
		/** The Sector 9 name with "Sector_09" pattern which is only accepted by new 999 sector loader */
		std::u8string intuitive_name;
	};

	/**
	 * @brief The class for building Ballance sector group name.
	 */
	class SectorNameBuilder {
	public:
		SectorNameBuilder();
		~SectorNameBuilder();
		YYCC_DEFAULT_COPY_MOVE(SectorNameBuilder)

	public:
		/**
		 * @brief 
		 * @param[in] sector 
		 * @return 
		 * @remarks
		 * If you deliver sector index with 9, its return name is "Sector_9" which is accepted by all 999 sector loader.
		 */
		SectorName get_name(L::CKDWORD sector) const;
		Sector9Names get_sector9_names() const;
	};

#pragma endregion

#pragma region Check Functions

	/**
	 * @brief Check whether given 2 float point values are equal with given tolerance.
	 * @param lhs 
	 * @param rhs 
	 * @param tolerance 
	 * @return 
	 */
	bool FPEqual(L::CKFLOAT lhs, L::CKFLOAT rhs, L::CKFLOAT tolerance);

	/**
	 * @brief 
	 * @param[in] ctx Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return Found pointer to CKGroup, otherwise nullptr.
	 */
	O::CKGroup* FetchGroup(C::CKContext* ctx, L::CKSTRING name);
	/**
	 * @brief 
	 * @param[in] ctx Can not be nullptr.
	 * @param[in] name Can not be nullptr.
	 * @return Found pointer to CKMaterial, otherwise nullptr.
	 */
	O::CKMaterial* FetchMaterial(C::CKContext* ctx, L::CKSTRING name);
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
	std::u8string QuoteObjectName(O::CKObject* obj);
	/**
	 * @brief 
	 * @tparam InputIt 
	 * @param first 
	 * @param last 
	 * @return 
	 */
	template<std::input_iterator InputIt>
	    requires std::is_pointer_v<std::iter_value_t<InputIt>>
	             && std::is_base_of_v<O::CKObject, std::remove_pointer_t<std::iter_value_t<InputIt>>>
	std::u8string QuoteObjectNames(InputIt first, InputIt last) {
		std::u8string cache;
		return yycc::string::op::join(
		    [&cache, &first, &last]() -> std::optional<std::u8string_view> {
			    if (first == last) return std::nullopt;
				// YYC MARK:
				// We must use "cache", otherwise "use after free" will occur.
			    cache = QuoteObjectName(*(first++));
			    return cache;
		    },
		    u8", ");
	}

#pragma endregion

} // namespace BMapInspector::Rule::Shared
