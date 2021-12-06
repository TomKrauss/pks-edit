#include <windows.h>
#include "alloc.h"
#include "pksedit.h"
#include "winutil.h"

/**
 * Calculate the dimensions of a text and return the size encoded as a LONG.
 */
LONG win_getTextExtent(HDC hdc, char *text, int len) {
	SIZE size;

	GetTextExtentPoint32(hdc, text, len, &size);
	return MAKELONG((SHORT)size.cx, (SHORT)size.cy);
}


/*
 * Set the logical extension of the viewport and return the size encoded as a LONG. 
 */
LONG win_setViewportExtension(HDC hdc, int nx, int ny) {
	SIZE size;
 	SetViewportExtEx(hdc, nx, ny, &size);
	return MAKELONG((SHORT)size.cx, (SHORT)size.cy);
}

/*------------------------------------------------------------
 * win_sendParentCommand()
 */
void win_sendParentCommand(HWND hwnd, LPARAM lParam)
{
	SendMessage(GetParent(hwnd), WM_COMMAND, GetDlgCtrlID(hwnd), lParam);
}

/*--------------------------------------------------------------------------
 * win_sendRedrawToWindow()
 */
void win_sendRedrawToWindow(HWND hwnd)
{
	if (hwnd > 0) {
		UpdateWindow(hwnd);
		InvalidateRect(hwnd, (LPRECT)0, TRUE);
	}
}


/*
 * Set the extension of the window and return the size encoded as a LONG.
 */
LONG win_setWindowExtension(HDC hdc, int nx, int ny) {
	SIZE size;
 	SetWindowExtEx(hdc, nx, ny, &size);
	return MAKELONG((SHORT)size.cx, (SHORT)size.cy);
}

/*
 * Return the extension of the window encoded as a LONG.
 */
LONG win_getWindowExtension(HDC hdc) {
	SIZE size;
 	GetWindowExtEx(hdc, &size);
	return MAKELONG((SHORT)size.cx, (SHORT)size.cy);
}

/*
 * Free the dereferenced pointer and initialize the space pointed to with NULL.
 */
void destroy(void **ap) {
	if (*ap) {
		free(*ap);
		*ap = 0;
	}
}

