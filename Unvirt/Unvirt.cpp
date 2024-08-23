#include <YYCCommonplace.hpp>
#include "UnvirtContext.hpp"

int main(int argc, char* argv[]) {
	
	// register exception handler on windows release
#if defined(LIBCMO_BUILD_RELEASE) && (YYCC_OS == YYCC_OS_WINDOWS)
	YYCC::ExceptionHelper::Register();
#endif
	
	// run core
	Unvirt::Context::UnvirtContext ctx;
	ctx.Run();

	// unregister exception handler on windows release
#if defined(LIBCMO_BUILD_RELEASE) && (YYCC_OS == YYCC_OS_WINDOWS)
	YYCC::ExceptionHelper::Register();
#endif

	return 0;
}
