//
// PATHNAME.H
//
// path name definitions
//

#ifndef PATHNAME_H

// this is for allocating filename buffers
#define EDMAXPATHLEN		2048
#define EDMAXFNLEN			256
#define EDMAXSUFFIXLEN		64
#define EDMAXDIRLEN			(EDMAXPATHLEN-EDMAXFNLEN)

#define DIRSEPARATOR		'\\'

#if !defined(BOOL)
#include <windows.h>
#endif

#define PATHNAME_H
#endif

