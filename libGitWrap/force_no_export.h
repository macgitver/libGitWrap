
#pragma once

#include "libgit2/include/git2/common.h"

#ifdef _MSC_VER
# undef GIT_EXTERN
# define GIT_EXTERN(type) extern type
#endif

// Yes, I know this is a fucking kludge...
