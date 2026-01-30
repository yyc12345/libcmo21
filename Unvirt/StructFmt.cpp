#include "StructFmt.hpp"
#include "Docstring.hpp"
#include "Utils.hpp"
#include <yycc.hpp>
#include <yycc/string/op.hpp>
#include <yycc/carton/termcolor.hpp>
#include <yycc/carton/tabulate.hpp>
#include <yycc/patch/ptr_pad.hpp>
#include <yycc/patch/stream.hpp>
#include <iostream>
#include <initializer_list>

using namespace yycc::patch::stream;
namespace strop = yycc::string::op;
namespace termcolor = yycc::carton::termcolor;
namespace tabulate = yycc::carton::tabulate;
using termcolor::Color;
using Unvirt::Utils::PageBreaker;

namespace Unvirt::StructFmt {

#pragma region Assist Printer

	static std::u8string PrintCKSTRING(LibCmo::CKSTRING name) {
		std::u8string ret;
		if (name == nullptr) {
			ret = termcolor::colored(u8"<anonymous>", Color::LightMagenta);
		} else {
			if (name[0] == u8'\0') {
				ret = termcolor::colored(u8"<blank>", Color::LightMagenta);
			} else {
				ret = name;
			}
		}
		return ret;
	}
	static std::u8string PrintPointer(const void* ptr) {
		std::u8string ret;
		if (ptr == nullptr) {
			ret = termcolor::colored(u8"<null>", Color::LightCyan);
		} else {
			ret = strop::printf(u8"<0x%" PRIXPTR_LPAD PRIxPTR ">", reinterpret_cast<uintptr_t>(ptr));
		}
		return ret;
	}
	static std::u8string PrintCKGUID(const LibCmo::CK2::CKGUID& guid) {
		return strop::printf(u8"<0x%08" PRIxCKDWORD ", 0x%08" PRIxCKDWORD ">", guid.d1, guid.d2);
	}
	static std::u8string PrintBool(bool v) {
		return (v ? u8"true" : u8"false");
	}
	static std::u8string PrintColorfulBool(bool v) {
		return (v ? termcolor::colored(u8"Yes", Color::LightGreen) : termcolor::colored(u8"No", Color::LightRed));
	}
	static std::u8string PrintColor(LibCmo::CKDWORD argb) {
		LibCmo::CKDWORD a = (argb & 0xFF000000) >> 24,
			r = (argb & 0x00FF0000) >> 16,
			g = (argb & 0x0000FF00) >> 8,
			b = (argb & 0x000000FF);
		LibCmo::CKFLOAT fa = static_cast<LibCmo::CKFLOAT>(a) / 255.0f;
		return strop::printf(
			u8"A:%" PRIuCKDWORD " (%.4" PRIfCKFLOAT ") "
			"RGB(%" PRIuCKDWORD ", %" PRIuCKDWORD ", %" PRIuCKDWORD ") "
			"RGB#%02" PRIxCKDWORD "%02" PRIxCKDWORD "%02" PRIxCKDWORD " "
			"RGBA#%02" PRIxCKDWORD "%02" PRIxCKDWORD "%02" PRIxCKDWORD "%02" PRIxCKDWORD,
			a, fa,
			r, g, b,
			r, g, b,
			r, g, b, a
		);
	}
	static std::u8string PrintColor(const LibCmo::VxMath::VxColor& col) {
		return PrintColor(col.ToARGB());
	}

#pragma endregion

#pragma region Table Creator

	static tabulate::Tabulate CreateStandardTable(std::initializer_list<std::u8string_view> header) {
		tabulate::Tabulate table(header.size());
		table.show_header(true);
		table.show_bar(false);
		table.set_header(header);
		return table;
	}

	static tabulate::Tabulate CreateGridTable(size_t n) {
		tabulate::Tabulate table(n);
		table.show_header(false);
		table.show_bar(false);
		return table;
	}

#pragma endregion

#pragma region Page Helper

	/**
	 * @brief 
	 * @param pager 
	 * @param page 
	 * @return True if given page index is valid, otherwise false.
	 */
	static bool CheckPageHeader(const PageBreaker& pager, size_t page) {
		bool good_page = pager.IsValidPage(page);
		if (!good_page) {
			termcolor::cprintln(u8"Page out of range.", Color::LightRed);
		}
		return good_page;
	}

	static void WritePageFooter(const PageBreaker& pager, size_t page) {
		std::cout << strop::printf(u8"Page %" PRIuSIZET " of %" PRIuSIZET, page + 1, pager.GetMaxPage() + 1) << std::endl;
	}

#pragma endregion

#pragma region Object Printer

	static void PrintCKObjectDetail(LibCmo::CK2::ObjImpls::CKObject* obj) {
		termcolor::cprintln(u8"CKObject", Color::LightYellow);
		// print name
		std::cout << strop::printf(u8"Name: %s", PrintCKSTRING(obj->GetName()).c_str()) << std::endl;
		// print id
		std::cout << strop::printf(u8"CK ID: %" PRIuCKID, obj->GetID()) << std::endl;
		// print class id
		std::cout << strop::printf(u8"Class ID: %" PRIiCLASSID " (%s)",
		                           obj->GetClassID(),
		                           Docstring::GetClassIdHierarchy(obj->GetClassID()).c_str())
		          << std::endl;
		// print flags
		std::cout << u8"Flags:" << std::endl;
		std::cout << Docstring::GetFlagEnumName(obj->GetObjectFlags(), u8"\n", u8"\t") << std::endl;
	}

	static void PrintCKSceneObjectDetail(LibCmo::CK2::ObjImpls::CKSceneObject* obj) {
		PrintCKObjectDetail(obj);
		termcolor::cprintln(u8"CKSceneObject", Color::LightYellow);
		termcolor::cprintln(u8"No Data", Color::LightRed);
	}

	static void PrintCKBeObjectDetail(LibCmo::CK2::ObjImpls::CKBeObject* obj) {
		PrintCKSceneObjectDetail(obj);
		termcolor::cprintln(u8"CKBeObject", Color::LightYellow);
		termcolor::cprintln(u8"No Data", Color::LightRed);
	}

	static void PrintCKGroupDetail(LibCmo::CK2::ObjImpls::CKGroup* obj) {
		PrintCKBeObjectDetail(obj);
		termcolor::cprintln(u8"CKGroup", Color::LightYellow);

		LibCmo::CKDWORD count = obj->GetObjectCount();
		std::cout << strop::printf(u8"Group Object Count: %" PRIuCKDWORD, count) << std::endl;
		auto group_object_table = CreateStandardTable({
		    u8"CKID",
		    u8"Type",
		    u8"Object",
		    u8"Name",
		});
		for (LibCmo::CKDWORD i = 0; i < count; ++i) {
			LibCmo::CK2::ObjImpls::CKBeObject* beobj = obj->GetObject(i);

			group_object_table.add_row({strop::printf(u8"%" PRIuCKID, beobj->GetID()),
			                            Docstring::GetClassIdName(beobj->GetClassID()),
			                            PrintPointer(beobj),
			                            (beobj != nullptr ? PrintCKSTRING(beobj->GetName()) : u8"")});
		}
		group_object_table.print();
	}

	static void PrintCKRenderObjectDetail(LibCmo::CK2::ObjImpls::CKRenderObject* obj) {
		PrintCKBeObjectDetail(obj);
		termcolor::cprintln(u8"CKRenderObject", Color::LightYellow);
		termcolor::cprintln(u8"No Data", Color::LightRed);
	}

	static void PrintCK3dEntityDetail(LibCmo::CK2::ObjImpls::CK3dEntity* obj) {
		PrintCKRenderObjectDetail(obj);
		termcolor::cprintln(u8"CK3dEntity", Color::LightYellow);

		std::cout << u8"== World Matrix ==" << std::endl;
		const auto& mat = obj->GetWorldMatrix();
		auto matrix_table = CreateGridTable(4);
		for (LibCmo::CKDWORD i = 0; i < 4; ++i) {
			matrix_table.add_row({
			    strop::printf(u8"%+" PRIeCKFLOAT, mat[i].x),
			    strop::printf(u8"%+" PRIeCKFLOAT, mat[i].y),
			    strop::printf(u8"%+" PRIeCKFLOAT, mat[i].z),
			    strop::printf(u8"%+" PRIeCKFLOAT, mat[i].w),
			});
		}
		matrix_table.print();

		std::cout << u8"== Flags ==" << std::endl;
		std::cout << u8"3dEntity Flags:" << std::endl;
		std::cout << Docstring::GetFlagEnumName(obj->GetEntityFlags(), u8"\n", u8"\t") << std::endl;
		std::cout << u8"Moveable Flags:" << std::endl;
		std::cout << Docstring::GetFlagEnumName(obj->GetMoveableFlags(), u8"\n", u8"\t") << std::endl;

		std::cout << u8"== Meshs ==" << std::endl;
		auto mesh_table = CreateStandardTable({
		    u8"Index",
		    u8"Address",
		    u8"Name",
		});
		// print current mesh
		{
			auto curmesh = obj->GetCurrentMesh();
			mesh_table.add_row({u8"->", PrintPointer(curmesh), (curmesh != nullptr ? PrintCKSTRING(curmesh->GetName()) : u8"")});
		}
		// print other meshes
		for (LibCmo::CKDWORD i = 0; i < obj->GetPotentialMeshCount(); ++i) {
			auto thismesh = obj->GetPotentialMesh(i);
			mesh_table.add_row({strop::printf(u8"#%" PRIuCKDWORD, i),
			                    PrintPointer(thismesh),
			                    (thismesh != nullptr ? PrintCKSTRING(thismesh->GetName()) : u8"")});
		}
		mesh_table.print();

	}
	
	static void PrintCKLightDetail(LibCmo::CK2::ObjImpls::CKLight* obj) {
		PrintCK3dEntityDetail(obj);
		termcolor::cprintln(u8"CKLight", Color::LightYellow);
		
		std::cout << u8"== Basics ==" << std::endl;
		std::cout << u8"Type: " << Docstring::GetEnumName(obj->GetType()) << std::endl;
		std::cout << u8"Color: " << PrintColor(obj->GetColor()) << std::endl;
		std::cout << strop::printf(u8"Light Power: %.2" PRIfCKFLOAT, obj->GetLightPower()) << std::endl;
		
		std::cout << u8"== Switches ==" << std::endl;
		std::cout << u8"Activity: " << PrintBool(obj->GetActivity()) << std::endl;
		std::cout << u8"Specular Flag: "<< PrintBool(obj->GetSpecularFlag()) << std::endl;

		std::cout << u8"== Attenuation ==" << std::endl;
		std::cout << strop::printf(u8"Constant Attenuation: %.2" PRIfCKFLOAT, obj->GetConstantAttenuation()) << std::endl;
		std::cout << strop::printf(u8"Linear Attenuation: %.2" PRIfCKFLOAT, obj->GetLinearAttenuation()) << std::endl;
		std::cout << strop::printf(u8"Quadratic Attenuation: %.2" PRIfCKFLOAT, obj->GetQuadraticAttenuation()) << std::endl;
		
		std::cout << u8"== Spot Cone ==" << std::endl;
		std::cout << strop::printf(u8"Range: %.2" PRIfCKFLOAT, obj->GetRange()) << std::endl;
		std::cout << strop::printf(u8"Hot Spot: %.2" PRIfCKFLOAT, obj->GetHotSpot()) << std::endl;
		std::cout << strop::printf(u8"Falloff: %.2" PRIfCKFLOAT, obj->GetFalloff()) << std::endl;
		std::cout << strop::printf(u8"Falloff Shape: %.2" PRIfCKFLOAT, obj->GetFalloffShape()) << std::endl;
		
		std::cout << u8"== Target ==" << std::endl;
		auto thistarget = obj->GetTarget();
		std::cout << u8"Target Address: " << PrintPointer(thistarget) << std::endl;
		if (thistarget != nullptr) {
			std::cout << u8"Target Name: " << PrintCKSTRING(thistarget->GetName()) << std::endl;
		}
	}
	
	static void PrintCKTargetLightDetail(LibCmo::CK2::ObjImpls::CKTargetLight* obj) {
		PrintCKLightDetail(obj);
		termcolor::cprintln(u8"CKTargetLight", Color::LightYellow);
		termcolor::cprintln(u8"No Data", Color::LightRed);
	}
	
	static void PrintCKCameraDetail(LibCmo::CK2::ObjImpls::CKCamera* obj) {
		PrintCK3dEntityDetail(obj);
		termcolor::cprintln(u8"CKCamera", Color::LightYellow);
		
		std::cout << u8"== Basics ==" << std::endl;
		std::cout << u8"Projection Type: " << Docstring::GetEnumName(obj->GetProjectionType()) << std::endl;
		std::cout << strop::printf(u8"Orthographic Zoom: %.2" PRIfCKFLOAT, obj->GetOrthographicZoom()) << std::endl;
		std::cout << strop::printf(u8"Front Plane: %.2" PRIfCKFLOAT, obj->GetFrontPlane()) << std::endl;
		std::cout << strop::printf(u8"Back Plane: %.2" PRIfCKFLOAT, obj->GetBackPlane()) << std::endl;
		std::cout << strop::printf(u8"Fov: %.2" PRIfCKFLOAT, obj->GetFov()) << std::endl;
		LibCmo::CKDWORD width, height;
		obj->GetAspectRatio(width, height);
		std::cout << strop::printf(u8"Aspect Ratio: %" PRIuCKDWORD " : %" PRIuCKDWORD, width, height) << std::endl;
		
		std::cout << u8"== Target ==" << std::endl;
		auto thistarget = obj->GetTarget();
		std::cout << u8"Target Address: " << PrintPointer(thistarget) << std::endl;
		if (thistarget != nullptr) {
			std::cout << u8"Target Name: " << PrintCKSTRING(thistarget->GetName()) << std::endl;
		}
	}

	static void PrintCKTargetCameraDetail(LibCmo::CK2::ObjImpls::CKTargetCamera* obj) {
		PrintCKCameraDetail(obj);
		termcolor::cprintln(u8"CKTargetCamera", Color::LightYellow);
		termcolor::cprintln(u8"No Data", Color::LightRed);
	}

	static void PrintCK3dObjectDetail(LibCmo::CK2::ObjImpls::CK3dObject* obj) {
		PrintCK3dEntityDetail(obj);
		termcolor::cprintln(u8"CK3dObject", Color::LightYellow);
		termcolor::cprintln(u8"No Data", Color::LightRed);
	}

	static void PrintCKTextureDetail(LibCmo::CK2::ObjImpls::CKTexture* obj) {
		PrintCKBeObjectDetail(obj);
		termcolor::cprintln(u8"CKTexture", Color::LightYellow);

		// texture
		std::cout << u8"== Texture ==" << std::endl;

		LibCmo::CK2::CKBitmapData& bd = obj->GetUnderlyingData();
		LibCmo::CKDWORD slot_count = bd.GetSlotCount();
		std::cout << strop::printf(u8"Slot Count: %" PRIuCKDWORD, slot_count) << std::endl;
		std::cout << strop::printf(u8"Current Slot: %" PRIuCKDWORD, bd.GetCurrentSlot()) << std::endl;

		auto image_table = CreateStandardTable({
		    u8"Index",
		    u8"Width",
		    u8"Height",
		    u8"Image Address",
		    u8"Image Size",
		    u8"File Name",
		});
		for (LibCmo::CKDWORD i = 0; i < slot_count; ++i) {
			auto desc = bd.GetImageDesc(i);

			image_table.add_row({
			    strop::printf(u8"#%" PRIuCKDWORD, i),
			    strop::printf(u8"%" PRIuCKDWORD, desc->GetWidth()),
			    strop::printf(u8"%" PRIuCKDWORD, desc->GetHeight()), 
				PrintPointer(desc->GetImage()),
			    strop::printf(u8"0x%" PRIxCKDWORD " bytes", desc->GetImageSize()), 
				PrintCKSTRING(bd.GetSlotFileName(i)),
			});
		}
		image_table.print();

		// other data
		std::cout << u8"== Misc ==" << std::endl;
		std::cout << u8"Video Format: " << Docstring::GetEnumName(obj->GetVideoFormat()) << std::endl;
		std::cout << u8"Save Option: " << Docstring::GetEnumName(bd.GetSaveOptions()) << std::endl;
		std::cout << strop::printf(u8"Pick Threshold: %" PRIuCKDWORD, bd.GetPickThreshold()) << std::endl;
		std::cout << u8"Transparent Color: " << PrintColor(bd.GetTransparentColor()) << std::endl;

		std::cout << u8"Bitmap Flags:" << std::endl;
		std::cout << Docstring::GetFlagEnumName(bd.GetBitmapFlags(), u8"\n", u8"\t") << std::endl;
	}

	static void PrintCKMaterialDetail(LibCmo::CK2::ObjImpls::CKMaterial* obj) {
		PrintCKBeObjectDetail(obj);
		termcolor::cprintln(u8"CKMaterial", Color::LightYellow);

		// color
		std::cout << u8"== Color ==" << std::endl;

		std::cout << u8"Diffuse: " << PrintColor(obj->GetDiffuse()) << std::endl;
		std::cout << u8"Ambient: " << PrintColor(obj->GetAmbient()) << std::endl;
		std::cout << u8"Specular: " << PrintColor(obj->GetSpecular()) << std::endl;
		std::cout << u8"Emissive: " << PrintColor(obj->GetEmissive()) << std::endl;

		std::cout << strop::printf(u8"Specular Power: %.2" PRIfCKFLOAT, obj->GetSpecularPower()) << std::endl;

		// basic data
		std::cout << u8"== Basic ==" << std::endl;
		std::cout << u8"Both Sided: " << PrintBool(obj->GetTwoSidedEnabled()) << std::endl;
		std::cout << u8"Fill Mode: " << Docstring::GetEnumName(obj->GetFillMode()) << std::endl;
		std::cout << u8"Shade Mode: " << Docstring::GetEnumName(obj->GetShadeMode()) << std::endl;

		// textures
		std::cout << u8"== Texture ==" << std::endl;
		std::cout << u8"Textures:" << std::endl;
		auto texture_table = CreateStandardTable({
		    u8"Index",
		    u8"Address",
		    u8"Name",
		});
		for (LibCmo::CKDWORD i = 0; i < 4; ++i) {
			auto tex = obj->GetTexture(i);
			texture_table.add_row(
			    {strop::printf(u8"#%" PRIuCKDWORD, i), PrintPointer(tex), (tex != nullptr ? PrintCKSTRING(tex->GetName()) : u8"")});
		}
		texture_table.print();
		std::cout << u8"Texture Blend: " << Docstring::GetEnumName(obj->GetTextureBlendMode()) << std::endl;
		std::cout << u8"Filter Min: " << Docstring::GetEnumName(obj->GetTextureMinMode()) << std::endl;
		std::cout << u8"Filter Mag: " << Docstring::GetEnumName(obj->GetTextureMagMode()) << std::endl;
		std::cout << u8"Address Mode: " << Docstring::GetEnumName(obj->GetTextureAddressMode()) << std::endl;
		std::cout << u8"Perspective Correct: " << PrintBool(obj->GetPerspectiveCorrectionEnabled()) << std::endl;

		// alpha test
		std::cout << u8"== Alpha Test ==" << std::endl;
		std::cout << u8"Enabled: " << PrintBool(obj->GetAlphaTestEnabled()) << std::endl;
		std::cout << u8"Alpha Function: " << Docstring::GetEnumName(obj->GetAlphaFunc()) << std::endl;
		std::cout << strop::printf(u8"Alpha Ref Value: %" PRIuCKBYTE, obj->GetAlphaRef()) << std::endl;

		// alpha blend
		std::cout << u8"== Alpha Blend ==" << std::endl;
		std::cout << u8"Enabled: " << PrintBool(obj->GetAlphaBlendEnabled()) << std::endl;
		std::cout << u8"Source Blend: " << Docstring::GetEnumName(obj->GetSourceBlend()) << std::endl;
		std::cout << u8"Destination Blend: " << Docstring::GetEnumName(obj->GetDestBlend()) << std::endl;

		// z buffer
		std::cout << u8"== Z-Buffer Write ==" << std::endl;
		std::cout << u8"Enabled: " << PrintBool(obj->GetZWriteEnabled()) << std::endl;
		std::cout << u8"Z Compare Function: " << Docstring::GetEnumName(obj->GetZFunc()) << std::endl;

		// effect
		std::cout << u8"== Effect ==" << std::endl;
		std::cout << u8"Effect: " << Docstring::GetEnumName(obj->GetEffect()) << std::endl;

	}

	static void PrintCKMeshDetail(LibCmo::CK2::ObjImpls::CKMesh* obj) {
		PrintCKBeObjectDetail(obj);
		termcolor::cprintln(u8"CKMesh", Color::LightYellow);

		std::cout << u8"== Flags ==" << std::endl;
		std::cout << u8"Mesh Flags:" << std::endl;
		std::cout << Docstring::GetFlagEnumName(obj->GetMeshFlags(), u8"\n", u8"\t") << std::endl;
		std::cout << u8"Lit Mode: " << Docstring::GetEnumName(obj->GetLitMode()) << std::endl;
		std::cout << u8"Wrap Mode: " << Docstring::GetEnumName(obj->GetWrapMode()) << std::endl;

		// vertex data
		std::cout << u8"== Vertex ==" << std::endl;
		std::cout << strop::printf(u8"Vertex Count: %" PRIuCKDWORD, obj->GetVertexCount()) << std::endl;
		auto vertex_table = CreateStandardTable({
		    u8"Address",
		    u8"Size",
		    u8"Type",
		});
		vertex_table.add_row({
		    PrintPointer(obj->GetVertexPositions()),
		    strop::printf(u8"0x%" PRIxCKDWORD " bytes", obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector3)),
		    u8"Positions",
		});
		vertex_table.add_row({
		    PrintPointer(obj->GetVertexNormals()),
		    strop::printf(u8"0x%" PRIxCKDWORD " bytes", obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector3)),
		    u8"Normals",
		});
		vertex_table.add_row({
		    PrintPointer(obj->GetVertexUVs()),
		    strop::printf(u8"0x%" PRIxCKDWORD " bytes", obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector2)),
		    u8"UVs",
		});
		vertex_table.add_row({
		    PrintPointer(obj->GetVertexColors()),
		    strop::printf(u8"0x%" PRIxCKDWORD " bytes", obj->GetVertexCount() * CKSizeof(LibCmo::CKDWORD)),
		    u8"Colors",
		});
		vertex_table.add_row({
		    PrintPointer(obj->GetVertexSpecularColors()),
		    strop::printf(u8"0x%" PRIxCKDWORD " bytes", obj->GetVertexCount() * CKSizeof(LibCmo::CKDWORD)),
		    u8"Specular Colors",
		});
		vertex_table.print();

		// face data
		std::cout << u8"== Face ==" << std::endl;
		std::cout << strop::printf(u8"Face Count: %" PRIuCKDWORD, obj->GetFaceCount()) << std::endl;
		auto face_table = CreateStandardTable({
		    u8"Address",
		    u8"Size",
		    u8"Type",
		});
		face_table.add_row({
		    PrintPointer(obj->GetFaceIndices()),
		    strop::printf(u8"0x%" PRIxCKDWORD " bytes", obj->GetFaceCount() * 3 * CKSizeof(LibCmo::CKWORD)),
		    u8"Indices",
		});
		face_table.add_row({
		    PrintPointer(obj->GetFaceMaterialSlotIndexs()),
		    strop::printf(u8"0x%" PRIxCKDWORD " bytes", obj->GetFaceCount() * CKSizeof(LibCmo::CKWORD)),
		    u8"Material Slot Indexs",
		});
		face_table.print();

		// mtl slot data
		std::cout << u8"== Material Slot ==" << std::endl;
		LibCmo::CKDWORD slotcount = obj->GetMaterialSlotCount();
		LibCmo::CK2::ObjImpls::CKMaterial** pMtlSlots = obj->GetMaterialSlots();
		std::cout << strop::printf(u8"Material Slot Count: %" PRIuCKDWORD, slotcount) << std::endl;
		auto mtl_table = CreateStandardTable({
		    u8"Index",
		    u8"Address",
		    u8"Name",
		});
		for (LibCmo::CKDWORD i = 0; i < slotcount; ++i) {
			LibCmo::CK2::ObjImpls::CKMaterial* mtl = pMtlSlots[i];
			mtl_table.add_row(
			    {strop::printf(u8"#%" PRIuCKDWORD, i), PrintPointer(mtl), (mtl != nullptr ? PrintCKSTRING(mtl->GetName()) : u8"")});
		}
		mtl_table.print();

	}

#pragma endregion


	void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo) {
		termcolor::cprintln(u8"CKFileInfo", Color::LightYellow);
		std::cout << strop::printf(u8"FileVersion: %" PRIuCKDWORD, fileinfo.FileVersion) << std::endl;
		LibCmo::CKDWORD ck_series[3] {
			(fileinfo.CKVersion >> 24) & 0xFF,
			(fileinfo.CKVersion >> 16) & 0xFF,
			(fileinfo.CKVersion >> 0) & 0xFFFF
		};
		std::cout << strop::printf(u8"CKVersion: %02" PRIxCKDWORD "/%02" PRIxCKDWORD "/%04" PRIxCKDWORD,
		                           ck_series[0],
		                           ck_series[1],
		                           ck_series[2])
		          << std::endl;
		LibCmo::CKDWORD product_series[4] {
			(fileinfo.ProductBuild >> 24) & 0xFF,
			(fileinfo.ProductBuild >> 16) & 0xFF,
			(fileinfo.ProductBuild >> 8) & 0xFF,
			(fileinfo.ProductBuild >> 0) & 0xFF,
		};
		std::cout << strop::printf(u8"Product (Version / Build): %" PRIuCKDWORD " / %" PRIuCKDWORD ".%" PRIuCKDWORD ".%" PRIuCKDWORD ".%" PRIuCKDWORD,
		                           fileinfo.ProductVersion,
		                           product_series[0],
		                           product_series[1],
		                           product_series[2],
		                           product_series[3])
		          << std::endl;

		std::cout << u8"Save Flags: " << Docstring::GetFlagEnumName(fileinfo.FileWriteMode, u8", ") << std::endl;

		std::cout << u8"File Size: " << Docstring::GetReadableFileSize(fileinfo.FileSize) << std::endl;

		std::cout << strop::printf(u8"Crc: 0x%" PRIxCKDWORD, fileinfo.Crc) << std::endl << std::endl;

		std::cout << strop::printf(u8"Hdr1 (Pack / UnPack): %s / %s",
		                           Docstring::GetReadableFileSize(fileinfo.Hdr1PackSize).c_str(),
		                           Docstring::GetReadableFileSize(fileinfo.Hdr1UnPackSize).c_str())
		          << std::endl;

		std::cout << strop::printf(u8"Data (Pack / UnPack): %s / %s",
		                           Docstring::GetReadableFileSize(fileinfo.DataPackSize).c_str(),
		                           Docstring::GetReadableFileSize(fileinfo.DataUnPackSize).c_str())
		          << std::endl
		          << std::endl;


		std::cout << strop::printf(u8"Manager Count: %" PRIuCKDWORD, fileinfo.ManagerCount) << std::endl;
		std::cout << strop::printf(u8"Object Count: %" PRIuCKDWORD, fileinfo.ObjectCount) << std::endl;
		std::cout << strop::printf(u8"Max ID Saved: %" PRIuCKID, fileinfo.MaxIDSaved) << std::endl;

	}

#pragma region Object List Printer

	static tabulate::Tabulate CreateObjectListTable(bool full_detail) {
		if (full_detail) {
			return CreateStandardTable({
			    u8"SaveFlags",
			    u8"Options",
			    u8"CK ID",
			    u8"File CK ID",
			    u8"File Index",
			    u8"Pack Size",
			    u8"Index",
			    u8"Type",
			    u8"CKObject",
			    u8"CKStateChunk",
			    u8"Name",
			});
		} else {
			return CreateStandardTable({
			    u8"Index",
			    u8"Type",
			    u8"Object",
			    u8"Chunk",
			    u8"Name",
			});
		}
	}
	static void FillObjectListTable(tabulate::Tabulate& table,
	                                const LibCmo::CK2::CKFileObject& obj,
	                                const LibCmo::CK2::CKFileInfo& fileinfo,
	                                size_t entry_index,
	                                bool full_detail) {
		// Prepare simple layout data first
		auto col_index = strop::printf(u8"#%" PRIuSIZET, entry_index);
		auto col_type = Docstring::GetClassIdName(obj.ObjectCid);
		auto col_object = PrintColorfulBool(obj.ObjPtr != nullptr);
		auto col_chunk = PrintColorfulBool(obj.Data != nullptr);
		auto col_name = PrintCKSTRING(LibCmo::XContainer::NSXString::ToCKSTRING(obj.Name));

		// Return first if we are simple layout
		if (!full_detail) {
			table.add_row({
			    col_index,
			    col_type,
			    col_object,
			    col_chunk,
			    col_name,
			});
			return;
		}

		// Prepare full detail data
		auto col_save_flag = strop::printf(u8"0x%08" PRIxCKDWORD, obj.SaveFlags);
		auto col_options = Docstring::GetEnumName(obj.Options);
		auto col_ckid = strop::printf(u8"%" PRIuCKID, obj.CreatedObjectId);
		auto col_file_ckid = strop::printf(u8"%" PRIuCKID, obj.ObjectId);
		auto col_file_index = strop::printf(u8"0x%08" PRIxCKDWORD " (RVA: 0x%08" PRIxCKDWORD ")",
		                                    obj.FileIndex,
		                                    obj.FileIndex - CKSizeof(LibCmo::CK2::CKRawFileInfo) - fileinfo.Hdr1UnPackSize);
		auto col_pack_size = strop::printf(u8"0x%08" PRIxCKDWORD, obj.PackSize);

		// Return full info layout
		table.add_row({
		    col_save_flag,
		    col_options,
		    col_ckid,
		    col_file_ckid,
		    col_file_index,
		    col_pack_size,
		    col_index,
		    col_type,
		    col_object,
		    col_chunk,
		    col_name,
		});
	}
	void PrintObjectList(const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls,
	                     const LibCmo::CK2::CKFileInfo& fileinfo,
	                     size_t page,
	                     size_t pageitems,
	                     bool full_detail) {
		termcolor::cprintln(u8"CKFileObject", Color::LightYellow);

		PageBreaker pager(ls.size(), pageitems);
		if (!CheckPageHeader(pager, page)) return;

		auto table = CreateObjectListTable(full_detail);
		for (size_t i = pager.GetPageBeginIndex(page); i < pager.GetPageEndIndex(page); ++i) {
			FillObjectListTable(table, ls[i], fileinfo, i, full_detail);
		}
		table.print();

		WritePageFooter(pager, page);
	}
	void PrintSearchedObjectList(const LibCmo::XContainer::XArray<size_t>& idxls,
	                             const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls,
	                             const LibCmo::CK2::CKFileInfo& fileinfo,
	                             size_t page,
	                             size_t pageitems,
	                             bool full_detail) {
		termcolor::cprintln(u8"CKFileObject Searching Result", Color::LightYellow);

		PageBreaker pager(idxls.size(), pageitems);
		if (!pager.IsValidPage(page)) return;

		auto table = CreateObjectListTable(full_detail);
		for (size_t i = pager.GetPageBeginIndex(page); i < pager.GetPageEndIndex(page); ++i) {
			FillObjectListTable(table, ls[idxls[i]], fileinfo, i, full_detail);
		}
		table.print();

		WritePageFooter(pager, page);
	}

#pragma endregion

#pragma region Manager List Printer

	static tabulate::Tabulate CreateManagerListTable(bool full_detail) {
		// manager list now do not affected by list style because it is enough short
		return CreateStandardTable({
		    u8"Index",
		    u8"CKGUID",
		    u8"CKStateChunk",
		});
	}
	static void FillManagerListTable(tabulate::Tabulate& table, const LibCmo::CK2::CKFileManagerData& mgr, size_t entry_index, bool full_detail) {
		// not affected by list style.
		auto col_index = strop::printf(u8"#%" PRIuSIZET, entry_index);
		auto col_ckguid = PrintCKGUID(mgr.Manager);
		auto col_ckstatechunk = PrintPointer(mgr.Data);
	}
	void PrintManagerList(const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls,
	                      size_t page,
	                      size_t pageitems,
	                      bool full_detail) {
		termcolor::cprintln(u8"CKFileManager", Color::LightYellow);

		PageBreaker pager(ls.size(), pageitems);
		if (!pager.IsValidPage(page)) return;

		auto table = CreateObjectListTable(full_detail);
		for (size_t i = pager.GetPageBeginIndex(page); i < pager.GetPageEndIndex(page); ++i) {
			FillManagerListTable(table, ls[i], i, full_detail);
		}
		table.print();

		WritePageFooter(pager, page);
	}
	void PrintSearchedManagerList(const LibCmo::XContainer::XArray<size_t>& idxls,
	                              const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls,
	                              size_t page,
	                              size_t pageitems,
	                              bool full_detail) {
		termcolor::cprintln(u8"CKFileManager Searching Result", Color::LightYellow);

		PageBreaker pager(ls.size(), pageitems);
		if (!pager.IsValidPage(page)) return;

		auto table = CreateObjectListTable(full_detail);
		for (size_t i = pager.GetPageBeginIndex(page); i < pager.GetPageEndIndex(page); ++i) {
			FillManagerListTable(table, ls[idxls[i]], i, full_detail);
		}
		table.print();

		WritePageFooter(pager, page);
	}

#pragma endregion

	void PrintCKObject(const LibCmo::CK2::ObjImpls::CKObject* obj) {
		if (obj == nullptr) {
			termcolor::cprintln(u8"Null Object", Color::LightRed);
			return;
		}

		LibCmo::CK2::ObjImpls::CKObject* mobj = const_cast<LibCmo::CK2::ObjImpls::CKObject*>(obj);
		switch (mobj->GetClassID()) {
			case LibCmo::CK2::CK_CLASSID::CKCID_OBJECT:
				PrintCKObjectDetail(static_cast<LibCmo::CK2::ObjImpls::CKObject*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_SCENEOBJECT:
				PrintCKSceneObjectDetail(static_cast<LibCmo::CK2::ObjImpls::CKSceneObject*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_BEOBJECT:
				PrintCKBeObjectDetail(static_cast<LibCmo::CK2::ObjImpls::CKBeObject*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_GROUP:
				PrintCKGroupDetail(static_cast<LibCmo::CK2::ObjImpls::CKGroup*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_RENDEROBJECT:
				PrintCKRenderObjectDetail(static_cast<LibCmo::CK2::ObjImpls::CKRenderObject*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_3DENTITY:
				PrintCK3dEntityDetail(static_cast<LibCmo::CK2::ObjImpls::CK3dEntity*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_LIGHT:
				PrintCKLightDetail(static_cast<LibCmo::CK2::ObjImpls::CKLight*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_TARGETLIGHT:
				PrintCKTargetLightDetail(static_cast<LibCmo::CK2::ObjImpls::CKTargetLight*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_CAMERA:
				PrintCKCameraDetail(static_cast<LibCmo::CK2::ObjImpls::CKCamera*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_TARGETCAMERA:
				PrintCKTargetCameraDetail(static_cast<LibCmo::CK2::ObjImpls::CKTargetCamera*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_3DOBJECT:
				PrintCK3dObjectDetail(static_cast<LibCmo::CK2::ObjImpls::CK3dObject*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE:
				PrintCKTextureDetail(static_cast<LibCmo::CK2::ObjImpls::CKTexture*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL:
				PrintCKMaterialDetail(static_cast<LibCmo::CK2::ObjImpls::CKMaterial*>(mobj));
				break;
			case LibCmo::CK2::CK_CLASSID::CKCID_MESH:
				PrintCKMeshDetail(static_cast<LibCmo::CK2::ObjImpls::CKMesh*>(mobj));
				break;
			default:
				termcolor::cprintln(u8"Not Implemented.", Color::LightRed);
				break;
		}

	}

	void PrintCKBaseManager(const LibCmo::CK2::MgrImpls::CKBaseManager* mgr) {
		termcolor::cprintln(u8"CKBaseManager", Color::LightYellow);
		if (mgr == nullptr) {
			termcolor::cprintln(u8"Null Manager", Color::LightRed);
			return;
		}

		termcolor::cprintln(u8"Not Implemented.", Color::LightRed);
	}

	void PrintCKStateChunk(const LibCmo::CK2::CKStateChunk* chunk) {
		termcolor::cprintln(u8"CKStateChunk", Color::LightYellow);
		if (chunk == nullptr) {
			termcolor::cprintln(u8"Null Chunk", Color::LightRed);
			return;
		}

		// hack const state chunk
		LibCmo::CK2::CKStateChunk* operchunk = const_cast<LibCmo::CK2::CKStateChunk*>(chunk);

		// write profile
		const auto profile = operchunk->GetStateChunkProfile();

		std::cout << u8"Type: " << Docstring::GetClassIdName(profile.m_ClassId) << std::endl;

		std::cout << strop::printf(u8"Version (Data / Chunk): %" PRIuCKDWORD " (%s) / %" PRIuCKDWORD " (%s)",
		                           static_cast<LibCmo::CKDWORD>(profile.m_DataVersion),
		                           Docstring::GetEnumName(profile.m_DataVersion).c_str(),
		                           static_cast<LibCmo::CKDWORD>(profile.m_ChunkVersion),
		                           Docstring::GetEnumName(profile.m_ChunkVersion).c_str())
		          << std::endl;
		std::cout << strop::printf(u8"List (Object / Chunk / Manager): %" PRIuCKDWORD " / %" PRIuCKDWORD " / %" PRIuCKDWORD,
		                           static_cast<LibCmo::CKDWORD>(profile.m_ObjectListSize),
		                           static_cast<LibCmo::CKDWORD>(profile.m_ChunkListSize),
		                           static_cast<LibCmo::CKDWORD>(profile.m_ManagerListSize))
		          << std::endl;

		std::cout << strop::printf(u8"Data: %s (0x%" PRIxCKDWORD " DWORD)", PrintPointer(profile.m_pData).c_str(), profile.m_DataDwSize)
		          << std::endl;

		std::cout << u8"Bind CKFile: " << PrintPointer(profile.m_BindFile) << std::endl;
		std::cout << u8"Bind CKContext: " << PrintPointer(profile.m_BindContext) << std::endl;

		// write identifiers
		operchunk->StartRead();
		const auto collection = operchunk->GetIdentifiersProfile();
		operchunk->StopRead();
		termcolor::cprintln(u8"Identifiers", Color::LightYellow);
		auto ident_table = CreateStandardTable({
		    u8"Identifier",
		    u8"Data Address",
		    u8"Data Size",
		});
		for (const auto& ident : collection) {
			ident_table.add_row({
			    strop::printf(u8"0x%08" PRIxCKDWORD, ident.m_Identifier),
			    PrintPointer(ident.m_DataPtr),
			    strop::printf(u8"%" PRIuCKDWORD " (%" PRIuCKDWORD " DWORD + %" PRIuCKDWORD ")",
			                  ident.m_AreaSize,
			                  ident.m_AreaSize / CKSizeof(LibCmo::CKDWORD),
			                  ident.m_AreaSize % CKSizeof(LibCmo::CKDWORD)),
			});
		}
		ident_table.print();
	}

}
