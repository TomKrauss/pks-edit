/*
 * CodeCompletion.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: showing code completion and suggestion windows.
 *
 * 										created: 12.08.21
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "customcontrols.h"
#include "winfo.h"
#include "grammar.h"
#include "linkedlist.h"
#include "crossreferencelinks.h"
#include "codecompletion.h"
#include "themes.h"

#define CLASS_CODE_COMPLETION	"CodeCompletion"
#define CC_WIDTH			150
#define CC_HEIGHT			200

#define CC_PADDING			2

extern HINSTANCE hInst;

typedef enum { CA_TEMPLATE, CA_TAG} CODE_ACTION_TYPE;

typedef struct tagCODE_ACTION {
	struct tagCODE_ACTION* ca_next;
	CODE_ACTION_TYPE ca_type;
	char*	ca_name;
	BOOL ca_replaceWord;
	union {
		UCLIST* template;
	} ca_param;
} CODE_ACTION;

typedef struct tagCODE_COMPLETION_PARAMS {
	WINFO* ccp_wp;
	long   ccp_topRow;
	long   ccp_size;
	long   ccp_pageSize;
	long   ccp_selection;
	CODE_ACTION* cpp_actions;
} CODE_COMPLETION_PARAMS;

#define	GWL_PARAMS			0

static void codecomplete_updateScrollbar(HWND hwnd) {
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	TEXTMETRIC textmetric;
	HDC hdc = GetWindowDC(hwnd);
	RECT rect;
	SCROLLINFO info = { sizeof(info)};

	HFONT hFont = SelectObject(hdc, theme_createSmallFixedFont());
	GetTextMetrics(hdc, &textmetric);
	GetClientRect(hwnd, &rect);
	rect.top += CC_PADDING;
	int nVisibleRows = (rect.bottom - rect.top + textmetric.tmHeight + CC_PADDING - 1) / (textmetric.tmHeight + CC_PADDING);
	int nRows = pCC->ccp_size;
	info.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
	info.nPage = nVisibleRows;
	pCC->ccp_pageSize = nVisibleRows;
	info.nPos = pCC->ccp_topRow;
	info.nMax = nRows;
	info.nMin = 0;
	SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
	DeleteObject(SelectObject(hdc, hFont));
	ReleaseDC(hwnd, hdc);
}

static CODE_ACTION* codecomplete_actionsFor(HWND hwnd) {
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS * )GetWindowLongPtr(hwnd, GWL_PARAMS);
	WINFO* wp = pCC->ccp_wp;
	FTABLE* fp = wp->fp;
	UCLIST* up = grammar_getUndercursorActions(fp->documentDescriptor->grammar);
	CODE_ACTION* pActions = NULL;
	CODE_ACTION* pCurrent;

	if (pCC->cpp_actions) {
		ll_destroy((LINKED_LIST**)&pCC->cpp_actions, NULL);
	}
	while (up) {
		if (up->action == UA_ABBREV) {
			pCurrent = (CODE_ACTION*) ll_insert((LINKED_LIST**) &pActions, sizeof * pActions);
			pCurrent->ca_name = up->pat;
			pCurrent->ca_type = CA_TEMPLATE;
			pCurrent->ca_param.template = up;
			pCurrent->ca_replaceWord = strstr(up->p.uc_template, "${word_selection}") != NULL;
		}
		up = up->next;
	}
	pCC->cpp_actions = pActions;
	pCC->ccp_size = ll_size((LINKED_LIST*)pActions);
	codecomplete_updateScrollbar(hwnd);
	return pActions;
}

/*
 * Paint the list of selectable code action. 
 */
static void codecomplete_paint(HWND hwnd) {
	PAINTSTRUCT paint;
	HICON hIconTemplate = LoadIcon(hInst, "TEMPLATE");
	HICON hIconTag = LoadIcon(hInst, "CROSSREFERENCE");
	TEXTMETRIC textmetric;
	CODE_ACTION* up = codecomplete_actionsFor(hwnd);
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	long nSelectedIndex = pCC->ccp_selection;
	int x = CC_PADDING;
	int y = CC_PADDING;
	int nDelta = CC_PADDING;
	int nIconSize = 16;

	BeginPaint(hwnd, &paint);
	SetBkMode(paint.hdc, TRANSPARENT);
	HFONT hFont = SelectObject(paint.hdc, theme_createSmallFixedFont());
	GetTextMetrics(paint.hdc, &textmetric);
	FillRect(paint.hdc, &paint.rcPaint, GetSysColorBrush(COLOR_WINDOW));
	for(int i = 0; up; i++) {
		if (i >= pCC->ccp_topRow) {
			if (i == nSelectedIndex) {
				RECT rect;
				rect.left = 0;
				rect.right = paint.rcPaint.right;
				rect.top = y;
				rect.bottom = y + textmetric.tmHeight + nDelta;
				FillRect(paint.hdc, &rect, GetSysColorBrush(COLOR_HIGHLIGHT));
				SetTextColor(paint.hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
			else {
				SetTextColor(paint.hdc, GetSysColor(COLOR_WINDOWTEXT));
			}
			char* pszDescription = up->ca_name;
			DrawIconEx(paint.hdc, x, y, up->ca_type == CA_TEMPLATE ? hIconTemplate : hIconTag, textmetric.tmHeight, textmetric.tmHeight, 0, NULL, DI_NORMAL);
			TextOut(paint.hdc, x + nIconSize + 4, y + (nDelta / 2), pszDescription, (int)strlen(pszDescription));
			y += textmetric.tmHeight + nDelta;
		}
		up = up->ca_next;
	}
	DeleteObject(SelectObject(paint.hdc, hFont));
	DeleteObject(hIconTemplate);
	DeleteObject(hIconTag);
	EndPaint(hwnd, &paint);
}

static void codecomplete_moveCaret(HWND hwnd, int nBy) {
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	long nSelectedIndex = pCC->ccp_selection;
	if (nBy < 0) {
		if (nSelectedIndex <= 0) {
			return;
		}
	} else {
		if (nSelectedIndex + nBy >= pCC->ccp_size) {
			return;
		}
	}
	pCC->ccp_selection = nSelectedIndex + nBy;
	if (pCC->ccp_selection < pCC->ccp_topRow) {
		pCC->ccp_topRow = pCC->ccp_selection;
	}
	else if (pCC->ccp_topRow + pCC->ccp_pageSize <= pCC->ccp_selection) {
		pCC->ccp_topRow = pCC->ccp_selection - pCC->ccp_pageSize;
	}
	if (pCC->ccp_topRow < 0) {
		pCC->ccp_topRow = 0;
	}
	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
}

/*
 * Execute the actual action.
 */
static void codecomplete_action(HWND hwnd) {
	CODE_ACTION* cap;
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	long nSelectedIndex = pCC->ccp_selection;
	WINFO* wp = pCC->ccp_wp;
	CODE_ACTION* capFirst = codecomplete_actionsFor(hwnd);

	cap = (CODE_ACTION*) ll_at((LINKED_LIST*)capFirst, nSelectedIndex);
	if (cap && cap->ca_type == CA_TEMPLATE) {
		macro_insertCodeTemplate(wp, cap->ca_param.template, cap->ca_replaceWord);
	}
	ShowWindow(hwnd, SW_HIDE);
}

/*
 * Window procedure of the code completion window. 
 */
static LRESULT codecomplete_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	WINFO* wp;
	CODE_COMPLETION_PARAMS* pCC;

	switch (message) {
		case WM_CREATE: {
			LPCREATESTRUCT cp = (LPCREATESTRUCT)lParam;
			wp = (WINFO*)cp->lpCreateParams;
			pCC = calloc(1, sizeof * pCC);
			pCC->ccp_wp = wp;
			SetWindowLongPtr(hwnd, GWL_PARAMS, (LONG_PTR)pCC);
		}
		break;
		case WM_DESTROY:
			pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
			if (pCC) {
				ll_destroy((LINKED_LIST**)&pCC->cpp_actions, NULL);
				free(pCC);
				SetWindowLongPtr(hwnd, GWL_PARAMS, (LONG_PTR)NULL);
			}
			break;
		case WM_SIZE:
			codecomplete_updateScrollbar(hwnd);
			break;
		case WM_PAINT:
			codecomplete_paint(hwnd);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_UP || wParam == VK_DOWN) {
				codecomplete_moveCaret(hwnd, wParam == VK_UP ? -1 : 1);
			} else if (wParam == VK_ESCAPE) {
				ShowWindow(hwnd, SW_HIDE);
			} else if (wParam == VK_RETURN) {
				codecomplete_action(hwnd);
			}
			return 1;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*------------------------------------------------------------
 * codecomplete_registerWindowClass()
 */
int codecomplete_registerWindowClass() {
	WNDCLASS  wc;

	cust_initializeWindowClassDefaults(&wc);
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = codecomplete_wndProc;
	wc.cbWndExtra = sizeof(CODE_COMPLETION_PARAMS);
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wc.lpszClassName = CLASS_CODE_COMPLETION;
	return (RegisterClass(&wc));
}

/*
 * Calculate the place of the code completion window relative to the caret of the given window. 
 */
static void codecomplete_calculateWindowPos(WINFO* wp, POINT *pPoint, int nHeight) {
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	pPoint->y = wp->cy + wp->cheight + 5;
	pPoint->x = wp->cx - wp->cwidth - 5;
	ClientToScreen(wp->ww_handle, pPoint);
	if (pPoint->y + nHeight > rect.bottom) {
		pPoint->x = wp->cx - wp->cwidth - 5;
		pPoint->y = wp->cy - 5 - nHeight;
		ClientToScreen(wp->ww_handle, pPoint);
	} else if (pPoint->y < rect.top - 5) {
		pPoint->y = rect.top - 5;
	}
}

/*
 * Hide the current suggestion window. 
 */
void codecomplete_hideSuggestionWindow(WINFO* wp) {
	if (wp && wp->codecomplete_handle) {
		ShowWindow(wp->codecomplete_handle, SW_HIDE);
	}
}

 /*--------------------------------------------------------------------------
  * codecomplete_showSuggestionWindow()
  */
int codecomplete_showSuggestionWindow(void) {
	WINFO* wp;
	static unsigned char id;

	wp = ww_getCurrentEditorWindow();
	if (wp == NULL) {
		return 0;
	}
	POINT pt;
	int width = CC_WIDTH;
	int height = CC_HEIGHT;
	if (wp->codecomplete_handle == NULL) {
		codecomplete_calculateWindowPos(wp, &pt, height);
		wp->codecomplete_handle = CreateWindow(CLASS_CODE_COMPLETION, NULL, WS_POPUP|WS_SIZEBOX|WS_VSCROLL, pt.x, pt.y, width, height, wp->ww_handle, NULL, hInst, wp);
	} else {
		RECT rect;
		GetWindowRect(wp->codecomplete_handle, &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		codecomplete_calculateWindowPos(wp, &pt, height);
		MoveWindow(wp->codecomplete_handle, pt.x, pt.y, width, height, TRUE);
	}
	if (!IsWindowVisible(wp->codecomplete_handle)) {
		ShowWindow(wp->codecomplete_handle, SW_SHOWNA);
	}
	return 1;
}


