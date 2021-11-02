/*
 * StatusLine.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: statusline window
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
#include <commctrl.h>
#include "alloc.h"
#include "pksrc.h"
#include "trace.h"
#include "winterf.h"
#include "documentmodel.h"
#include "winfo.h"
#include "edierror.h"
#include "editorconfiguration.h"
#include "stringutil.h"
#include "customcontrols.h"
#include "themes.h"
#include "xdialog.h"

#define WM_ST_REDRAW		WM_USER+142

#define	MAXSEGMENTS			20

static char	*	pszStatusMessage;
static BOOL		bSimpleMode;
static HWND		hwndStatus;
static BOOL		_redrawPosted;

/*------------------------------------------------------------
 * st_format()
 */
static int st_format(char *dest)
{
	int 	len;
	WINFO *	wp = ww_getCurrentEditorWindow();

	*dest = 0;
	if (!wp) {
		return 0;
	}
	len = mysprintf(wp, dest,
				 (wp && wp->statusline) ? 
				 wp->statusline : 
				 dlg_getResourceString(IDS_STATUSLINE));
	return len;
}

/*------------------------------------------------------------
 * st_setparts()
 */
static void st_setparts(char *text, BOOL bUpdateMessageOnly)
{
	INT					pSegments[MAXSEGMENTS];
	LPSTR				pszStrArr[MAXSEGMENTS];
	LPSTR				s;
	LPSTR				pszEnd;
	int					i;
	int					offset;
	int					nSegments;

	if (!hwndStatus) {
		return;
	}
	if (bSimpleMode) {
		nSegments = 0;
	} else {
		for (nSegments = 0, s = text; ;) {
			if (nSegments >= MAXSEGMENTS-2) {
				break;
			}
			if ((pszEnd = lstrchr(s, '!')) != 0) {
				*pszEnd++ = 0;
			}
			pszStrArr[nSegments++] = s;
			if ((s = pszEnd) == 0) {
				break;
			}
		}
	}
	if (nSegments != SendMessage(hwndStatus, SB_GETPARTS, 0, 0)) {
		for (i = 0, offset = 100; i < nSegments; i++) {
			pSegments[i] = offset;
			offset += 100;
		}
		pSegments[nSegments] = -1;
		SendMessage(hwndStatus, SB_SETPARTS, nSegments + 1, (LPARAM)pSegments);
	}
	if (!bUpdateMessageOnly) {
		for (i = 0; i < nSegments; i++) {
			SendMessage(hwndStatus, SB_SETTEXT, i, (LPARAM)pszStrArr[i]);
		}
	}
	SendMessage(hwndStatus, SB_SETTEXT, nSegments | SBT_NOBORDERS,
			(LPARAM)(pszStatusMessage ? pszStatusMessage : ""));
}


void st_redraw(BOOL bUpdateMessageOnly) {
	if (!_redrawPosted) {
		PostMessage(hwndStatus, WM_ST_REDRAW, (WPARAM)bUpdateMessageOnly, 0);
		_redrawPosted = TRUE;
	}
}

/*--------------------------------------------------------------------------
 * st_seterrmsg()
 */
void st_seterrmsg(char *msg) {
	if (pszStatusMessage) {
		free(pszStatusMessage);
		pszStatusMessage = 0;
	} else if (!msg) {
		return;
	}

	if (msg) {
		pszStatusMessage = _strdup(msg);
	}
	st_redraw(TRUE);
}

/*------------------------------------------------------------
 * status_wh()
 */
void status_wh(WORD *width, WORD *height)
{
	RECT		rect;

	if (hwndStatus == 0 || (GetConfiguration()->layoutoptions & OL_SHOWSTATUS) == 0) {
		*width = 0;
		*height = 0;
		return;
	}
	GetWindowRect(hwndStatus, &rect);
	*width = (WORD)(rect.right - rect.left) + 1;
	*height = (WORD)(rect.bottom - rect.top) + 1;
}

/* 
 * Custom status bar window procedure. 
 */
static WNDPROC _wpOrigStatusWndProc;
LRESULT CALLBACK st_myStatusWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static char	szBuf[1024];

	switch (msg) {
	case WM_ST_REDRAW: {
		st_format(szBuf);
		st_setparts(szBuf, (BOOL)wParam);
		_redrawPosted = FALSE;
		return 0;
	}

	case WM_PAINT: {
		THEME_DATA* pTheme = theme_getCurrent();
		if (pTheme->th_isWinTheme) {
			return CallWindowProc(_wpOrigStatusWndProc, hwnd, msg, wParam, lParam);
		}
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HBRUSH brush = CreateSolidBrush(pTheme->th_dialogBackground);
		FillRect(hdc, &ps.rcPaint, brush);
		DeleteObject(brush);
		SetTextColor(hdc, pTheme->th_dialogForeground);
		HPEN hPen = CreatePen(0, 1, pTheme->th_dialogBorder);
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hPen);
		HFONT hFont = SelectObject(hdc, cust_getSmallEditorFont());
		int nSegments = (int) SendMessage(hwndStatus, SB_GETPARTS, 0, 0);
		for (int i = 0; i < nSegments; i++) {
			RECT rc;
			char szText[128];
			SendMessage(hwndStatus, SB_GETRECT, i, (LPARAM)&rc);
			if (rc.left > ps.rcPaint.right) {
				break;
			}
			SendMessage(hwndStatus, SB_GETTEXT, i, (LPARAM)szText);
			MoveTo(hdc, rc.right, rc.top + 1);
			LineTo(hdc, rc.right, rc.bottom - 1);
			if (!szText[0] || szText[0] == ' ') {
				continue;
			}
			InflateRect(&rc, -4, -2);
			DrawText(hdc, szText, (int) strlen(szText), &rc, DT_END_ELLIPSIS);
		}
		DeleteObject(hPen);
		SelectObject(hdc, hFont);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		free(pszStatusMessage);
		break;
	case WM_ERASEBKGND: {
		THEME_DATA* pTheme = theme_getCurrent();
		if (pTheme->th_isWinTheme) {
			return CallWindowProc(_wpOrigStatusWndProc, hwnd, msg, wParam, lParam);
		}
		return 0;
	}
	}
	return CallWindowProc(_wpOrigStatusWndProc, hwnd, msg, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * st_init()
 */
void st_init(HWND hwndDaddy)
{
	if (!hwndStatus) {
		hwndStatus = CreateStatusWindow(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
			"", hwndDaddy, (UINT)-1);
		_wpOrigStatusWndProc = (WNDPROC)SetWindowLongPtr(hwndStatus, GWLP_WNDPROC, (LONG_PTR)st_myStatusWndProc);

	}
}

/*--------------------------------------------------------------------------
 * st_update()
 */
void st_update(void)
{
	st_redraw(FALSE);
}

void st_switchtomenumode(BOOL bMenuMode) {
	bSimpleMode = bMenuMode;
}

/*
 * Resize the status line. 
 */
void st_resize(int nStatusHeight, RECT* pRect) {
	ShowWindow(hwndStatus, nStatusHeight ? SW_SHOW : SW_HIDE);
	MoveWindow(hwndStatus, pRect->left, pRect->bottom - nStatusHeight,
		pRect->right, nStatusHeight, 1);

}
