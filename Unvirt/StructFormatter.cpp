#include "StructFormatter.hpp"

#include <YYCCommonplace.hpp>
#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"

namespace Console = YYCC::ConsoleHelper;

namespace Unvirt::StructFormatter {

#pragma region Assist Functions

	static std::u8string PrintCKSTRING(LibCmo::CKSTRING name) {
		std::u8string ret;
		if (name == nullptr) {
			ret = YYCC_COLOR_LIGHT_MAGENTA(u8"<anonymous>");
		} else {
			if (name[0] == u8'\0') {
				ret = YYCC_COLOR_LIGHT_MAGENTA(u8"<blank>");
			} else {
				ret = name;
			}
		}
		return ret;
	}
	static std::u8string PrintPointer(const void* ptr) {
		std::u8string ret;
		if (ptr == nullptr) {
			ret = YYCC_COLOR_LIGHT_CYAN(u8"<null>");
		} else {
			YYCC::StringHelper::Printf(ret, u8"<0x%" PRI_XPTR_LEFT_PADDING PRIxPTR ">", reinterpret_cast<uintptr_t>(ptr));
		}
		return ret;
	}
	static std::u8string PrintCKGUID(const LibCmo::CK2::CKGUID& guid) {
		return YYCC::StringHelper::Printf(u8"<0x%08" PRIxCKDWORD ", 0x%08" PRIxCKDWORD ">", guid.d1, guid.d2);
	}
	static std::u8string PrintBool(bool v) {
		return (v ? u8"true" : u8"false");
	}
	static std::u8string PrintColorfulBool(bool v) {
		return (v ? YYCC_COLOR_LIGHT_GREEN(u8"Yes") : YYCC_COLOR_LIGHT_RED(u8"No"));
	}
	static std::u8string PrintColor(LibCmo::CKDWORD argb) {
		LibCmo::CKDWORD a = (argb & 0xFF000000) >> 24,
			r = (argb & 0x00FF0000) >> 16,
			g = (argb & 0x0000FF00) >> 8,
			b = (argb & 0x000000FF);
		LibCmo::CKFLOAT fa = static_cast<LibCmo::CKFLOAT>(a) / 255.0f;
		return YYCC::StringHelper::Printf(
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

	template<class _Ty>
	static void GeneralPrintList(
		const std::vector<_Ty>& data, size_t page, size_t pageitems,
		std::function<void()> printHdrFct, std::function<void(size_t, const _Ty&)> printEntryFct) {

		// check page overflow
		if (page * pageitems >= data.size()) {
			Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"Page out of range."));
			return;
		}

		// calc page data
		size_t fulllen = data.size(),
			startpos = page * pageitems,
			fullpage = (fulllen + (pageitems - 1)) / pageitems;	// to solve `fulllen / pageitems` empty page issue. like CKStateChunk::GetCeilDwordSize function (+3 /4 to get DWORD size).

		// print header
		printHdrFct();

		// print body
		for (size_t counter = startpos; counter < fulllen && (counter - startpos) < pageitems; ++counter) {
			printEntryFct(counter, data[counter]);
		}

		Console::FormatLine(u8"Page %" PRIuSIZET " of %" PRIuSIZET, page + 1, fullpage);
	}

#pragma endregion

#pragma region Object Printer

	static void PrintCKObjectDetail(LibCmo::CK2::ObjImpls::CKObject* obj) {
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKObject"));
		// print name
		Console::FormatLine(u8"Name: %s", PrintCKSTRING(obj->GetName()).c_str());
		// print id
		Console::FormatLine(u8"CK ID: %" PRIuCKID, obj->GetID());
		// print class id
		Console::FormatLine(u8"Class ID: %" PRIiCLASSID " (%s)", obj->GetClassID(), AccessibleValue::GetClassIdHierarchy(obj->GetClassID()).c_str());
		// print flags
		Console::WriteLine(u8"Flags:");
		Console::WriteLine(AccessibleValue::GetFlagEnumName(obj->GetObjectFlags(), AccessibleValue::EnumDesc::CK_OBJECT_FLAGS, u8"\n", u8"\t").c_str());
	}

	static void PrintCKSceneObjectDetail(LibCmo::CK2::ObjImpls::CKSceneObject* obj) {
		PrintCKObjectDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKSceneObject"));
		Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"No Data"));
	}

	static void PrintCKBeObjectDetail(LibCmo::CK2::ObjImpls::CKBeObject* obj) {
		PrintCKSceneObjectDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKBeObject"));
		Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"No Data"));
	}

	static void PrintCKGroupDetail(LibCmo::CK2::ObjImpls::CKGroup* obj) {
		PrintCKBeObjectDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKGroup"));

		LibCmo::CKDWORD count = obj->GetObjectCount();
		Console::FormatLine(u8"Group Object Count: %" PRIuCKDWORD, count);
		Console::WriteLine(u8"CKID\tType\tObject\tName");
		for (LibCmo::CKDWORD i = 0; i < count; ++i) {
			LibCmo::CK2::ObjImpls::CKBeObject* beobj = obj->GetObject(i);

			Console::FormatLine(u8"%" PRIuCKID "\t%s\t%s\t%s",
				beobj->GetID(),
				AccessibleValue::GetClassIdName(beobj->GetClassID()).c_str(),
				PrintPointer(beobj).c_str(),
				(beobj != nullptr ? PrintCKSTRING(beobj->GetName()).c_str() : u8"")
			);
		}
	}

	static void PrintCKRenderObjectDetail(LibCmo::CK2::ObjImpls::CKRenderObject* obj) {
		PrintCKBeObjectDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKRenderObject"));
		Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"No Data"));
	}

	static void PrintCK3dEntityDetail(LibCmo::CK2::ObjImpls::CK3dEntity* obj) {
		PrintCKRenderObjectDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CK3dEntity"));

		Console::WriteLine(u8"== World Matrix ==");
		auto mat = obj->GetWorldMatrix();
		for (LibCmo::CKDWORD i = 0; i < 4; ++i) {
			Console::FormatLine(u8"%+" PRIeCKFLOAT "\t%+" PRIeCKFLOAT "\t%+" PRIeCKFLOAT "\t%+" PRIeCKFLOAT,
				mat[i].x, mat[i].y, mat[i].z, mat[i].w
			);
		}

		Console::WriteLine(u8"== Flags ==");
		Console::WriteLine(u8"3dEntity Flags:");
		Console::WriteLine(AccessibleValue::GetFlagEnumName(obj->GetEntityFlags(), AccessibleValue::EnumDesc::CK_3DENTITY_FLAGS, u8"\n", u8"\t").c_str());
		Console::WriteLine(u8"Moveable Flags:");
		Console::WriteLine(AccessibleValue::GetFlagEnumName(obj->GetMoveableFlags(), AccessibleValue::EnumDesc::VX_MOVEABLE_FLAGS, u8"\n", u8"\t").c_str());

		Console::WriteLine(u8"== Meshs ==");
		Console::WriteLine(u8"Index\tAddress\tName");
		// print current mesh
		{
			auto curmesh = obj->GetCurrentMesh();
			Console::Format(u8"->\t%s\t%s",
				PrintPointer(curmesh).c_str(),
				(curmesh != nullptr ? PrintCKSTRING(curmesh->GetName()).c_str() : u8"")
			);
		}
		// print other meshes
		for (LibCmo::CKDWORD i = 0; i < obj->GetPotentialMeshCount(); ++i) {
			auto thismesh = obj->GetPotentialMesh(i);
			Console::FormatLine(u8"#%" PRIuCKDWORD "\t%s\t%s",
				i,
				PrintPointer(thismesh).c_str(),
				(thismesh != nullptr ? PrintCKSTRING(thismesh->GetName()).c_str() : u8"")
			);
		}

	}

	static void PrintCK3dObjectDetail(LibCmo::CK2::ObjImpls::CK3dObject* obj) {
		PrintCK3dEntityDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CK3dObject"));
		Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"No Data"));
	}

	static void PrintCKTextureDetail(LibCmo::CK2::ObjImpls::CKTexture* obj) {
		PrintCKBeObjectDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKTexture"));

		// texture
		Console::WriteLine(u8"== Texture ==");

		LibCmo::CK2::CKBitmapData& bd = obj->GetUnderlyingData();
		LibCmo::CKDWORD slot_count = bd.GetSlotCount();
		Console::FormatLine(u8"Slot Count: %" PRIuCKDWORD, slot_count);
		Console::FormatLine(u8"Current Slot: %" PRIuCKDWORD, bd.GetCurrentSlot());

		Console::WriteLine(u8"Index\tWidth\tHeight\tImage Addr\tImage Size\tFilename");
		for (LibCmo::CKDWORD i = 0; i < slot_count; ++i) {
			auto desc = bd.GetImageDesc(i);

			Console::FormatLine(u8"#%" PRIuCKDWORD "\t%" PRIuCKDWORD "\t%" PRIuCKDWORD "\t0x%" PRIxCKDWORD " bytes\t",
				i,
				desc->GetWidth(),
				desc->GetHeight(),
				PrintPointer(desc->GetImage()).c_str(),
				desc->GetImageSize(),
				PrintCKSTRING(bd.GetSlotFileName(i)).c_str()
			);
		}

		// other data
		Console::WriteLine(u8"== Misc ==");
		Console::FormatLine(u8"Video Format: %s", AccessibleValue::GetEnumName(obj->GetVideoFormat(), AccessibleValue::EnumDesc::VX_PIXELFORMAT).c_str());
		Console::FormatLine(u8"Save Option: %s", AccessibleValue::GetEnumName(bd.GetSaveOptions(), AccessibleValue::EnumDesc::CK_TEXTURE_SAVEOPTIONS).c_str());
		Console::FormatLine(u8"Pick Threshold: %" PRIuCKDWORD, bd.GetPickThreshold());
		Console::FormatLine(u8"Transparent Color: %s", PrintColor(bd.GetTransparentColor()).c_str());

		Console::WriteLine(u8"Bitmap Flags:");
		Console::WriteLine(AccessibleValue::GetFlagEnumName(bd.GetBitmapFlags(), AccessibleValue::EnumDesc::CK_BITMAPDATA_FLAGS, u8"\n", u8"\t").c_str());
	}

	static void PrintCKMaterialDetail(LibCmo::CK2::ObjImpls::CKMaterial* obj) {
		PrintCKBeObjectDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKMaterial"));

		// color
		Console::WriteLine(u8"== Color ==");

		Console::FormatLine(u8"Diffuse: %s", PrintColor(obj->GetDiffuse()).c_str());
		Console::FormatLine(u8"Ambient: %s", PrintColor(obj->GetAmbient()).c_str());
		Console::FormatLine(u8"Specular: %s", PrintColor(obj->GetSpecular()).c_str());
		Console::FormatLine(u8"Emissive: %s", PrintColor(obj->GetEmissive()).c_str());

		Console::FormatLine(u8"Specular Power: %.2" PRIfCKFLOAT, obj->GetSpecularPower());

		// basic data
		Console::WriteLine(u8"== Basic ==");
		Console::FormatLine(u8"Both Sided: %s", PrintBool(obj->GetTwoSidedEnabled()).c_str());
		Console::FormatLine(u8"Fill Mode: %s", AccessibleValue::GetEnumName(obj->GetFillMode(), AccessibleValue::EnumDesc::VXFILL_MODE).c_str());
		Console::FormatLine(u8"Shade Mode: %s", AccessibleValue::GetEnumName(obj->GetShadeMode(), AccessibleValue::EnumDesc::VXSHADE_MODE).c_str());

		// textures
		Console::WriteLine(u8"== Texture ==");
		Console::WriteLine(u8"Textures:");
		Console::WriteLine(u8"Index\tAddress\tName");
		for (LibCmo::CKDWORD i = 0; i < 4; ++i) {
			auto tex = obj->GetTexture(i);
			Console::FormatLine(u8"#%" PRIuCKDWORD "\t%s\t%s",
				i,
				PrintPointer(tex).c_str(),
				(tex != nullptr ? PrintCKSTRING(tex->GetName()).c_str() : u8"")
			);
		}
		Console::FormatLine(u8"Texture Blend: %s", AccessibleValue::GetEnumName(obj->GetTextureBlendMode(), AccessibleValue::EnumDesc::VXTEXTURE_BLENDMODE).c_str());
		Console::FormatLine(u8"Filter Min: %s", AccessibleValue::GetEnumName(obj->GetTextureMinMode(), AccessibleValue::EnumDesc::VXTEXTURE_FILTERMODE).c_str());
		Console::FormatLine(u8"Filter Mag: %s", AccessibleValue::GetEnumName(obj->GetTextureMagMode(), AccessibleValue::EnumDesc::VXTEXTURE_FILTERMODE).c_str());
		Console::FormatLine(u8"Address Mode: %s", AccessibleValue::GetEnumName(obj->GetTextureAddressMode(), AccessibleValue::EnumDesc::VXTEXTURE_ADDRESSMODE).c_str());
		Console::FormatLine(u8"Perspective Correct: %s", PrintBool(obj->GetPerspectiveCorrectionEnabled()).c_str());

		// alpha test
		Console::WriteLine(u8"== Alpha Test ==");
		Console::FormatLine(u8"Enabled: %s", PrintBool(obj->GetAlphaTestEnabled()).c_str());
		Console::FormatLine(u8"Alpha Function: %s", AccessibleValue::GetEnumName(obj->GetAlphaFunc(), AccessibleValue::EnumDesc::VXCMPFUNC).c_str());
		Console::FormatLine(u8"Alpha Ref Value: %" PRIuCKBYTE, obj->GetAlphaRef());

		// alpha blend
		Console::WriteLine(u8"== Alpha Blend ==");
		Console::FormatLine(u8"Enabled: %s", PrintBool(obj->GetAlphaBlendEnabled()).c_str());
		Console::FormatLine(u8"Source Blend: %s", AccessibleValue::GetEnumName(obj->GetSourceBlend(), AccessibleValue::EnumDesc::VXBLEND_MODE).c_str());
		Console::FormatLine(u8"Destination Blend: %s", AccessibleValue::GetEnumName(obj->GetDestBlend(), AccessibleValue::EnumDesc::VXBLEND_MODE).c_str());

		// z buffer
		Console::WriteLine(u8"== Z-Buffer Write ==");
		Console::FormatLine(u8"Enabled: %s", PrintBool(obj->GetZWriteEnabled()).c_str());
		Console::FormatLine(u8"Z Compare Function: %s", AccessibleValue::GetEnumName(obj->GetZFunc(), AccessibleValue::EnumDesc::VXCMPFUNC).c_str());

		// effect
		Console::WriteLine(u8"== Effect ==");
		Console::FormatLine(u8"Effect: %s", AccessibleValue::GetEnumName(obj->GetEffect(), AccessibleValue::EnumDesc::VX_EFFECT).c_str());

	}

	static void PrintCKMeshDetail(LibCmo::CK2::ObjImpls::CKMesh* obj) {
		PrintCKBeObjectDetail(obj);
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKMesh"));

		Console::WriteLine(u8"== Flags ==");
		Console::WriteLine(u8"Mesh Flags:");
		Console::WriteLine(AccessibleValue::GetFlagEnumName(obj->GetMeshFlags(), AccessibleValue::EnumDesc::VXMESH_FLAGS, u8"\n", u8"\t").c_str());
		Console::FormatLine(u8"Lit Mode: %s", AccessibleValue::GetEnumName(obj->GetLitMode(), AccessibleValue::EnumDesc::VXMESH_LITMODE).c_str());
		Console::FormatLine(u8"Wrap Mode: %s", AccessibleValue::GetEnumName(obj->GetWrapMode(), AccessibleValue::EnumDesc::VXTEXTURE_WRAPMODE).c_str());

		// vertex data
		Console::WriteLine(u8"== Vertex ==");
		Console::FormatLine(u8"Vertex Count: %" PRIuCKDWORD, obj->GetVertexCount());
		Console::WriteLine(u8"Address\tSize\tType");

		Console::FormatLine(u8"%s\t0x%" PRIxCKDWORD " bytes\tPositions",
			PrintPointer(obj->GetVertexPositions()).c_str(),
			obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector3)
		);
		Console::FormatLine(u8"%s\t0x%" PRIxCKDWORD " bytes\tNormals",
			PrintPointer(obj->GetVertexNormals()).c_str(),
			obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector3)
		);
		Console::FormatLine(u8"%s\t0x%" PRIxCKDWORD " bytes\tUVs",
			PrintPointer(obj->GetVertexUVs()).c_str(),
			obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector2)
		);
		Console::FormatLine(u8"%s\t0x%" PRIxCKDWORD " bytes\tColors",
			PrintPointer(obj->GetVertexColors()).c_str(),
			obj->GetVertexCount() * CKSizeof(LibCmo::CKDWORD)
		);
		Console::FormatLine(u8"%s\t0x%" PRIxCKDWORD " bytes\tSpecularColors",
			PrintPointer(obj->GetVertexSpecularColors()).c_str(),
			obj->GetVertexCount() * CKSizeof(LibCmo::CKDWORD)
		);

		// face data
		Console::WriteLine(u8"== Face ==");
		Console::FormatLine(u8"Face Count: %" PRIuCKDWORD, obj->GetFaceCount());
		Console::WriteLine(u8"Address\tSize\tType");

		Console::FormatLine(u8"%s\t0x%" PRIxCKDWORD " bytes\tIndices",
			PrintPointer(obj->GetFaceIndices()).c_str(),
			obj->GetFaceCount() * 3 * CKSizeof(LibCmo::CKWORD)
		);
		Console::FormatLine(u8"%s\t0x%" PRIxCKDWORD " bytes\tMaterialSlotIndexs",
			PrintPointer(obj->GetFaceMaterialSlotIndexs()).c_str(),
			obj->GetFaceCount() * CKSizeof(LibCmo::CKWORD)
		);

		// mtl slot data
		Console::WriteLine(u8"== Material Slot ==");
		LibCmo::CKDWORD slotcount = obj->GetMaterialSlotCount();
		LibCmo::CK2::ObjImpls::CKMaterial** pMtlSlots = obj->GetMaterialSlots();
		Console::FormatLine(u8"Material Slot Count: %" PRIuCKDWORD, slotcount);
		Console::WriteLine(u8"Index\tAddress\tName");
		for (LibCmo::CKDWORD i = 0; i < slotcount; ++i) {
			LibCmo::CK2::ObjImpls::CKMaterial* mtl = pMtlSlots[i];

			Console::FormatLine(u8"#%" PRIuCKDWORD "\t%s\t%s",
				i,
				PrintPointer(mtl).c_str(),
				(mtl != nullptr ? PrintCKSTRING(mtl->GetName()).c_str() : u8"")
			);
		}

	}

#pragma endregion


	void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo) {

		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKFileInfo"));
		Console::FormatLine(u8"FileVersion: %" PRIuCKDWORD, fileinfo.FileVersion);
		LibCmo::CKDWORD ck_series[3] {
			(fileinfo.CKVersion >> 24) & 0xFF,
			(fileinfo.CKVersion >> 16) & 0xFF,
			(fileinfo.CKVersion >> 0) & 0xFFFF
		};
		Console::FormatLine(u8"CKVersion: %02" PRIxCKDWORD "/%02" PRIxCKDWORD "/%04" PRIxCKDWORD,
			ck_series[0], ck_series[1], ck_series[2]
		);
		LibCmo::CKDWORD product_series[4] {
			(fileinfo.ProductBuild >> 24) & 0xFF,
			(fileinfo.ProductBuild >> 16) & 0xFF,
			(fileinfo.ProductBuild >> 8) & 0xFF,
			(fileinfo.ProductBuild >> 0) & 0xFF,
		};
		Console::FormatLine(u8"Product (Version / Build): %" PRIuCKDWORD " / %" PRIuCKDWORD ".%" PRIuCKDWORD ".%" PRIuCKDWORD ".%" PRIuCKDWORD,
			fileinfo.ProductVersion, product_series[0], product_series[1], product_series[2], product_series[3]
		);

		Console::FormatLine(u8"Save Flags: %s",
			AccessibleValue::GetFlagEnumName(fileinfo.FileWriteMode, AccessibleValue::EnumDesc::CK_FILE_WRITEMODE, u8", ").c_str()
		);

		Console::FormatLine(u8"File Size: %s", AccessibleValue::GetReadableFileSize(fileinfo.FileSize).c_str());

		Console::FormatLine(u8"Crc: 0x%" PRIxCKDWORD, fileinfo.Crc);
		Console::WriteLine(u8"");


		Console::FormatLine(u8"Hdr1 (Pack / UnPack): %s / %s",
			AccessibleValue::GetReadableFileSize(fileinfo.Hdr1PackSize).c_str(),
			AccessibleValue::GetReadableFileSize(fileinfo.Hdr1UnPackSize).c_str()
		);

		Console::FormatLine(u8"Data (Pack / UnPack): %s / %s",
			AccessibleValue::GetReadableFileSize(fileinfo.DataPackSize).c_str(),
			AccessibleValue::GetReadableFileSize(fileinfo.DataUnPackSize).c_str()
		);
		Console::WriteLine(u8"");


		Console::FormatLine(u8"Manager Count: %" PRIuCKDWORD, fileinfo.ManagerCount);
		Console::FormatLine(u8"Object Count: %" PRIuCKDWORD, fileinfo.ObjectCount);
		Console::FormatLine(u8"Max ID Saved: %" PRIuCKID, fileinfo.MaxIDSaved);

	}

#pragma region Object List Printer

	static void PrintObjectListHeader(bool full_detail) {
		if (full_detail) {
			Console::WriteLine(u8"SaveFlags\tOptions\tCK ID\tFile CK ID\tFile Index\tPack Size\tIndex\tType\tCKObject\tCKStateChunk\tName");
		} else {
			Console::WriteLine(u8"Index\tType\tObject\tChunk\tName");
		}
	}
	static void PrintObjectListEntry(const LibCmo::CK2::CKFileObject& obj, const LibCmo::CK2::CKFileInfo& fileinfo, size_t entry_index, bool full_detail) {
		if (full_detail) {
			Console::Format(u8"0x%08" PRIxCKDWORD "\t", obj.SaveFlags);
			Console::Format(u8"%s\t", AccessibleValue::GetEnumName(obj.Options, AccessibleValue::EnumDesc::CK_FO_OPTIONS).c_str());

			Console::Format(u8"%" PRIuCKID "\t%" PRIuCKID "\t",
				obj.CreatedObjectId,
				obj.ObjectId
			);

			Console::Format(u8"0x%08" PRIxCKDWORD " (Rel: 0x%08" PRIxCKDWORD ")\t",
				obj.FileIndex,
				obj.FileIndex - CKSizeof(LibCmo::CK2::CKRawFileInfo) - fileinfo.Hdr1UnPackSize
			);
			Console::Format(u8"0x%08" PRIxCKDWORD "\t", obj.PackSize);
		}
		// following items are shared by full details and simple layout
		Console::FormatLine(u8"#%" PRIuSIZET "\t%s\t%s\t%s\t%s",
			entry_index,
			AccessibleValue::GetClassIdName(obj.ObjectCid).c_str(),
			PrintColorfulBool(obj.ObjPtr != nullptr).c_str(),
			PrintColorfulBool(obj.Data != nullptr).c_str(),
			PrintCKSTRING(LibCmo::XContainer::NSXString::ToCKSTRING(obj.Name)).c_str()
		);
	}
	void PrintObjectList(
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls,
		const LibCmo::CK2::CKFileInfo& fileinfo,
		size_t page, size_t pageitems,
		bool full_detail) {

		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKFileObject"));
		GeneralPrintList<LibCmo::CK2::CKFileObject>(ls, page, pageitems,
			[full_detail]() -> void {
				PrintObjectListHeader(full_detail);
			},
			[&fileinfo, full_detail](size_t index, const LibCmo::CK2::CKFileObject& obj) -> void {
				PrintObjectListEntry(obj, fileinfo, index, full_detail);
			}
			);

	}
	void PrintSearchedObjectList(
		const LibCmo::XContainer::XArray<size_t>& idxls,
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls,
		const LibCmo::CK2::CKFileInfo& fileinfo,
		size_t page, size_t pageitems,
		bool full_detail) {

		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKFileObject Searching Result"));
		GeneralPrintList<size_t>(idxls, page, pageitems,
			[full_detail]() -> void {
				PrintObjectListHeader(full_detail);
			},
			[&ls, &fileinfo, full_detail](size_t, const size_t& index_to_obj) -> void {
				// resolve index to real item.
				// and pass the entry index, not the page index.
				PrintObjectListEntry(ls[index_to_obj], fileinfo, index_to_obj, full_detail);
			}
			);

	}

#pragma endregion

#pragma region Manager List Printer

	static void PrintManagerListHeader(bool full_detail) {
		// manager list now do not affected by list style because it is enough short
		Console::WriteLine(u8"Index\tCKGUID\tCKStateChunk");
	}
	static void PrintManagerListEntry(const LibCmo::CK2::CKFileManagerData& mgr, size_t entry_index, bool full_detail) {
		// not affected by list style.
		Console::FormatLine(u8"#%" PRIuSIZET "\t%s\t%s",
			entry_index,
			PrintCKGUID(mgr.Manager).c_str(),
			PrintPointer(mgr.Data).c_str()
		);
	}
	void PrintManagerList(
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls,
		size_t page, size_t pageitems,
		bool full_detail) {

		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKFileManager"));
		GeneralPrintList<LibCmo::CK2::CKFileManagerData>(ls, page, pageitems,
			[full_detail]() -> void {
				// print header
				PrintManagerListHeader(full_detail);
			},
			[full_detail](size_t index, const LibCmo::CK2::CKFileManagerData& mgr) -> void {
				// print body
				PrintManagerListEntry(mgr, index, full_detail);
			}
			);

	}
	void PrintSearchedManagerList(
		const LibCmo::XContainer::XArray<size_t>& idxls,
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls,
		size_t page, size_t pageitems,
		bool full_detail) {

		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKFileManager Searching Result"));
		GeneralPrintList<size_t>(idxls, page, pageitems,
			[full_detail]() -> void {
				PrintManagerListHeader(full_detail);
			},
			[&ls, full_detail](size_t, const size_t& index_to_mgr) -> void {
				// resolve index to real one
				PrintManagerListEntry(ls[index_to_mgr], index_to_mgr, full_detail);
			}
			);

	}

#pragma endregion

	void PrintCKObject(const LibCmo::CK2::ObjImpls::CKObject* obj) {
		if (obj == nullptr) {
			Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"Null Object"));
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
				Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"Not Implemented."));
				break;
		}

	}

	void PrintCKBaseManager(const LibCmo::CK2::MgrImpls::CKBaseManager* mgr) {
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKBaseManager"));
		if (mgr == nullptr) {
			Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"Null Manager"));
			return;
		}

		Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"Not Implemented."));
	}

	void PrintCKStateChunk(const LibCmo::CK2::CKStateChunk* chunk) {
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"CKStateChunk"));
		if (chunk == nullptr) {
			Console::WriteLine(YYCC_COLOR_LIGHT_RED(u8"Null Chunk"));
			return;
		}

		// hack const state chunk
		LibCmo::CK2::CKStateChunk* operchunk = const_cast<LibCmo::CK2::CKStateChunk*>(chunk);

		// write profile
		const auto profile = operchunk->GetStateChunkProfile();

		Console::FormatLine(u8"Type: %s", AccessibleValue::GetClassIdName(profile.m_ClassId).c_str());

		Console::FormatLine(u8"Version (Data / Chunk): %" PRIuCKDWORD " (%s) / %" PRIuCKDWORD " (%s)",
			static_cast<LibCmo::CKDWORD>(profile.m_DataVersion), AccessibleValue::GetEnumName(profile.m_DataVersion, AccessibleValue::EnumDesc::CK_STATECHUNK_DATAVERSION).c_str(),
			static_cast<LibCmo::CKDWORD>(profile.m_ChunkVersion), AccessibleValue::GetEnumName(profile.m_ChunkVersion, AccessibleValue::EnumDesc::CK_STATECHUNK_CHUNKVERSION).c_str()
		);
		Console::FormatLine(u8"List (Object / Chunk / Manager): %" PRIuCKDWORD " / %" PRIuCKDWORD " / %" PRIuCKDWORD,
			static_cast<LibCmo::CKDWORD>(profile.m_ObjectListSize),
			static_cast<LibCmo::CKDWORD>(profile.m_ChunkListSize),
			static_cast<LibCmo::CKDWORD>(profile.m_ManagerListSize)
		);

		Console::FormatLine(u8"Data: %s (0x%" PRIxCKDWORD " DWORD)",
			PrintPointer(profile.m_pData).c_str(),
			profile.m_DataDwSize
		);

		Console::FormatLine(u8"Bind CKFile: %s", PrintPointer(profile.m_BindFile).c_str());
		Console::FormatLine(u8"Bind CKContext: %s", PrintPointer(profile.m_BindContext).c_str());

		// write identifiers
		operchunk->StartRead();
		const auto collection = operchunk->GetIdentifiersProfile();
		operchunk->StopRead();
		Console::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"Identifiers"));
		Console::WriteLine(u8"Identifier\tData Pointer\tData Size");
		for (const auto& ident : collection) {
			Console::FormatLine(u8"0x%08" PRIxCKDWORD "\t%s\t%" PRIuCKDWORD " (%" PRIuCKDWORD " DWORD + %" PRIuCKDWORD ")",
				ident.m_Identifier,
				PrintPointer(ident.m_DataPtr).c_str(),
				ident.m_AreaSize,
				ident.m_AreaSize / CKSizeof(LibCmo::CKDWORD),
				ident.m_AreaSize % CKSizeof(LibCmo::CKDWORD)
			);
		}
	}

}
