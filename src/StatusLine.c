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
#include "dpisupport.h"
#include "fontawesome.h"

#define WM_ST_REDRAW		WM_USER+142

#define	MAXSEGMENTS			20
#define ICON_SIZE			14

struct tagICONS {
	CHAR_WITH_STYLE ic_char;
	COLORREF        ic_color;
	HBITMAP			ic_bitmap;
};

static char	*	pszStatusMessage;
static BOOL		bSimpleMode;
static HWND		hwndStatus;
static int		_redrawsPosted;
static struct tagICONS _cachedIcons[MAXSEGMENTS];

/*------------------------------------------------------------
 * st_setparts()
 */
static int st_setparts(BOOL bUpdateMessageOnly, int pSegments[MAXSEGMENTS]) {
	static				char szBuf[1024];
	LPSTR				pszStrArr[MAXSEGMENTS];
	LPSTR				pszStart = szBuf;
	int					nSegments = 0;

	if (!hwndStatus) {
		return 0;
	}
	memset(pSegments, 0, MAXSEGMENTS * sizeof(int));
	memset(pszStrArr, 0, sizeof pszStrArr);
	if (!bSimpleMode) {
		faicon_loadFontAwesome();
		WINFO* wp = ww_getCurrentEditorWindow();
		if (!wp) {
			return 1;
		}
		STATUS_LINE_SEGMENT* pSegment = wp->statuslineSegments;
		union U_ARG_VALUE values[] = {
			{0}
		};
		SPRINTF_ARGS args = (SPRINTF_ARGS){ .sa_wp = wp, .sa_values = values };
		char* pszLocale = config_getLocale();
		while (pSegment) {
			struct tagICONS* pIcon = &_cachedIcons[nSegments];
			CHAR_WITH_STYLE c = { 0 };
			if (pSegment->sls_icon != NULL) {
				c = faicon_codeForName(pSegment->sls_icon);
			}
			COLORREF color = pSegment->sls_iconColor == RGB(0,0,0) ? theme_getCurrent()->th_dialogBorder : pSegment->sls_iconColor;
			CHAR_WITH_STYLE cExisting = pIcon->ic_char;
			if (pIcon->ic_bitmap != NULL && (c.symbol == 0 || c.symbol != cExisting.symbol || color != pIcon->ic_color)) {
				DeleteObject(pIcon->ic_bitmap);
				pIcon->ic_bitmap = NULL;
			}
			if (c.symbol != 0 && pIcon->ic_bitmap == NULL) {
				pIcon->ic_char = c;
				pIcon->ic_color = color;
				int nSize = dpisupport_getSize(ICON_SIZE);
				pIcon->ic_bitmap = faicon_createAwesomeIcons(pIcon->ic_color, nSize, nSize, &c, 1);
			}
			if (pSegment->sls_text == NULL || (pSegment->sls_lang[0] && strcmp(pSegment->sls_lang, pszLocale) != 0)) {
				pSegment = pSegment->sls_next;
				continue;
			}
			int len = mysprintf(pszStart, pSegment->sls_text, &args);
			pszStrArr[nSegments] = pszStart;
			pSegments[nSegments] = pSegment->sls_width ? pSegment->sls_width : 1;
			pszStart += len;
			*pszStart++ = 0;
			nSegments++;
			if (nSegments >= MAXSEGMENTS - 2) {
				break;
			}
			pSegment = pSegment->sls_next;
		}
	}
	pSegments[nSegments++] = -1;
	if (!bUpdateMessageOnly) {
		for (int i = 0; i < nSegments; i++) {
			SendMessage(hwndStatus, SB_SETTEXT, i, (LPARAM)pszStrArr[i]);
			struct tagICONS* pIcon = &_cachedIcons[i];
			SendMessage(hwndStatus, SB_SETICON, i, (LPARAM) pIcon->ic_bitmap);
		}
	}
	SendMessage(hwndStatus, SB_SETPARTS, nSegments, (LPARAM) pSegments);
	if (pszStatusMessage && nSegments == 1) {
		SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)pszStatusMessage);
	}
	return nSegments;
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
	static int segments;
	static int segmentSizes[MAXSEGMENTS];

	switch (msg) {
	case WM_ST_REDRAW: {
		segments = st_setparts((BOOL)wParam, segmentSizes);
		return 0;
	}

	case WM_PAINT: {
		THEME_DATA* pTheme = theme_getCurrent();
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HBRUSH brush = CreateSolidBrush(pTheme->th_dialogBackground);
		FillRect(hdc, &ps.rcPaint, brush);
		DeleteObject(brush);
		SetTextColor(hdc, pTheme->th_dialogBorder);
		HPEN hPen = CreatePen(0, 1, pTheme->th_dialogBorder);
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hPen);
		HFONT hFont = SelectObject(hdc, cust_getSmallEditorFont(hwnd));
		int nSegments = segments;
		if (nSegments <= 0) {
			break;
		}
		RECT rect;
		GetClientRect(hwnd, &rect);
		int nDelta = dpisupport_getSize(3);
		int nTotal = dpisupport_getSegmentCount(&rect, 12);
		int nSegmentOffset = 0;
		for (int i = 0; i < nSegments; i++) {
			INT nFactor = segmentSizes[i];
			RECT rc = {
				.top = 0,
				.bottom = rect.bottom
			};
			rc.left = nSegmentOffset * rect.right / nTotal;
			if (rc.left > ps.rcPaint.right) {
				break;
			}
			if (nSegmentOffset >= nTotal - 1 || nFactor < 0) {
				rc.right = rect.right;
			}
			else {
				rc.right = (nSegmentOffset + nFactor) * rect.right / nTotal;
				nSegmentOffset += nFactor;
			}
			if (rc.right < ps.rcPaint.left) {
				break;
			}
			char szText[1024];
			szText[0] = 0;
			SendMessage(hwndStatus, SB_GETTEXT, i, (LPARAM)szText);
			int nFlags = DT_SINGLELINE | DT_END_ELLIPSIS | DT_VCENTER;
			char* pszText = szText;
			if (szText[0] == '\t') {
				nFlags |= DT_CENTER;
				pszText++;
			}
			else {
				rc.left += nDelta;
			}
			if (!pszText[0] || pszText[0] == ' ') {
				continue;
			}
			HBITMAP hBmp = (HBITMAP)SendMessage(hwndStatus, SB_GETICON, i, (LPARAM)0);
			if (hBmp != NULL) {
				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP oldBitmap = SelectObject(hdcMem, hBmp);
				int nWidth = dpisupport_getSize(ICON_SIZE);
				int nPadding = dpisupport_getSize(3);
				SetStretchBltMode(hdc, COLORONCOLOR);
				TransparentBlt(hdc, rc.left+ nPadding, (rc.bottom- rc.top - nWidth)/2, nWidth, nWidth, hdcMem, 0, 0, nWidth, nWidth, RGB(0,0,0));
				SelectObject(hdcMem, oldBitmap);
				DeleteDC(hdcMem);
				rc.left += nPadding * 2 + nWidth;
			}
			DrawText(hdc, pszText, (int) strlen(pszText), &rc, nFlags);
		}
		WINDOWPLACEMENT placement = {
			.length = sizeof(placement)
		};
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
					.top = rect.bottom - y * nDelta - 1
					};
					rc.bottom = rc.top + nPix;
					rc.right = rc.left + nPix;
					FillRect(hdc, &rc, brush);
				}
			}
			DeleteObject(brush);
		}
		DeleteObject(hPen);
		SelectObject(hdc, hFont);
		EndPaint(hwnd, &ps);
		_redrawsPosted = 0;
		return 0;
	}
	case WM_DESTROY:
		free(pszStatusMessage);
		break;
	case WM_ERASEBKGND: 
		return 0;
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
