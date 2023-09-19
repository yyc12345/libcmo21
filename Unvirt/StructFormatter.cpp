#include <VTUserAll.hpp>
#include "StructFormatter.hpp"
#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"

namespace Unvirt::StructFormatter {

#pragma region Assist Functions

#define PRIuSIZET "zu"

	static void PrintCKSTRING(LibCmo::CKSTRING name) {
		if (name == nullptr) {
			fputs(UNVIRT_TERMCOL_LIGHT_MAGENTA(("<anonymous>")), stdout);
		} else {
			if (name[0] == '\0') {
				fputs(UNVIRT_TERMCOL_LIGHT_MAGENTA(("<blank>")), stdout);
			} else {
				fputs(name, stdout);
			}
		}
	}
	static void PrintPointer(const void* ptr) {
		if (ptr == nullptr) {
			fputs(UNVIRT_TERMCOL_LIGHT_CYAN(("<null>")), stdout);
		} else {
#if UINTPTR_MAX == UINT32_MAX
			// 32 bit. padding 8 zero
			fprintf(stdout, "<0x%08" PRIxPTR ">", reinterpret_cast<uintptr_t>(ptr));
#else
			// 64 bit. padding 16 zero
			fprintf(stdout, "<0x%016" PRIxPTR ">", reinterpret_cast<uintptr_t>(ptr));
#endif
		}
	}
	static void PrintCKGUID(const LibCmo::CK2::CKGUID& guid) {
		fprintf(stdout, "<0x%08" PRIxCKDWORD ", 0x%08" PRIxCKDWORD ">", guid.d1, guid.d2);
	}

	template<class _Ty>
	static void GeneralPrintList(
		const std::vector<_Ty>& data, size_t page, size_t pageitems,
		std::function<void()> printHdrFct, std::function<void(size_t, const _Ty&)> printEntryFct) {

		size_t fulllen = data.size(),
			startpos = page * pageitems,
			fullpage = fulllen / pageitems;

		// print header
		printHdrFct();

		// print body
		for (size_t counter = startpos; counter < fulllen && (counter - startpos) < pageitems; ++counter) {
			printEntryFct(counter, data[counter]);
		}

		fprintf(stdout, "Page %" PRIuSIZET " of %" PRIuSIZET "\n", page + 1, fullpage + 1);
	}

#pragma endregion

#pragma region Object Printer
	
	static void PrintCKObjectDetail(LibCmo::CK2::ObjImpls::CKObject* obj) {
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKObject\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}
	
	static void PrintCKSceneObjectDetail(LibCmo::CK2::ObjImpls::CKSceneObject* obj) {
		PrintCKObjectDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKSceneObject\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}

	static void PrintCKBeObjectDetail(LibCmo::CK2::ObjImpls::CKBeObject* obj) {
		PrintCKSceneObjectDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKBeObject\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}

	static void PrintCKGroupDetail(LibCmo::CK2::ObjImpls::CKGroup* obj) {
		PrintCKBeObjectDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKGroup\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}
	
	static void PrintCKRenderObjectDetail(LibCmo::CK2::ObjImpls::CKRenderObject* obj) {
		PrintCKBeObjectDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKRenderObject\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}
	
	static void PrintCK3dEntityDetail(LibCmo::CK2::ObjImpls::CK3dEntity* obj) {
		PrintCKRenderObjectDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CK3dEntity\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}
	
	static void PrintCK3dObjectDetail(LibCmo::CK2::ObjImpls::CK3dObject* obj) {
		PrintCK3dEntityDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CK3dObject\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}
	
	static void PrintCKTextureDetail(LibCmo::CK2::ObjImpls::CKTexture* obj) {
		PrintCKBeObjectDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKTexture\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}
	
	static void PrintCKMaterialDetail(LibCmo::CK2::ObjImpls::CKMaterial* obj) {
		PrintCKBeObjectDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKMaterial\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("No Data\n")), stdout);
	}
	
	static void PrintCKMeshDetail(LibCmo::CK2::ObjImpls::CKMesh* obj) {
		PrintCKBeObjectDetail(obj);
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKMesh\n")), stdout);

		fputs("Vertex:\n", stdout);
		fprintf(stdout, "Vertex Count: %" PRIuCKDWORD "\n", obj->GetVertexCount());

		fputs("VertexPositions: ", stdout);
		PrintPointer(obj->GetVertexPositions());
		fprintf(stdout, " (0x%" PRIxCKDWORD " bytes)\n", obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector3));
		fputs("VertexNormals: ", stdout);
		PrintPointer(obj->GetVertexNormals());
		fprintf(stdout, " (0x%" PRIxCKDWORD " bytes)\n", obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector3));
		fputs("VertexUVs: ", stdout);
		PrintPointer(obj->GetVertexUVs());
		fprintf(stdout, " (0x%" PRIxCKDWORD " bytes)\n", obj->GetVertexCount() * CKSizeof(LibCmo::VxMath::VxVector2));
		fputs("VertexColors: ", stdout);
		PrintPointer(obj->GetVertexColors());
		fprintf(stdout, " (0x%" PRIxCKDWORD " bytes)\n", obj->GetVertexCount() * CKSizeof(LibCmo::CKDWORD));
		fputs("VertexSpecularColors: ", stdout);
		PrintPointer(obj->GetVertexSpecularColors());
		fprintf(stdout, " (0x%" PRIxCKDWORD " bytes)\n", obj->GetVertexCount() * CKSizeof(LibCmo::CKDWORD));
		fputs("VertexWeights: ", stdout);
		PrintPointer(obj->GetVertexWeights());
		fprintf(stdout, " (0x%" PRIxCKDWORD " bytes)\n", obj->GetVertexCount() * CKSizeof(LibCmo::CKFLOAT));
		
		fputs("Face:\n", stdout);
		fprintf(stdout, "Face Count: %" PRIuCKDWORD "\n", obj->GetFaceCount());
		
		fputs("FaceIndices: ", stdout);
		PrintPointer(obj->GetFaceIndices());
		fprintf(stdout, " (0x%" PRIxCKDWORD " bytes)\n", obj->GetFaceCount() * 3 * CKSizeof(LibCmo::CKWORD));
		fputs("FaceMaterialSlotIndexs: ", stdout);
		PrintPointer(obj->GetFaceMaterialSlotIndexs());
		fprintf(stdout, " (0x%" PRIxCKDWORD " bytes)\n", obj->GetFaceCount() * CKSizeof(LibCmo::CKWORD));

	}

#pragma endregion


	void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo) {

		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKFileInfo\n")), stdout);
		fprintf(stdout, "FileVersion: %" PRIuCKDWORD "\n", fileinfo.FileVersion);
		fprintf(stdout, "CKVersion: %02" PRIxCKDWORD "/%02" PRIxCKDWORD "/%04" PRIxCKDWORD "\n",
			(fileinfo.CKVersion >> 24) & 0xFF,
			(fileinfo.CKVersion >> 16) & 0xFF,
			(fileinfo.CKVersion >> 0) & 0xFFFF
		);

		LibCmo::CKDWORD product_series[4] {
			(fileinfo.ProductBuild >> 24) & 0xFF,
			(fileinfo.ProductBuild >> 16) & 0xFF,
			(fileinfo.ProductBuild >> 8) & 0xFF,
			(fileinfo.ProductBuild >> 0) & 0xFF,
		};
		fprintf(stdout, "Product (Version / Build): %" PRIuCKDWORD " / %" PRIuCKDWORD ".%" PRIuCKDWORD ".%" PRIuCKDWORD ".%" PRIuCKDWORD "\n",
			fileinfo.ProductVersion, product_series[0], product_series[1], product_series[2], product_series[3]
		);

		fprintf(stdout, "Save Flags: %s\n", AccessibleValue::GetFlagEnumName(
			fileinfo.FileWriteMode, AccessibleValue::EnumDesc::CK_FILE_WRITEMODE
		).c_str());

		fprintf(stdout, "File Size: %s\n", AccessibleValue::GetReadableFileSize(fileinfo.FileSize).c_str());

		fprintf(stdout, "Crc: 0x%" PRIxCKDWORD "\n", fileinfo.Crc);
		fputc('\n', stdout);


		fputs("Hdr1 (Pack / UnPack): ", stdout);
		fprintf(stdout, "%s / ", AccessibleValue::GetReadableFileSize(fileinfo.Hdr1PackSize).c_str());
		fprintf(stdout, "%s\n", AccessibleValue::GetReadableFileSize(fileinfo.Hdr1UnPackSize).c_str());

		fputs("Data (Pack / UnPack): ", stdout);
		fprintf(stdout, "%s / ", AccessibleValue::GetReadableFileSize(fileinfo.DataPackSize).c_str());
		fprintf(stdout, "%s\n", AccessibleValue::GetReadableFileSize(fileinfo.DataUnPackSize).c_str());
		fputc('\n', stdout);


		fprintf(stdout, "Manager Count: %" PRIuCKDWORD "\nObject Count: %" PRIuCKDWORD "\nMax ID Saved: %" PRIuCKDWORD "\n",
			fileinfo.ManagerCount, fileinfo.ObjectCount, fileinfo.MaxIDSaved
		);

	}

	void PrintObjectList(
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls,
		const LibCmo::CK2::CKFileInfo& fileinfo,
		size_t page, size_t pageitems) {

		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKFileObject\n")), stdout);
		GeneralPrintList<LibCmo::CK2::CKFileObject>(ls, page, pageitems,
			[]() -> void {
				fputs("SaveFlags\tOptions\tCK ID\tFile CK ID\tFile Index\tPack Size\tIndex\tType\tCKObject\tCKStateChunk\tName\n", stdout);
			},
			[&fileinfo](size_t index, const LibCmo::CK2::CKFileObject& obj) -> void {
				fprintf(stdout, "0x%08" PRIxCKDWORD "\t", obj.SaveFlags);
				fputs(AccessibleValue::GetEnumName(obj.Options, AccessibleValue::EnumDesc::CK_FO_OPTIONS).c_str(), stdout);
				fputc('\t', stdout);

				fprintf(stdout, "%" PRIuCKID "\t", obj.CreatedObjectId);
				fprintf(stdout, "%" PRIuCKID "\t", obj.ObjectId);

				fprintf(stdout, "0x%08" PRIxCKDWORD " (Rel: 0x%08" PRIxCKDWORD ")\t",
					obj.FileIndex,
					obj.FileIndex - CKSizeof(LibCmo::CK2::CKRawFileInfo) - fileinfo.Hdr1UnPackSize);
				fprintf(stdout, "0x%08" PRIxCKDWORD "\t", obj.PackSize);

				fprintf(stdout, "#%" PRIuSIZET "\t", index);
				fputs(AccessibleValue::GetClassIdName(obj.ObjectCid).c_str(), stdout);
				fputc('\t', stdout);
				PrintPointer(obj.ObjPtr);
				fputc('\t', stdout);
				PrintPointer(obj.Data);
				fputc('\t', stdout);
				PrintCKSTRING(LibCmo::XContainer::NSXString::ToCKSTRING(obj.Name));
				fputc('\n', stdout);
			}
			);

	}

	void PrintManagerList(
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls,
		size_t page, size_t pageitems) {

		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKFileManager\n")), stdout);
		GeneralPrintList<LibCmo::CK2::CKFileManagerData>(ls, page, pageitems,
			[]() -> void {
				// print header
				fputs("Index\tCKGUID\tCKStateChunk\n", stdout);
			},
			[](size_t index, const LibCmo::CK2::CKFileManagerData& mgr) -> void {
				// print body
				fprintf(stdout, "#%" PRIuSIZET "\t", index);
				PrintCKGUID(mgr.Manager);
				fputc('\t', stdout);
				PrintPointer(mgr.Data);
				fputc('\n', stdout);
			}
			);

	}

	void PrintCKObject(const LibCmo::CK2::ObjImpls::CKObject* obj) {
		if (obj == nullptr) {
			fputs(UNVIRT_TERMCOL_LIGHT_RED(("Null Object\n")), stdout);
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
				fputs(UNVIRT_TERMCOL_LIGHT_RED(("Not Implemented.\n")), stdout);
				break;
		}

	}
	void PrintCKBaseManager(const LibCmo::CK2::MgrImpls::CKBaseManager* mgr) {
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKBaseManager\n")), stdout);
		if (mgr == nullptr) {
			fputs(UNVIRT_TERMCOL_LIGHT_RED(("Null Manager\n")), stdout);
			return;
		}

		fputs(UNVIRT_TERMCOL_LIGHT_RED(("Not Implemented.\n")), stdout);
	}
	void PrintCKStateChunk(const LibCmo::CK2::CKStateChunk* chunk) {
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKStateChunk\n")), stdout);
		if (chunk == nullptr) {
			fputs(UNVIRT_TERMCOL_LIGHT_RED(("Null Chunk\n")), stdout);
			return;
		}

		// hack const state chunk
		LibCmo::CK2::CKStateChunk* operchunk = const_cast<LibCmo::CK2::CKStateChunk*>(chunk);

		// write profile
		const auto profile = operchunk->GetStateChunkProfile();

		fputs("Type: ", stdout);
		fputs(AccessibleValue::GetClassIdName(profile.m_ClassId).c_str(), stdout);
		fputc('\n', stdout);

		fprintf(stdout, "Version (Data / Chunk): %" PRIuCKDWORD " (%s) / %" PRIuCKDWORD " (%s)\n",
			static_cast<LibCmo::CKDWORD>(profile.m_DataVersion), AccessibleValue::GetEnumName(profile.m_DataVersion, AccessibleValue::EnumDesc::CK_STATECHUNK_DATAVERSION).c_str(),
			static_cast<LibCmo::CKDWORD>(profile.m_ChunkVersion), AccessibleValue::GetEnumName(profile.m_ChunkVersion, AccessibleValue::EnumDesc::CK_STATECHUNK_CHUNKVERSION).c_str());
		fprintf(stdout, "List (Object / Chunk / Manager): %" PRIuCKDWORD " / %" PRIuCKDWORD " / %" PRIuCKDWORD "\n",
			static_cast<LibCmo::CKDWORD>(profile.m_ObjectListSize),
			static_cast<LibCmo::CKDWORD>(profile.m_ChunkListSize),
			static_cast<LibCmo::CKDWORD>(profile.m_ManagerListSize));

		fputs("Data: ", stdout);
		PrintPointer(profile.m_pData);
		fprintf(stdout, " (0x%" PRIxCKDWORD " DWORD)\n", profile.m_DataDwSize);

		fputs("Bind CKFile: ", stdout);
		PrintPointer(profile.m_BindFile);
		fputc('\n', stdout);

		fputs("Bind CKContext: ", stdout);
		PrintPointer(profile.m_BindContext);
		fputc('\n', stdout);

		// write identifiers
		operchunk->StartRead();
		const auto collection = operchunk->GetIdentifiersProfile();
		operchunk->StopRead();
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("Identifiers\n")), stdout);
		fputs("Identifier\tData Pointer\tData Size\n", stdout);
		for (const auto& ident : collection) {
			fprintf(stdout, "0x%08" PRIxCKDWORD "\t", ident.m_Identifier);
			PrintPointer(ident.m_DataPtr);
			fputc('\t', stdout);
			fprintf(stdout, "%" PRIuCKDWORD " (%" PRIuCKDWORD " DWORD + %" PRIuCKDWORD ")\n",
				ident.m_AreaSize, ident.m_AreaSize / CKSizeof(LibCmo::CKDWORD), ident.m_AreaSize % CKSizeof(LibCmo::CKDWORD));
		}
	}

}
