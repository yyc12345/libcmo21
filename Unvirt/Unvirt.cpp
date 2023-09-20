#include "UnvirtContext.hpp"
#include <IronPad.hpp>

int main(int argc, char* argv[]) {

	// start iron pad
	IronPad::IronPadRegister();
	
	Unvirt::Context::UnvirtContext ctx;
	ctx.Run();

	// stop iron pad
	IronPad::IronPadUnregister();

	return 0;
}
