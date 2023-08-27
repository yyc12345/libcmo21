#include "UnvirtContext.hpp"

int main(int argc, char* argv[]) {

	//LibCmo::CK2::CKMinContext vtctx;
	//vtctx.SetTempPath("Temp");
	//vtctx.SetEncoding("850");

	//LibCmo::CK2::CKFile vtfile(&vtctx);
	//LibCmo::CK2::CKFileDocument* doc;
	//LibCmo::CK2::CKERROR err = vtfile.DeepLoad("Level_02.NMO", &doc);

	//if (doc)
	//	Unvirt::StructFormatter::PrintCKFileInfo(doc->m_FileInfo);
	Unvirt::Context::UnvirtContext ctx;
	ctx.Run();

	return 0;
}
