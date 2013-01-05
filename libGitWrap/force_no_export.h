
#pragma once

/*
 * On MSVC, we do a forced include of this file before any other file can include libgit2's public
 * common.h file. We then unconditionally undefine the GIT_EXTERN (which by that time includes a
 * declspec for dll-exports) and redefine it to a simple "extern".
 *
 * This leaves only git methods from git2/common.h publicly exported; but those might turn out to
 * be handy to have, even if no other libgit2 interface shall be accessable.
 */

#include "libgit2/include/git2/common.h"

#ifdef _MSC_VER
# undef GIT_EXTERN
# define GIT_EXTERN(type) extern type
#endif

// Yes, I know this is a fucking kludge...
