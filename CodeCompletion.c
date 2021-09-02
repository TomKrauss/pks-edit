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
#include <windowsx.h>
#include "customcontrols.h"
#include "winfo.h"
#include "stringutil.h"
#include "arraylist.h"
#include "hashmap.h"
#include "winterf.h"
#include "grammar.h"
#include "pksrc.h"
#include "linkedlist.h"
#include "crossreferencelinks.h"
#include "codecompletion.h"
#include "themes.h"
#include "xdialog.h"
#include "codeanalyzer.h"

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
	BOOL ca_freeName;
	union {
		UCLIST* template;
		unsigned char* text;
	} ca_param;
} CODE_ACTION;

typedef struct tagCODE_COMPLETION_PARAMS {
	WINFO* ccp_wp;
	int    ccp_lineHeight;
	long   ccp_topRow;
	long   ccp_size;
	long   ccp_pageSize;
	long   ccp_selection;
	CODE_ACTION* ccp_actions;
} CODE_COMPLETION_PARAMS;

#define	GWL_PARAMS			0

static void codecomplete_updateScrollbar(HWND hwnd) {
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	if (pCC->ccp_lineHeight == 0) {
		return;
	}
	HDC hdc = GetWindowDC(hwnd);
	RECT rect;
	SCROLLINFO info = { sizeof(info)};

	GetClientRect(hwnd, &rect);
	rect.top += CC_PADDING;
	int nVisibleRows = (rect.bottom - rect.top + pCC->ccp_lineHeight - 1) / pCC->ccp_lineHeight;
	int nRows = pCC->ccp_size;
	info.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
	info.nPage = nVisibleRows;
	pCC->ccp_pageSize = nVisibleRows;
	info.nPos = pCC->ccp_topRow;
	info.nMax = nRows;
	info.nMin = 0;
	SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
	ReleaseDC(hwnd, hdc);
}

static HASHMAP* _suggestions;
static CODE_ACTION* codecomplete_addTagsWithAlloc(intptr_t pszTagName, BOOL bAlloc) {
	if (hashmap_containsKey(_suggestions, pszTagName)) {
		return NULL;
	}
	unsigned char* pszCopy = bAlloc ? _strdup((char*)pszTagName) : (unsigned char* )pszTagName;
	CODE_ACTION* pCurrent = (CODE_ACTION*)calloc(1, sizeof * pCurrent);
	pCurrent->ca_name = pszCopy;
	pCurrent->ca_type = CA_TAG;
	pCurrent->ca_param.text = pszCopy;
	pCurrent->ca_replaceWord = TRUE;
	pCurrent->ca_freeName = bAlloc;
	hashmap_put(_suggestions, (intptr_t)pszCopy, (intptr_t)pCurrent);
	return pCurrent;
}

static void codecomplete_addTags(intptr_t pszTagName, intptr_t pTag) {
	codecomplete_addTagsWithAlloc(pszTagName, FALSE);
}

static void codecomplete_analyzerCallback(const char* pszRecommendation) {
	codecomplete_addTagsWithAlloc((intptr_t) pszRecommendation, TRUE);
}

/*
 * The current identifier under the cursor. 
 */
static 	char szIdent[100];
static int codecomplete_compare(const CODE_ACTION** p1, const CODE_ACTION** p2) {
	char* pszString1 = (*p1)->ca_name;
	char* pszString2 = (*p2)->ca_name;
	int ret = strcmp(pszString1, pszString2);
	if (ret != 0) {
		size_t nLen = strlen(szIdent);
		if (strncmp(szIdent, pszString1, nLen) == 0) {
			if (strncmp(szIdent, pszString2, nLen) == 0) {
				return ret;
			}
			return -1;
		} else {
			if (strncmp(szIdent, pszString2, nLen) == 0) {
				return 1;
			}
		}
	}
	return ret;
}

static char* _pszMatch;
static int codecomplete_matchWord(const char* pszWord) {
	// add all words to the completion list, which are not identical to the word searched, but where the word
	// searched / completed is a substring.
	return string_strcasestr(pszWord, _pszMatch) != NULL && strcmp(pszWord, _pszMatch) != 0;
}

/*
 * Destroy actions previously allocated.
 */
static int codecomplete_destroyAction(CODE_ACTION* pAction) {
	if (pAction->ca_freeName) {
		free(pAction->ca_name);
	}
	return 1;
}

static void codecomplete_destroyActions(CODE_COMPLETION_PARAMS* pCC) {
	ll_destroy((LINKED_LIST**)&pCC->ccp_actions, codecomplete_destroyAction);
}

/*
 * codecomplete_updateCompletionList
 * update the list of completions awailable.
 */
void codecomplete_updateCompletionList(WINFO* wp, BOOL bForce) {
	if (!wp->codecomplete_handle || (!IsWindowVisible(wp->codecomplete_handle) && !bForce)) {
		return;
	}
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS * )GetWindowLongPtr(wp->codecomplete_handle, GWL_PARAMS);
	FTABLE* fp = wp->fp;
	UCLIST* up = grammar_getUndercursorActions(fp->documentDescriptor->grammar);
	CODE_ACTION* pCurrent;

	codecomplete_destroyActions(pCC);
	pCC->ccp_topRow = 0;
	while (up) {
		if (up->action == UA_ABBREV) {
			pCurrent = ll_insert(&pCC->ccp_actions, sizeof * pCC->ccp_actions);
			pCurrent->ca_name = up->pat;
			pCurrent->ca_type = CA_TEMPLATE;
			pCurrent->ca_param.template = up;
			pCurrent->ca_replaceWord = strstr(up->p.uc_template, "${word_selection}") != NULL;
		}
		up = up->next;
	}
	_suggestions = hashmap_create(37, NULL, NULL);
	xref_getSelectedIdentifier(szIdent, sizeof szIdent);
	_pszMatch = szIdent;
	// (T) pass in the match function here as well rather than the identifier.
	xref_forAllTagsDo(wp, szIdent, (void (*)(intptr_t, intptr_t))codecomplete_addTags);
	GRAMMAR* pGrammar = fp->documentDescriptor->grammar;
	grammar_addSuggestionsMatching(pGrammar, codecomplete_matchWord, (void (*)(intptr_t, intptr_t))codecomplete_addTags);
	char* pszAnalyzer = grammar_getCodeAnalyzer(pGrammar);
	analyzer_performAnalysis(pszAnalyzer, wp, codecomplete_matchWord, codecomplete_analyzerCallback);
	ARRAY_LIST* actionList = hashmap_values(_suggestions);
	hashmap_destroy(_suggestions, NULL);
	_suggestions = NULL;
	arraylist_sort(actionList, codecomplete_compare);
	for (int i = (int)arraylist_size(actionList); --i >= 0; ) {
		CODE_ACTION* pAction = arraylist_get(actionList, i);
		pAction->ca_next = pCC->ccp_actions;
		pCC->ccp_actions = pAction;
	}
	arraylist_destroy(actionList);
	pCC->ccp_size = ll_size((LINKED_LIST*)pCC->ccp_actions);
	codecomplete_updateScrollbar(wp->codecomplete_handle);
	RedrawWindow(wp->codecomplete_handle, NULL, NULL, RDW_INVALIDATE);
}

/*
 * Paint the list of selectable code action. 
 */
static void codecomplete_paint(HWND hwnd) {
	PAINTSTRUCT paint;
	HICON hIconTemplate = LoadIcon(hInst, "TEMPLATE");
	HICON hIconTag = LoadIcon(hInst, "CROSSREFERENCE");
	TEXTMETRIC textmetric;
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	CODE_ACTION* up = pCC->ccp_actions;
	long nSelectedIndex = pCC->ccp_selection;
	int x = CC_PADDING;
	int y = CC_PADDING;
	int nDelta = CC_PADDING;
	int nIconSize = 16;

	BeginPaint(hwnd, &paint);
	SetBkMode(paint.hdc, TRANSPARENT);
	HFONT hFont = SelectObject(paint.hdc, theme_createSmallFixedFont());
	GetTextMetrics(paint.hdc, &textmetric);
	int oldLineHeight = pCC->ccp_lineHeight;
	pCC->ccp_lineHeight = textmetric.tmHeight + CC_PADDING;
	FillRect(paint.hdc, &paint.rcPaint, GetSysColorBrush(COLOR_WINDOW));
	if (pCC->ccp_size == 0) {
		RECT rect;
		GetClientRect(hwnd, &rect);
		InflateRect(&rect, -5, -5);
		SetTextColor(paint.hdc, GetSysColor(COLOR_GRAYTEXT));
		char *pszBuf;
		if ((pszBuf = dlg_getResourceString(IDS_NO_TEMPLATES_DEFINED)) != NULL) {
			DrawText(paint.hdc, pszBuf, -1, &rect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
		}
	} else {
		for (int i = 0; up; i++) {
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
	}
	DeleteObject(SelectObject(paint.hdc, hFont));
	DeleteObject(hIconTemplate);
	DeleteObject(hIconTag);
	EndPaint(hwnd, &paint);
	if (oldLineHeight != pCC->ccp_lineHeight) {
		codecomplete_updateScrollbar(hwnd);
	}
}

static void codecomplete_moveCaret(HWND hwnd, CODE_COMPLETION_PARAMS* pCC, int nTo) {
	if (nTo < 0) {
		nTo = 0;
	} else if (nTo >= pCC->ccp_size) {
		nTo = pCC->ccp_size - 1;
	}
	if (pCC->ccp_selection == nTo || nTo < 0) {
		return;
	}
	pCC->ccp_selection = nTo;
	int newTop = pCC->ccp_topRow;
	if (pCC->ccp_selection < pCC->ccp_topRow) {
		newTop = pCC->ccp_selection;
	}
	else if (pCC->ccp_topRow + pCC->ccp_pageSize - 2 <= pCC->ccp_selection) {
		newTop = pCC->ccp_selection - pCC->ccp_pageSize + 2;
	}
	if (pCC->ccp_topRow < 0) {
		newTop = 0;
	}
	if (newTop != pCC->ccp_topRow) {
		pCC->ccp_topRow = newTop;
		codecomplete_updateScrollbar(hwnd);
	}
	RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
}

static void codecomplete_moveCaretBy(HWND hwnd, CODE_COMPLETION_PARAMS* pCC, int nBy) {
	codecomplete_moveCaret(hwnd, pCC, pCC->ccp_selection + nBy);
}

/*
 * Execute the actual action.
 */
static void codecomplete_action(HWND hwnd) {
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	CODE_ACTION* cap = pCC->ccp_actions;
	long nSelectedIndex = pCC->ccp_selection;
	WINFO* wp = pCC->ccp_wp;

	cap = (CODE_ACTION*) ll_at((LINKED_LIST*)cap, nSelectedIndex);
	if (cap) {
		if (cap->ca_type == CA_TEMPLATE) {
			macro_insertCodeTemplate(wp, cap->ca_param.template, cap->ca_replaceWord);
		} else {
			UCLIST uclTemp;
			uclTemp.action = UA_ABBREV;
			uclTemp.p.uc_template = cap->ca_param.text;
			macro_insertCodeTemplate(wp, &uclTemp, cap->ca_replaceWord);
		}
	}
	ShowWindow(hwnd, SW_HIDE);
}

static void codecomplete_scrollTo(HWND hwnd, CODE_COMPLETION_PARAMS* pCC, int nNewTop) {
	if (nNewTop + pCC->ccp_pageSize > pCC->ccp_size+1) {
		nNewTop = pCC->ccp_size - pCC->ccp_pageSize + 1;
	}
	if (nNewTop < 0) {
		nNewTop = 0;
	}
	if (nNewTop != pCC->ccp_topRow) {
		pCC->ccp_topRow = nNewTop;
		if (pCC->ccp_selection < pCC->ccp_topRow) {
			pCC->ccp_selection = pCC->ccp_topRow;
		} else if (pCC->ccp_selection >= pCC->ccp_topRow + pCC->ccp_pageSize) {
			pCC->ccp_selection = pCC->ccp_topRow;
		}
		codecomplete_updateScrollbar(hwnd);
		RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
	}
}

static void codecomplete_selectByMouse(HWND hwnd, int y) {
	CODE_COMPLETION_PARAMS* pCC;
	pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);

	int nIndex = (y - CC_PADDING) / pCC->ccp_lineHeight + pCC->ccp_topRow;
	codecomplete_moveCaretBy(hwnd, pCC, nIndex - pCC->ccp_selection);
}

/*
 * Process keyboard input in the code completion window. If the key is processed, return TRUE,
 * otherwise FALSE.
 */
BOOL codecomplete_processKey(HWND hwnd, UINT message, WPARAM wParam) {
	if (message != WM_KEYDOWN || !IsWindowVisible(hwnd)) {
		return FALSE;
	}
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	if (wParam == VK_UP || wParam == VK_DOWN) {
		if (pCC->ccp_size == 0) {
			return FALSE;
		}
		codecomplete_moveCaretBy(hwnd, pCC, wParam == VK_UP ? -1 : 1);
		return TRUE;
	}
	if (wParam == VK_HOME) {
		codecomplete_moveCaret(hwnd, pCC, 0);
		return TRUE;
	}
	if (wParam == VK_END) {
		codecomplete_moveCaret(hwnd, pCC, pCC->ccp_size-1);
		return TRUE;
	}
	if (wParam == VK_NEXT) {
		codecomplete_moveCaret(hwnd, pCC, pCC->ccp_selection + pCC->ccp_pageSize - 1);
		return TRUE;
	}
	if (wParam == VK_PRIOR) {
		codecomplete_moveCaret(hwnd, pCC, pCC->ccp_selection - pCC->ccp_pageSize + 1);
		return TRUE;
	}
	if (wParam == VK_ESCAPE) {
		ShowWindow(hwnd, SW_HIDE);
		return TRUE;
	}
	if (wParam == VK_RETURN) {
		codecomplete_action(hwnd);
		return TRUE;
	}

	return FALSE;
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
				codecomplete_destroyActions(pCC);
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
		case WM_LBUTTONDBLCLK: 
		case WM_LBUTTONDOWN: {
				int y = GET_Y_LPARAM(lParam);
				codecomplete_selectByMouse(hwnd, y);
				if (message == WM_LBUTTONDBLCLK) {
					codecomplete_action(hwnd);
				}
			}
			break;
		case WM_SHOWWINDOW:
			pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
			if (pCC) {
				pCC->ccp_selection = 0;
			}
			break;
		case WM_VSCROLL: 
			pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
			if (pCC) {
				int nNewTop = pCC->ccp_topRow;
				int nCode = LOWORD(wParam);
				if (nCode == SB_LINEUP) {
					nNewTop--;
				} else if (nCode == SB_LINEDOWN) {
					nNewTop++;
				} else if (nCode == SB_PAGEDOWN) {
					nNewTop += pCC->ccp_pageSize;
				} else if (nCode == SB_PAGEUP) {
					nNewTop -= pCC->ccp_pageSize;
				} else if (nCode == SB_TOP) {
					nNewTop = 0;
				}
				else if (nCode == SB_BOTTOM) {
					nNewTop = pCC->ccp_size - pCC->ccp_pageSize;
				} else if (nCode == SB_ENDSCROLL) {
					break;
				} else {
					nNewTop = HIWORD(wParam);
				}
				codecomplete_scrollTo(hwnd, pCC, nNewTop);
			}
			break;
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
	wc.hCursor = LoadCursor(0, IDC_ARROW);
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
		codecomplete_updateCompletionList(wp, TRUE);
		ShowWindow(wp->codecomplete_handle, SW_SHOWNA);
	}
	return 1;
}


