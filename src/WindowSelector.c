/*
 * WindowSelector.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: allows Ctrl+TAB selection of active editor window.
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
#include <windowsx.h>
#include <stdlib.h>
#include "themes.h"
#include "customcontrols.h"
#include "documentmodel.h"
#include "winfo.h"
#include "xdialog.h"
#include "windowselector.h"
#include "dpisupport.h"

#define CLASS_WINDOW_SELECTOR "WindowSelector"
#define GWL_WINDOW_SELECTOR_PARAMS 0
#define WSP_PADDING 2
#define WSP_LINE_HEIGHT LB_ROW_WITH_ICON_HEIGHT

typedef struct tagWINDOW_SELECTOR_PARAMS {
	int wsp_current;
	int wsp_count;
	int wsp_lineHeight;
} WINDOW_SELECTOR_PARAMS;

extern HWND   hwndMain;
extern HINSTANCE hInst;

static HWND hwndSelector;
static WPARAM modifierKey = VK_CONTROL;

 /*
  * Paint the list of windows.
  */
static void windowselector_paint(HWND hwnd) {
	PAINTSTRUCT paint;
	TEXTMETRIC textmetric;
	WINDOW_SELECTOR_PARAMS* pWSP;
	int nLineHeight = dpisupport_getSize(WSP_LINE_HEIGHT);
	int nPadding = dpisupport_getSize(WSP_PADDING);
	THEME_DATA* pTheme = theme_getCurrent();

	pWSP = (WINDOW_SELECTOR_PARAMS*)GetWindowLongPtr(hwnd, GWL_WINDOW_SELECTOR_PARAMS);
	BeginPaint(hwnd, &paint);
	SetBkMode(paint.hdc, TRANSPARENT);
	HFONT hFontNormal = theme_createDialogFont(FW_NORMAL);
	HFONT hFontBold = theme_createDialogFont(FW_BOLD);
	HFONT hFont = SelectObject(paint.hdc, hFontNormal);
	GetTextMetrics(paint.hdc, &textmetric);
	pWSP->wsp_lineHeight = textmetric.tmHeight + nPadding;
	WINFO* wp = ww_getCurrentEditorWindow();
	SetTextColor(paint.hdc, pTheme->th_dialogForeground);
	int y = paint.rcPaint.top + nPadding;
	for (int i = 0; wp; i++) {
		RECT rect;
		GetClientRect(hwnd, &rect);
		rect.left = nPadding;
		rect.top = y;
		rect.bottom = rect.top + nLineHeight;
		BOOL bSelected = i == pWSP->wsp_current;
		if (bSelected) {
			HBRUSH hBrush = CreateSolidBrush(pTheme->th_dialogHighlight);
			FillRect(paint.hdc, &rect, hBrush);
			DeleteObject(hBrush);
		}
		SelectObject(paint.hdc, bSelected ? hFontBold : hFontNormal);
		dlg_drawFileInfo(paint.hdc, &rect, wp->edwin_handle, i, bSelected);
		y += nLineHeight;
		wp = wp->next;
	}

	SelectObject(paint.hdc, hFont);
	DeleteObject(hFontNormal);
	DeleteObject(hFontBold);
	EndPaint(hwnd, &paint);
}

/*
 * Select the window currently selected in the window selector. 
 */
static void windowselector_selectWindow(HWND hwnd) {
	WINDOW_SELECTOR_PARAMS* pWSP = (WINDOW_SELECTOR_PARAMS*)GetWindowLongPtr(hwnd, GWL_WINDOW_SELECTOR_PARAMS);
	int nWindow = pWSP->wsp_current;

	WINFO* wp = ww_getCurrentEditorWindow();
	while (wp) {
		if (--nWindow < 0) {
			ww_selectWindow(wp);
			return;
		}
		wp = wp->next;
	}
}

/*
  * Window procedure of the code completion window.
  */
static LRESULT windowselector_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	WINDOW_SELECTOR_PARAMS* pWSP;
	int nLineHeight = dpisupport_getSize(WSP_LINE_HEIGHT);

	switch (message) {
	case WM_CREATE: 
		pWSP = calloc(1, sizeof * pWSP);
		SetWindowLongPtr(hwnd, GWL_WINDOW_SELECTOR_PARAMS, (LONG_PTR)pWSP);
		break;
	case WM_SHOWWINDOW:
		pWSP = (WINDOW_SELECTOR_PARAMS*)GetWindowLongPtr(hwnd, GWL_WINDOW_SELECTOR_PARAMS);
		pWSP->wsp_count = ww_getNumberOfOpenWindows();
		if (pWSP->wsp_count > 1) {
			pWSP->wsp_current = 1;
		}
		break;
	case WM_ERASEBKGND: {
		THEME_DATA* pTheme = theme_getCurrent();
		HDC hdc = (HDC)wParam;
		RECT rc;
		GetClientRect(hwnd, &rc);
		HBRUSH hbrBg = CreateSolidBrush(pTheme->th_dialogBackground);
		HBRUSH hbrForeground = CreateSolidBrush(pTheme->th_dialogBorder);
		FillRect(hdc, &rc, hbrBg);
		FrameRect(hdc, &rc, hbrForeground);
		DeleteObject(hbrBg);
		DeleteObject(hbrForeground);
		return TRUE;
	}
	case WM_DESTROY:
		pWSP = (WINDOW_SELECTOR_PARAMS*)GetWindowLongPtr(hwnd, GWL_WINDOW_SELECTOR_PARAMS);
		free(pWSP);
		break;
	case WM_PAINT:
		windowselector_paint(hwnd);
		break;
	case WM_LBUTTONDOWN: {
		int nPadding = dpisupport_getSize(WSP_PADDING);
		int y = GET_Y_LPARAM(lParam) - nPadding;
		pWSP = (WINDOW_SELECTOR_PARAMS*)GetWindowLongPtr(hwnd, GWL_WINDOW_SELECTOR_PARAMS);
		pWSP->wsp_current = (y + (nLineHeight / 2)) / nLineHeight;
		windowselector_selectWindow(hwnd);
		ShowWindow(hwnd, SW_HIDE);
	}
	   break;
	case WM_ACTIVATE: 
		if (wParam == WA_INACTIVE) {
			ShowWindow(hwnd, SW_HIDE);
		}
		break;
	case WM_KEYUP: {
		if (wParam == modifierKey || wParam == VK_ESCAPE) {
			windowselector_selectWindow(hwnd);
			ShowWindow(hwnd, SW_HIDE);
		}
	}
	break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam == VK_TAB) {
			pWSP = (WINDOW_SELECTOR_PARAMS*)GetWindowLongPtr(hwnd, GWL_WINDOW_SELECTOR_PARAMS);
			if (modifierKey != VK_SHIFT && (GetKeyState(VK_SHIFT) & 0x8000) != 0) {
				pWSP->wsp_current--;
				if (pWSP->wsp_current < 0) {
					pWSP->wsp_current = pWSP->wsp_count - 1;
				}
			} else {
				pWSP->wsp_current++;
			}
			if (pWSP->wsp_current >= pWSP->wsp_count) {
				pWSP->wsp_current = 0;
			}
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_ERASE);
		}
		return 1;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*------------------------------------------------------------
  * windowselector_registerWindowClass()
  */
int windowselector_registerWindowClass() {
	WNDCLASS  wc;

	cust_initializeWindowClassDefaults(&wc);
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = windowselector_wndProc;
	wc.cbWndExtra = sizeof(WINDOW_SELECTOR_PARAMS*);
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_INFOBK);
	wc.lpszClassName = CLASS_WINDOW_SELECTOR;
	return (RegisterClass(&wc));
}

/*--------------------------------------------------------------------------
 * windowselector_showWindowList()
 * Shows the window selector list.
 */
long long windowselector_showWindowList(void) {
	int nLineHeight = dpisupport_getSize(WSP_LINE_HEIGHT);
	int nPadding = dpisupport_getSize(WSP_PADDING);
	int nWindows = ww_getNumberOfOpenWindows();
	if (nWindows < 1) {
		return 0;
	}
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		modifierKey = VK_CONTROL;
	} else if (GetKeyState(VK_MENU) & 0x8000) {
		modifierKey = VK_MENU;
	} else if (GetKeyState(VK_SHIFT) & 0x8000) {
		modifierKey = VK_SHIFT;
	} else {
		modifierKey = 0;
	}
	RECT rectParent;
	RECT rect;
	GetWindowRect(hwndMain, &rectParent);
	
	int nHeight = nWindows * nLineHeight + (2 * nPadding);
	int nParentHeight = rectParent.bottom - rectParent.top;
	int nParentWidth = rectParent.right - rectParent.left;
	int nWidth = dpisupport_getSize(600);
	rect.top = rectParent.top + (nParentHeight - nHeight) / 2;
	rect.left = rectParent.left + (nParentWidth - nWidth) / 2;
	if (hwndSelector == NULL) {
		hwndSelector = CreateWindow(CLASS_WINDOW_SELECTOR, NULL, WS_POPUP, rect.left, rect.top, nWidth, nHeight, hwndMain, NULL, hInst, NULL);
	}
	HDC hdc = GetWindowDC(hwndSelector);
	HFONT hFontBold = theme_createDialogFont(FW_BOLD);
	HFONT hFontOld = SelectObject(hdc, hFontBold);
	TEXTMETRIC textmetric;
	GetTextMetrics(hdc, &textmetric);
	nWidth = 120 * textmetric.tmAveCharWidth;
	DeleteObject(SelectObject(hdc, hFontOld));
	ReleaseDC(hwndSelector, hdc);
	MoveWindow(hwndSelector, rect.left, rect.top, nWidth, nHeight, TRUE);
	ShowWindow(hwndSelector, SW_SHOW);
	return 1;
}

/*
 * Returns != 0, if the passed HWND is a window handle to be used by the window selector.
 */
BOOL windowselector_isHandle(const HWND hwnd) {
	return hwndSelector == hwnd;
}
