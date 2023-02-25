#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"
#include "StructFormatter.hpp"
#include "CKFile.hpp"
#include <cstdio>
#include <iostream>

int main(int argc, char* argv[]) {
	Unvirt::TerminalHelper::EnsureTerminalColor();
	Unvirt::TerminalHelper::EnsureTerminalEncoding();

	LibCmo::CKMinContext vtctx;
	LibCmo::CKFile vtfile(&vtctx);
	LibCmo::CKFileData::ShallowDocument* doc;
	LibCmo::CKERROR err = vtfile.ShallowLoad("Language.old.nmo", &doc);

	Unvirt::StructFormatter::PrintCKFileInfo(doc->m_FileInfo);

	return 0;
}
