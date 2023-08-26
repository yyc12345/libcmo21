#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"
#include "StructFormatter.hpp"
#include "CmdHelper.hpp"

#include <VTAll.hpp>
#include <CK2/CKContext.hpp>
#include <CK2/CKFile.hpp>

#include <cstdio>
#include <iostream>

namespace Unvirt {

	class UnvirtContext {
	public:
		UnvirtContext() {}
		~UnvirtContext() {}

		void Run() {
			Unvirt::TerminalHelper::EnsureTerminalColor();
			Unvirt::TerminalHelper::EnsureTerminalEncoding();

		}
	private:

	};

}

int main(int argc, char* argv[]) {

	//LibCmo::CK2::CKMinContext vtctx;
	//vtctx.SetTempPath("Temp");
	//vtctx.SetEncoding("850");

	//LibCmo::CK2::CKFile vtfile(&vtctx);
	//LibCmo::CK2::CKFileDocument* doc;
	//LibCmo::CK2::CKERROR err = vtfile.DeepLoad("Level_02.NMO", &doc);

	//if (doc)
	//	Unvirt::StructFormatter::PrintCKFileInfo(doc->m_FileInfo);
	Unvirt::UnvirtContext ctx;
	ctx.Run();

	return 0;
}
