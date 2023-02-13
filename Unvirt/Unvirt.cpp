#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"
#include "CmdHelper.hpp"
#include "VTStruct.hpp"
#include <cstdio>
#include <iostream>

int main(int argc, char* argv[]) {
	Unvirt::TerminalHelper::EnsureTerminalColor();
	Unvirt::TerminalHelper::EnsureTerminalEncoding();

	LibCmo::Utils::VirtoolsContext vtctx;
	LibCmo::CKFile vtfile(vtctx);
	vtfile.Load("Language.old.nmo", LibCmo::CK_LOAD_FLAGS::CK_LOAD_DEFAULT);

	//printf(UNVIRT_TERMCOL_LIGHT_YELLOW(("Hello, %s\n")), u8"㴮!");

	//std::string bbb("load \"Language. 㴮!old.nmo\" utf8 ");
	//Unvirt::CmdHelper::CmdSplitter sp;
	//auto a = sp.Convert(bbb);
	//printf(UNVIRT_TERMCOL_LIGHT_YELLOW(("%s\n")), a[1].c_str());

	return 0;
}
