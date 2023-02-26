#include "StructFormatter.hpp"
#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"

namespace Unvirt {
	namespace StructFormatter {

		static FILE* fout = stdout;

		void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo) {
			std::string container;

			fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("CKFileInfo\n")), fout);
			fprintf(fout, "Version (File / CK): %" PRIu32 " / %02" PRIX32 "/%02" PRIX32 "/%04" PRIX32 "\n",
				fileinfo.FileVersion, 
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

			Unvirt::AccessibleValue::GetFlagEnumName<LibCmo::CK2::CK_FILE_WRITEMODE>(
				Unvirt::AccessibleValue::EnumDesc::CK_FILE_WRITEMODE, container, fileinfo.FileWriteMode
				);
			fprintf(fout, "Save Flags: %s\n", container.c_str());

			Unvirt::AccessibleValue::GetAccessibleFileSize(container, fileinfo.FileSize);
			fprintf(fout, "File Size: %s\n", container.c_str());

			fprintf(fout, "Crc: 0x%" PRIX32 "\n", fileinfo.Crc);
			fputc('\n', fout);


			fputs("Hdr1 (Pack / UnPack): ", fout);
			Unvirt::AccessibleValue::GetAccessibleFileSize(container, fileinfo.Hdr1PackSize);
			fprintf(fout, "%s / ", container.c_str());
			Unvirt::AccessibleValue::GetAccessibleFileSize(container, fileinfo.Hdr1UnPackSize);
			fprintf(fout, "%s\n", container.c_str());

			fputs("Data (Pack / UnPack): ", fout);
			Unvirt::AccessibleValue::GetAccessibleFileSize(container, fileinfo.DataPackSize);
			fprintf(fout, "%s / ", container.c_str());
			Unvirt::AccessibleValue::GetAccessibleFileSize(container, fileinfo.DataUnPackSize);
			fprintf(fout, "%s\n", container.c_str());
			fputc('\n', fout);


			fprintf(fout, "Manager Count: %" PRIu32 "\nObject Count: %" PRIu32 "\nMax ID Saved: %" PRIu32 "\n",
				fileinfo.ManagerCount, fileinfo.ObjectCount, fileinfo.MaxIDSaved
			);

		}

	}
}
