#include <windows.h>
#include <malloc.h>
#include "pksedit.h"

LONG GetTextExtent(HDC hdc, char *text, int len) {
	SIZE size;

	GetTextExtentPoint32(hdc, text, len, &size);
	return MAKELONG((SHORT)size.cx, (SHORT)size.cy);
}


LONG SetViewportExt(HDC hdc, int nx, int ny) {
	SIZE size;
 	SetViewportExtEx(hdc, nx, ny, &size);
	return MAKELONG((SHORT)size.cx, (SHORT)size.cy);
}


LONG SetWindowExt(HDC hdc, int nx, int ny) {
	SIZE size;
 	SetWindowExtEx(hdc, nx, ny, &size);
	return MAKELONG((SHORT)size.cx, (SHORT)size.cy);
}

LONG GetWindowExt(HDC hdc) {
	SIZE size;
 	GetWindowExtEx(hdc, &size);
	return MAKELONG((SHORT)size.cx, (SHORT)size.cy);
}

/*
 * Free the dereferenced pointer and initialize the space pointed to with NULL.
 */
void destroy(char **ap) {
	if (*ap) {
		free(*ap);
		*ap = 0;
	}
}

#if 0
void *shareAlloc(void) {	
	static char *szRet;
	
	if (!szRet) {
		HANDLE handle;

		if ((handle = GlobalAlloc(GMEM_FIXED|GMEM_DDESHARE,256)) != 0)
			szRet = GlobalLock(handle);
		if (!szRet)
			nomemory();
	}
	return szRet;
}
#endif
