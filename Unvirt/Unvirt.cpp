#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"
#include "StructFormatter.hpp"

#include "CKMinContext.hpp"
#include "CKFile.hpp"

#include <cstdio>
#include <iostream>

int main(int argc, char* argv[]) {
	Unvirt::TerminalHelper::EnsureTerminalColor();
	Unvirt::TerminalHelper::EnsureTerminalEncoding();

	LibCmo::CK2::CKMinContext vtctx;
	vtctx.SetTempPath("Temp");
	vtctx.SetEncoding("850");

	LibCmo::CK2::CKFile vtfile(&vtctx);
	LibCmo::CK2::CKFileData::DeepDocument* doc;
	LibCmo::CK2::CKERROR err = vtfile.DeepLoad("Level_01.NMO", &doc);

	if (doc)
		Unvirt::StructFormatter::PrintCKFileInfo(doc->m_FileInfo);

	return 0;
}
