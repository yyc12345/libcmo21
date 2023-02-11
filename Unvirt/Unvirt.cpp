#include "AccessibleValue.hpp"
#include "ColorfulTerminal.hpp"
#include "VTStruct.hpp"
#include <cstdio>

int main(int argc, char* argv[]) {
	Unvirt::ColorfulTerminal::EnsureTerminalColor();
	Unvirt::ColorfulTerminal::EnsureTerminalEncoding();

	LibCmo::Utils::VirtoolsContext vtctx;
	LibCmo::CKFile vtfile(vtctx);
	vtfile.Load("Language.old.nmo", LibCmo::CK_LOAD_FLAGS::CK_LOAD_DEFAULT);

	printf(UNVIRT_TERMCOL_LIGHT_YELLOW(("Hello, %s")), u8"㴮!");

	return 0;
}
