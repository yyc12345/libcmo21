#include "UnvirtContext.hpp"
#include <yycc.hpp>
#include <yycc/carton/ironpad.hpp>

int main(int argc, char* argv[]) {
	
	// register exception handler if we are in Windows.
#if defined(LIBCMO_BUILD_RELEASE) && defined(YYCC_OS_WINDOWS)
	yycc::carton::ironpad::startup();
#endif

	// run core
	Unvirt::Context::UnvirtContext ctx;
	ctx.Run();

	// unregister exception handler if we are in Windows
#if defined(LIBCMO_BUILD_RELEASE) && defined(YYCC_OS_WINDOWS)
	yycc::carton::ironpad::shutdown();
#endif

	return 0;
}
