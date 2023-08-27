#include <VTAll.hpp>
#include <CK2/CKStateChunk.hpp>
#include "StructFormatter.hpp"
#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"

namespace Unvirt::StructFormatter {

#pragma region Assist Functions

	static void PrintCKSTRING(const LibCmo::TypeHelper::MKString& name) {
		LibCmo::CK2::CKSTRING ckname = name.c_str();
		if (ckname == nullptr) {
			fputs(UNVIRT_TERMCOL_LIGHT_MAGENTA(("<anonymous>")), stdout);
		} else {
			fputs(ckname, stdout);
		}
	}
	static void PrintPointer(const void* ptr) {
		if (ptr == nullptr) {
			fputs(UNVIRT_TERMCOL_LIGHT_CYAN(("<null>")), stdout);
		} else {
#if UINTPTR_MAX == UINT32_MAX
			// 32 bit. padding 8 zero
			fprintf(stdout, "<0x%08" PRIXPTR ">", reinterpret_cast<uintptr_t>(ptr));
#else
			// 64 bit. padding 16 zero
			fprintf(stdout, "<0x%016" PRIXPTR ">", reinterpret_cast<uintptr_t>(ptr));
#endif
		}
	}
	static void PrintCKGUID(const LibCmo::CK2::CKGUID& guid) {
		fprintf(stdout, "<0x%08" PRIx32 ", 0x%08" PRIx32 ">", guid.d1, guid.d2);
	}
	static void PrintIdentifier(const LibCmo::CK2::CKDWORD val) {
		fprintf(stdout, "0x%08" PRIx32, val);
	}
	
	template<class _Ty>
	static void GeneralPrintList(
		const std::vector<_Ty>& data, size_t page, size_t pageitems,
		std::function<void()> printHdrFct, std::function<void(size_t,const _Ty&)> printEntryFct) {

		size_t fulllen = data.size(),
			startpos = page * pageitems,
			fullpage = fulllen / pageitems;

		// print header
		printHdrFct();

		// print body
		for (size_t counter = startpos; counter < fulllen && (counter - startpos) < pageitems; ++counter) {
			printEntryFct(counter, data[counter]);
		}

		fprintf(stdout, "Page %zu of %zu\n", page + 1, fullpage + 1);
	}

#pragma endregion

	void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo) {

		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKFileInfo\n")), stdout);
		fprintf(stdout, "FileVersion: %" PRIu32 "\n", fileinfo.FileVersion);
		fprintf(stdout, "CKVersion: %02" PRIX32 "/%02" PRIX32 "/%04" PRIX32 "\n",
			(fileinfo.CKVersion >> 24) & 0xFF,
			(fileinfo.CKVersion >> 16) & 0xFF,
			(fileinfo.CKVersion >> 0) & 0xFFFF
		);

		LibCmo::CK2::CKDWORD product_series[4] {
			(fileinfo.ProductBuild >> 24) & 0xFF,
			(fileinfo.ProductBuild >> 16) & 0xFF,
			(fileinfo.ProductBuild >> 8) & 0xFF,
			(fileinfo.ProductBuild >> 0) & 0xFF,
		};
		fprintf(stdout, "Product (Version / Build): %" PRIu32 " / %" PRIu32 ".%" PRIu32 ".%" PRIu32 ".%" PRIu32 "\n",
			fileinfo.ProductVersion, product_series[0], product_series[1], product_series[2], product_series[3]
		);

		fprintf(stdout, "Save Flags: %s\n", Unvirt::AccessibleValue::GetFlagEnumName(
			fileinfo.FileWriteMode, Unvirt::AccessibleValue::EnumDesc::CK_FILE_WRITEMODE
		).c_str());

		fprintf(stdout, "File Size: %s\n", Unvirt::AccessibleValue::GetReadableFileSize(fileinfo.FileSize).c_str());

		fprintf(stdout, "Crc: 0x%" PRIX32 "\n", fileinfo.Crc);
		fputc('\n', stdout);


		fputs("Hdr1 (Pack / UnPack): ", stdout);
		fprintf(stdout, "%s / ", Unvirt::AccessibleValue::GetReadableFileSize(fileinfo.Hdr1PackSize).c_str());
		fprintf(stdout, "%s\n", Unvirt::AccessibleValue::GetReadableFileSize(fileinfo.Hdr1UnPackSize).c_str());

		fputs("Data (Pack / UnPack): ", stdout);
		fprintf(stdout, "%s / ", Unvirt::AccessibleValue::GetReadableFileSize(fileinfo.DataPackSize).c_str());
		fprintf(stdout, "%s\n", Unvirt::AccessibleValue::GetReadableFileSize(fileinfo.DataUnPackSize).c_str());
		fputc('\n', stdout);


		fprintf(stdout, "Manager Count: %" PRIu32 "\nObject Count: %" PRIu32 "\nMax ID Saved: %" PRIu32 "\n",
			fileinfo.ManagerCount, fileinfo.ObjectCount, fileinfo.MaxIDSaved
		);

	}

	void PrintObjectList(const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls, size_t page, size_t pageitems) {

		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKFileObject\n")), stdout);
		GeneralPrintList<LibCmo::CK2::CKFileObject>(ls, page, pageitems,
			[]() -> void {
				fputs("Index\tThis CK ID\tFile CK ID\tType\tCKObject\tCKStateChunk\tName\n", stdout);
			},
			[](size_t index, const LibCmo::CK2::CKFileObject& obj) -> void {
				fprintf(stdout, "#%zu\t", index);
				fprintf(stdout, "%" PRIu32 "\t%" PRIu32 "\t", obj.CreatedObjectId, obj.ObjectId);
				fprintf(stdout, "%s\t", Unvirt::AccessibleValue::GetClassIdName(obj.ObjectCid).c_str());
				PrintPointer(obj.ObjPtr);
				fputc('\t', stdout);
				PrintPointer(obj.Data);
				fputc('\t', stdout);
				PrintCKSTRING(obj.Name);
				fputc('\n', stdout);
			}
		);

	}

	void PrintManagerList(const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls, size_t page, size_t pageitems) {

		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKFileManager\n")), stdout);
		GeneralPrintList<LibCmo::CK2::CKFileManagerData>(ls, page, pageitems,
			[]() -> void {
				// print header
				fputs("Index\tCKGUID\tCKStateChunk\n", stdout);
			},
			[](size_t index, const LibCmo::CK2::CKFileManagerData& mgr) -> void {
				// print body
				fprintf(stdout, "#%zu\t", index);
				PrintCKGUID(mgr.Manager);
				fputc('\t', stdout);
				PrintPointer(mgr.Data);
				fputc('\n', stdout);
			}
		);

	}

	void PrintCKObject(const LibCmo::CK2::ObjImpls::CKObject* obj) {
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKObject\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("Not Implemented.\n")), stdout);
	}
	void PrintCKBaseManager(const LibCmo::CK2::MgrImpls::CKBaseManager* mgr) {
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKBaseManager\n")), stdout);
		fputs(UNVIRT_TERMCOL_LIGHT_RED(("Not Implemented.\n")), stdout);
	}
	void PrintCKStateChunk(const LibCmo::CK2::CKStateChunk* chunk) {
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKStateChunk\n")), stdout);
		
		// print header
		fputs("Identifier\tData Pointer\tData Size\n", stdout);
		
		// print body
		LibCmo::CK2::CKStateChunk* operchunk = const_cast<LibCmo::CK2::CKStateChunk*>(chunk);
		operchunk->StartRead();
		auto collection = operchunk->GetIdentifierProfile();
		operchunk->StopRead();
		for (const auto& profile : collection) {
			PrintIdentifier(profile.m_Identifier);
			fputc('\t', stdout);
			PrintPointer(profile.m_DataPtr);
			fputc('\t', stdout);
			fprintf(stdout, "%" PRIckDWORD "\n", profile.m_AreaSize);
		}
	}

}
