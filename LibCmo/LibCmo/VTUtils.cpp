#include "VTUtils.hpp"

// Check LibCmo build type first.
#if !(defined(LIBCMO_BUILD_DEBUG) ^ defined(LIBCMO_BUILD_RELEASE))
#error "You must define ONE of LIBCMO_BUILD_DEBUG and LIBCMO_BUILD_RELEASE to indicate build type!"
#endif

// Check YYCC library version
#include <yycc/macro/version_cmp.hpp>
#if YYCC_VERCMP_NE(YYCC_VER_MAJOR, YYCC_VER_MINOR, YYCC_VER_PATCH, 2, 0, 0)
#error "YYCC library version is not matched with our expected version. Please check your library configuration."
#endif
