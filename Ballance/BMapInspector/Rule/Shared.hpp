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


		constexpr std::array OPAQUE_TEXS{
			u8"atari.bmp",
			u8"Ball_LightningSphere1.bmp",
			u8"Ball_LightningSphere2.bmp",
			u8"Ball_LightningSphere3.bmp",
			u8"Ball_Paper.bmp",
			u8"Ball_Stone.bmp",
			u8"Ball_Wood.bmp",
			u8"Brick.bmp",
			u8"Column_beige.bmp",
			u8"Column_blue.bmp",
			u8"Dome.bmp",
			u8"DomeEnvironment.bmp",
			u8"ExtraBall.bmp",
			u8"ExtraParticle.bmp",
			u8"E_Holzbeschlag.bmp",
			u8"FloorGlow.bmp",
			u8"Floor_Side.bmp",
			u8"Floor_Top_Border.bmp",
			u8"Floor_Top_Borderless.bmp",
			u8"Floor_Top_Checkpoint.bmp",
			u8"Floor_Top_Flat.bmp",
			u8"Floor_Top_Profil.bmp",
			u8"Floor_Top_ProfilFlat.bmp",
			u8"Gravitylogo_intro.bmp",
			u8"HardShadow.bmp",
			u8"Laterne_Glas.bmp",
			u8"Logo.bmp",
			u8"Metal_stained.bmp",
			u8"Misc_Ufo.bmp",
			u8"Misc_UFO_Flash.bmp",
			u8"Modul03_Floor.bmp",
			u8"Modul03_Wall.bmp",
			u8"Modul11_13_Wood.bmp",
			u8"Modul11_Wood.bmp",
			u8"Modul15.bmp",
			u8"Modul16.bmp",
			u8"Modul18.bmp",
			u8"Modul30_d_Seiten.bmp",
			u8"Particle_Flames.bmp",
			u8"Particle_Smoke.bmp",
			u8"PE_Bal_balloons.bmp",
			u8"PE_Bal_platform.bmp",
			u8"PE_Ufo_env.bmp",
			u8"P_Extra_Life_Oil.bmp",
			u8"P_Extra_Life_Particle.bmp",
			u8"P_Extra_Life_Shadow.bmp",
			u8"Rail_Environment.bmp",
			u8"sandsack.bmp",
			u8"SkyLayer.bmp",
			u8"Sky_Vortex.bmp",
			u8"Stick_Stripes.bmp",
			u8"Target.bmp",
			u8"Tower_Roof.bmp",
			u8"Trafo_Environment.bmp",
			u8"Trafo_FlashField.bmp",
			u8"Wood_Metal.bmp",
			u8"Wood_MetalStripes.bmp",
			u8"Wood_Misc.bmp",
			u8"Wood_Nailed.bmp",
			u8"Wood_Old.bmp",
			u8"Wood_Panel.bmp",
			u8"Wood_Plain.bmp",
			u8"Wood_Plain2.bmp",
			u8"Wood_Raft.bmp",
		};
		constexpr std::array TRANSPARENT_TEXS{
			u8"Button01_deselect.tga",
			u8"Button01_select.tga",
			u8"Button01_special.tga",
			u8"Column_beige_fade.tga",
			u8"Cursor.tga",
			u8"DomeShadow.tga",
			u8"Font_1.tga",
			u8"Laterne_Schatten.tga",
			u8"Laterne_Verlauf.tga",
			u8"Modul18_Gitter.tga",
			u8"Pfeil.tga",
			u8"Stick_Bottom.tga",
			u8"Trafo_Shadow_Big.tga",
			u8"Tut_Pfeil01.tga",
			u8"Tut_Pfeil_Hoch.tga",
			u8"Wolken_intro.tga",
		};

		constexpr std::array ALL{
			// u8"atari.avi",
			u8"atari.bmp",
			u8"Ball_LightningSphere1.bmp",
			u8"Ball_LightningSphere2.bmp",
			u8"Ball_LightningSphere3.bmp",
			u8"Ball_Paper.bmp",
			u8"Ball_Stone.bmp",
			u8"Ball_Wood.bmp",
			u8"Brick.bmp",
			u8"Button01_deselect.tga",
			u8"Button01_select.tga",
			u8"Button01_special.tga",
			u8"Column_beige.bmp",
			u8"Column_beige_fade.tga",
			u8"Column_blue.bmp",
			u8"Cursor.tga",
			u8"Dome.bmp",
			u8"DomeEnvironment.bmp",
			u8"DomeShadow.tga",
			u8"ExtraBall.bmp",
			u8"ExtraParticle.bmp",
			u8"E_Holzbeschlag.bmp",
			u8"FloorGlow.bmp",
			u8"Floor_Side.bmp",
			u8"Floor_Top_Border.bmp",
			u8"Floor_Top_Borderless.bmp",
			u8"Floor_Top_Checkpoint.bmp",
			u8"Floor_Top_Flat.bmp",
			u8"Floor_Top_Profil.bmp",
			u8"Floor_Top_ProfilFlat.bmp",
			u8"Font_1.tga",
			u8"Gravitylogo_intro.bmp",
			u8"HardShadow.bmp",
			u8"Laterne_Glas.bmp",
			u8"Laterne_Schatten.tga",
			u8"Laterne_Verlauf.tga",
			u8"Logo.bmp",
			u8"Metal_stained.bmp",
			u8"Misc_Ufo.bmp",
			u8"Misc_UFO_Flash.bmp",
			u8"Modul03_Floor.bmp",
			u8"Modul03_Wall.bmp",
			u8"Modul11_13_Wood.bmp",
			u8"Modul11_Wood.bmp",
			u8"Modul15.bmp",
			u8"Modul16.bmp",
			u8"Modul18.bmp",
			u8"Modul18_Gitter.tga",
			u8"Modul30_d_Seiten.bmp",
			u8"Particle_Flames.bmp",
			u8"Particle_Smoke.bmp",
			u8"PE_Bal_balloons.bmp",
			u8"PE_Bal_platform.bmp",
			u8"PE_Ufo_env.bmp",
			u8"Pfeil.tga",
			u8"P_Extra_Life_Oil.bmp",
			u8"P_Extra_Life_Particle.bmp",
			u8"P_Extra_Life_Shadow.bmp",
			u8"Rail_Environment.bmp",
			u8"sandsack.bmp",
			u8"SkyLayer.bmp",
			u8"Sky_Vortex.bmp",
			u8"Stick_Bottom.tga",
			u8"Stick_Stripes.bmp",
			u8"Target.bmp",
			u8"Tower_Roof.bmp",
			u8"Trafo_Environment.bmp",
			u8"Trafo_FlashField.bmp",
			u8"Trafo_Shadow_Big.tga",
			u8"Tut_Pfeil01.tga",
			u8"Tut_Pfeil_Hoch.tga",
			u8"Wolken_intro.tga",
			u8"Wood_Metal.bmp",
			u8"Wood_MetalStripes.bmp",
			u8"Wood_Misc.bmp",
			u8"Wood_Nailed.bmp",
			u8"Wood_Old.bmp",
			u8"Wood_Panel.bmp",
			u8"Wood_Plain.bmp",
			u8"Wood_Plain2.bmp",
			u8"Wood_Raft.bmp",
		};

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
