/*
 * StatusLine.c
 *
 * Project: PKS Edit for Windows
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
#include <windowsx.h>
#include "dpisupport.h"
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
#include "mouseutil.h"

#define WM_ST_REDRAW		WM_USER+142

#define	MAXSEGMENTS			20

static char	*	pszStatusMessage;
static BOOL		bSimpleMode;
static HWND		hwndStatus;
static int		_redrawsPosted;

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
	union U_ARG_VALUE values[] = {
		{0}
	};
	SPRINTF_ARGS args = (SPRINTF_ARGS){ .sa_wp = wp, .sa_values = values };
	len = mysprintf(dest,
				 (wp && wp->statusline) ? 
				 wp->statusline : 
				 dlg_getResourceString(IDS_STATUSLINE), &args);
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
			if ((pszEnd = strchr(s, '!')) != 0) {
				*pszEnd++ = 0;
			}
			pszStrArr[nSegments++] = s;
			if ((s = pszEnd) == 0) {
				break;
			}
		}
	}
	for (i = 0, offset = 0; i < nSegments; i++) {
		// This size is irrelevant - we will paint the status bar and determine the segments size dynamically
		offset += 10;
		pSegments[i] = offset;
	}
	pSegments[nSegments] = -1;
	SendMessage(hwndStatus, SB_SETPARTS, nSegments + 1, (LPARAM)pSegments);
	if (!bUpdateMessageOnly) {
		for (i = 0; i < nSegments; i++) {
			SendMessage(hwndStatus, SB_SETTEXT, i, (LPARAM)pszStrArr[i]);
		}
	}
	SendMessage(hwndStatus, SB_SETTEXT, nSegments,
			(LPARAM)(pszStatusMessage ? pszStatusMessage : ""));
}


void st_redraw(BOOL bUpdateMessageOnly) {
	if (!_redrawsPosted || _redrawsPosted > 5) {
		PostMessage(hwndStatus, WM_ST_REDRAW, (WPARAM)bUpdateMessageOnly, 0);
	}
	_redrawsPosted++;
}

/*--------------------------------------------------------------------------
 * st_setStatusLineMessage()
 */
void st_setStatusLineMessage(char *msg) {
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
		_redrawsPosted = 0;
		return 0;
	}

	case WM_PAINT: {
		THEME_DATA* pTheme = theme_getCurrent();
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HBRUSH brush = CreateSolidBrush(pTheme->th_dialogBackground);
		FillRect(hdc, &ps.rcPaint, brush);
		DeleteObject(brush);
		SetTextColor(hdc, pTheme->th_dialogForeground);
		HPEN hPen = CreatePen(0, 1, pTheme->th_dialogBorder);
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hPen);
		HFONT hFont = SelectObject(hdc, cust_getSmallEditorFont(hwnd));
		int nSegments = (int) SendMessage(hwndStatus, SB_GETPARTS, 0, 0);
		RECT rect;
		GetClientRect(hwnd, &rect);
		int width = rect.right / 12;
		MoveTo(hdc, rect.left, rect.top );
		LineTo(hdc, rect.right, rect.top);
		int nDelta = dpisupport_getSize(3);
		for (int i = 0; i < nSegments; i++) {
			RECT rc;
			rc.top = 0;
			rc.bottom = rect.bottom;
			rc.left = i * rect.right / 12;
			rc.right = rc.left + width;
			if (rc.left > ps.rcPaint.right) {
				break;
			}
			if (i < nSegments - 1) {
				MoveTo(hdc, rc.right-1, rc.top + 1);
				LineTo(hdc, rc.right-1, rc.bottom - 1);
			}
			char szText[1024];
			SendMessage(hwndStatus, SB_GETTEXT, i, (LPARAM)szText);
			if (!szText[0] || szText[0] == ' ') {
				continue;
			}
			int nFlags = DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER;
			char* pszText = szText;
			if (szText[0] == '\t') {
				nFlags |= DT_CENTER;
				pszText++;
			} else {
				rc.left += nDelta;
			}
			DrawText(hdc, pszText, (int) strlen(pszText), &rc, nFlags);
		}
		WINDOWPLACEMENT placement;
		GetWindowPlacement(hwndMain, &placement);
		if (placement.showCmd != SW_MAXIMIZE) {
			brush = CreateSolidBrush(pTheme->th_dialogBorder);
			int nPix = dpisupport_getSize(2)-1;
			if (nPix < 2) {
				nPix = 2;
			}
			for (int y = 1; y <= nDelta; y++) {
				for (int i = 0; i < nDelta + 1 - y; i++) {
					RECT rc = {
					.left = rect.right - i * nDelta - nDelta,
					.top = rect.bottom - y * nDelta - 1,
					.bottom = rc.top + nPix,
					.right = rc.left + nPix
					};
					FillRect(hdc, &rc, brush);
				}
			}
			DeleteObject(brush);
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

void st_switchtomenumode(BOOL bMenuMode) {
	bSimpleMode = bMenuMode;
}

/*
 * Resize the status line. 
 */
void st_resize(int nStatusHeight, RECT* pRect) {
	ShowWindow(hwndStatus, nStatusHeight > 0 ? SW_SHOW : SW_HIDE);
	MoveWindow(hwndStatus, pRect->left, pRect->bottom - nStatusHeight,
		pRect->right, nStatusHeight, 1);

}
