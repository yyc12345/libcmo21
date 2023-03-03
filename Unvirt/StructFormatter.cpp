#include "StructFormatter.hpp"
#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"

namespace Unvirt {
	namespace StructFormatter {

		static FILE* fout = stdout;

		void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo) {

			fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKFileInfo\n")), fout);
			fprintf(fout, "FileVersion: %" PRIu32 "\n", fileinfo.FileVersion);
			fprintf(fout, "CKVersion: %02" PRIX32 "/%02" PRIX32 "/%04" PRIX32 "\n",
				(fileinfo.CKVersion >> 24) & 0xFF,
				(fileinfo.CKVersion >> 16) & 0xFF,
				(fileinfo.CKVersion >> 0) & 0xFFFF
			);

			LibCmo::CK2::CKDWORD product_series[4]{
				(fileinfo.ProductBuild >> 24) & 0xFF,
				(fileinfo.ProductBuild >> 16) & 0xFF,
				(fileinfo.ProductBuild >> 8) & 0xFF,
				(fileinfo.ProductBuild >> 0) & 0xFF,
			};
			fprintf(fout, "Product (Version / Build): %" PRIu32 " / %" PRIu32 ".%" PRIu32 ".%" PRIu32 ".%" PRIu32 "\n",
				fileinfo.ProductVersion, product_series[0], product_series[1], product_series[2], product_series[3]
			);

			fprintf(fout, "Save Flags: %s\n", Unvirt::AccessibleValue::GetFlagEnumName<LibCmo::CK2::CK_FILE_WRITEMODE>(
				Unvirt::AccessibleValue::EnumDesc::CK_FILE_WRITEMODE, fileinfo.FileWriteMode
			).c_str());

			fprintf(fout, "File Size: %s\n", Unvirt::AccessibleValue::GetAccessibleFileSize(fileinfo.FileSize).c_str());

			fprintf(fout, "Crc: 0x%" PRIX32 "\n", fileinfo.Crc);
			fputc('\n', fout);


			fputs("Hdr1 (Pack / UnPack): ", fout);
			fprintf(fout, "%s / ", Unvirt::AccessibleValue::GetAccessibleFileSize(fileinfo.Hdr1PackSize).c_str());
			fprintf(fout, "%s\n", Unvirt::AccessibleValue::GetAccessibleFileSize(fileinfo.Hdr1UnPackSize).c_str());

			fputs("Data (Pack / UnPack): ", fout);
			fprintf(fout, "%s / ", Unvirt::AccessibleValue::GetAccessibleFileSize(fileinfo.DataPackSize).c_str());
			fprintf(fout, "%s\n", Unvirt::AccessibleValue::GetAccessibleFileSize(fileinfo.DataUnPackSize).c_str());
			fputc('\n', fout);


			fprintf(fout, "Manager Count: %" PRIu32 "\nObject Count: %" PRIu32 "\nMax ID Saved: %" PRIu32 "\n",
				fileinfo.ManagerCount, fileinfo.ObjectCount, fileinfo.MaxIDSaved
			);

		}

	}
}
