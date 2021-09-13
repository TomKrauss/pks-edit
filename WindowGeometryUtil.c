/*
 * WindowGeometryUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: utilities for window coordinate caluclation.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winterf.h"
#include "winfo.h"
#include "editorconfiguration.h"
#include "mouseutil.h"
#include "winutil.h"

/*--------------------------------------------------------------------------
 * win_getWindowSize()
 */
static void win_getWindowSize(HWND hwnd, int *width, int *height)
{	RECT r;

	GetWindowRect(hwnd,&r);
	*height = r.bottom-r.top;
	*width = r.right-r.left;
}

/*--------------------------------------------------------------------------
 * win_moveWindowToXY()
 */
EXPORT void win_moveWindowToXY(HWND hwnd, int meetpoint, int x, int y)
{
	int 	delta,scheight,height,width;

	win_getWindowSize(hwnd,&width,&height);
	scheight = GetSystemMetrics(SM_CYSCREEN);

	if (meetpoint) {		/* x/y is center of formular */
		y -= height / 2;
	} else {				/* position "near" x/y - x/y is not covered */
		delta = scheight / 16;
		if (y > (scheight / 2))
			y = y-delta-height;
		else
			y = y+delta;
	}

	x -= width / 2;
	if (y < 0)
		y = 0;
	if (y > (delta = scheight - height))
		y = delta;
	if (x < 0)
		x = 0;
	if (x > (delta = GetSystemMetrics(SM_CXSCREEN) - width))
		x = delta;
	MoveWindow(hwnd,x,y,width,height,1);
}

/*--------------------------------------------------------------------------
 * win_positionWindowRelativeToCaret()
 * Move a window relative to the caret of the current active window.
 */
EXPORT int win_positionWindowRelativeToCaret(HWND hwnd)
{	
	POINT 	p;
	RECT	r;
	WINFO*	wp = ww_getCurrentEditorWindow();

	if (wp == NULL) {
		return 0;
	}

	GetWindowRect(hwndMain, &r);

	p.y = wp->cy;
	// p.x = wp->cx;
	ClientToScreen(wp->ww_handle,&p);

	win_moveWindowToXY(hwnd,0,(r.left+r.right)/2,p.y);
	return 1;
}

/*--------------------------------------------------------------------------
 * win_moveWindowCloseToMousePosition()
 * Move a window close to the current mouse position.
 */
EXPORT int win_moveWindowCloseToMousePosition(HWND hwnd)
{	int  x = 0, y = 0;

	mouse_getXYPos(GetParent(hwnd),&x,&y);
	win_moveWindowToXY(hwnd,1,x,y);
	return 1;
}

/*--------------------------------------------------------------------------
 * win_centerWindow()
 * Move the window into the center to the screen.
 */
EXPORT int win_centerWindow(HWND hwnd)
{	RECT r;

	GetWindowRect(hwndMain, &r);
	win_moveWindowToXY(hwnd,1,(r.left+r.right)/2,(r.top+r.bottom)/2);
	return 1;
}

/*--------------------------------------------------------------------------
 * win_moveWindowToDefaultPosition()
 * Move a window relative to either the mouse or position it to the center of
 * the screen depending on the configuration.
 */
EXPORT int win_moveWindowToDefaultPosition(HWND hDlg)
{
	if (GetConfiguration()->options & O_FORMFOLLOW)
		return win_moveWindowCloseToMousePosition(hDlg);

	return
		win_centerWindow(hDlg);
}

