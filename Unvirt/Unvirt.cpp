#include "AccessibleValue.hpp"
#include "VTStruct.hpp"
#include <cstdio>

int main(int argc, char* argv[]) {
	LibCmo::Utils::VirtoolsContext vtctx;
	LibCmo::CKFile vtfile(vtctx);
	vtfile.Load("Language.old.nmo", LibCmo::CK_LOAD_FLAGS::CK_LOAD_DEFAULT);

	return 0;
}
