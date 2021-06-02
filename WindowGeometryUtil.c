/*
 * WindowGeometryUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: utilities for window coordinate caluclation.
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winterf.h"
#include "winfo.h"
#include "editorconfiguration.h"

/*--------------------------------------------------------------------------
 * GetWindowWH()
 */
static void GetWindowWH(HWND hwnd, int *width, int *height)
{	RECT r;

	GetWindowRect(hwnd,&r);
	*height = r.bottom-r.top;
	*width = r.right-r.left;
}

/*--------------------------------------------------------------------------
 * form_xy()
 */
EXPORT void form_xy(HWND hwnd, int meetpoint, int x, int y)
{
	int 	delta,scheight,height,width;

	GetWindowWH(hwnd,&width,&height);
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
 * form_textcursor()
 */
EXPORT int form_textcursor(HWND hwnd)
{	
	POINT 	p;
	WINFO 	*wp;
	RECT		r;

	if (!ft_CurrentDocument())
		return 0;

	wp = WIPOI(ft_CurrentDocument());

	GetWindowRect(hwndFrame, &r);

	p.y = wp->cy;
	// p.x = wp->cx;
	ClientToScreen(wp->ww_handle,&p);

	form_xy(hwnd,0,(r.left+r.right)/2,p.y);
	return 1;
}

/*--------------------------------------------------------------------------
 * form_mouse()
 */
EXPORT int form_mouse(HWND hwnd)
{	int  x = 0, y = 0;

	MouseGetXYPos(GetParent(hwnd),&x,&y);
	form_xy(hwnd,1,x,y);
	return 1;
}

/*--------------------------------------------------------------------------
 * form_center()
 */
EXPORT int form_center(HWND hwnd)
{	RECT r;

	GetWindowRect(hwndFrame, &r);
	form_xy(hwnd,1,(r.left+r.right)/2,(r.top+r.bottom)/2);
	return 1;
}

/*--------------------------------------------------------------------------
 * form_move()
 */
EXPORT int form_move(HWND hDlg)
{
	if (GetConfiguration()->options & O_FORMFOLLOW)
		return form_mouse(hDlg);

	return
		form_center(hDlg);
}

