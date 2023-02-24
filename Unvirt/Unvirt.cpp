#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"
#include "StructFormatter.hpp"
#include "VTStruct.hpp"
#include <cstdio>
#include <iostream>

int main(int argc, char* argv[]) {
	Unvirt::TerminalHelper::EnsureTerminalColor();
	Unvirt::TerminalHelper::EnsureTerminalEncoding();

	LibCmo::Utils::VirtoolsEnvironment vtctx;
	LibCmo::CKFile vtfile(vtctx);
	LibCmo::CKERROR err = vtfile.Load("vt2obj.nms", LibCmo::CK_LOAD_FLAGS::CK_LOAD_DEFAULT);

	Unvirt::StructFormatter::PrintCKFileInfo(vtfile.m_FileInfo);

	return 0;
}
