
#ifndef EDMALLOC_H

#ifndef EDTYPES_H
#include "edtypes.h"
#endif

void EdUnlock(MEMHANDLE hndlMem);
int EdLock(MEMHANDLE hndlMem);
void EdFree(MEMHANDLE hndlMem);
MEMHANDLE EdMalloc(long nSize);
MEMHANDLE EdRealloc(MEMHANDLE hMem,long nSize);
FARP EdMemAccess(MEMHANDLE hndl);

NEARP EdLocalMalloc(short size);
void EdLocalFree(NEARP mem);

NEARP EdStrAlloc(NEARP string);
void EdStrFree(NEARP mem);

#define EDMALLOC_H
#endif
