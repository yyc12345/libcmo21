#include "VTUtils.hpp"

// Check YYCC library version
#include <yycc/macro/version_cmp.hpp>
#if YYCC_VERCMP_NE(YYCC_VER_MAJOR, YYCC_VER_MINOR, YYCC_VER_PATCH, 2, 0, 0)
#error "YYCC library version is not matched with our expected version. Please check your library configuration."
#endif
