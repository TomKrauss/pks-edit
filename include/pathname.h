//
// PATHNAME.H
//
// path name definitions
//

#ifndef PATHNAME_H

// this is for allocating filename buffers
#define EDMAXPATHLEN		1024
#define EDMAXFNLEN			256
#define EDMAXEXTLEN			32
#define EDMAXSUFFIXLEN		64
#define EDMAXDIRLEN			(EDMAXPATHLEN-EDMAXFNLEN)

#define DIRSEPARATOR		'\\'

#if !defined(BOOL)
#include <windows.h>
#endif

extern int EdFsel(char* szFileNameIn, char* szSuffixes, char* szFileNameOut, BOOL bSaveAs);

#define PATHNAME_H
#endif

