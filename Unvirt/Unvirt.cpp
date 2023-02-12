#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"
#include "VTStruct.hpp"
#include <cstdio>
#include <iostream>

int main(int argc, char* argv[]) {
	Unvirt::TerminalHelper::EnsureTerminalColor();
	Unvirt::TerminalHelper::EnsureTerminalEncoding();

	LibCmo::Utils::VirtoolsContext vtctx;
	LibCmo::CKFile vtfile(vtctx);
	vtfile.Load("Language.old.nmo", LibCmo::CK_LOAD_FLAGS::CK_LOAD_DEFAULT);

	printf(UNVIRT_TERMCOL_LIGHT_YELLOW(("Hello, %s\n")), u8"㴮!");



	return 0;
}
