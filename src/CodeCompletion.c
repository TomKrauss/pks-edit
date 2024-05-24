/*
 * CodeCompletion.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: showing code completion and suggestion windows.
 *
 * 										created: 14.04.22
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "alloc.h"
#include <windows.h>
#include <windowsx.h>
#include "customcontrols.h"
#include "winfo.h"
#include "stringutil.h"
#include "arraylist.h"
#include "hashmap.h"
#include "winterf.h"
#include "codeanalyzer.h"
#include "grammar.h"
#include "pksrc.h"
#include "linkedlist.h"
#include "crossreferencelinks.h"
#include "themes.h"
#include "xdialog.h"
#include "streams.h"
#include "htmlrendering.h"
#include "dpisupport.h"
#include "levensthein.h"

#define GWL_HELPWINDOW_VIEWPARTS		0
#define GWL_HELPWINDOW_PARAMS			GWL_HELPWINDOW_VIEWPARTS+sizeof(void*)
#define GWL_HELPWINDOW_EXTRA			GWL_HELPWINDOW_PARAMS+sizeof(void*)

#define CLASS_CODE_COMPLETION		"CodeCompletion"
#define CLASS_CODE_COMPLETION_HELP	"CodeCompletionJHelp"
#define CC_WIDTH			180
#define CC_HEIGHT			200
#define CC_HELP_LINE_LEN	80

#define CC_PADDING			4

extern HINSTANCE hInst;

typedef enum { CA_TEMPLATE, CA_TAG} CODE_ACTION_TYPE;

typedef struct tagCODE_ACTION {
	struct tagCODE_ACTION* ca_next;
	CODE_ACTION_TYPE ca_type;
	const char* ca_name;
	BOOL ca_replaceWord;
	BOOL ca_freeName;
	int  ca_score;
	int  ca_replacedTextLength;		// The length of the place (left to the caret) to be replaced in a code completion.
	const char* (*ca_helpCB)(const char* pszCompletion, void* pParam);	
									// Callback to return a help text for this action. Note that the returned string must be malloc'd and will be freed after use.
	const char* (*ca_getHyperlinkHelp)(const char* pszLink);
									// Callback to return a help text for a hyperlink contained in a help text pointing to another help contents.
	void* ca_object;				// an arbitrary object representing the details about the object for the code completion (e.g. a MACROREF object representing a macro/command)
									// Can in particular used in the help callback to provide a help text for the action.
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

static void codecomplete_changeSelection(HWND hwnd, CODE_COMPLETION_PARAMS* pCC, int nNewSelection);

#define	GWL_PARAMS				0
#define	GWL_SECONDARY_WINDOW	GWL_PARAMS+sizeof(CODE_COMPLETION_PARAMS*)

static void codecomplete_updateScrollbar(HWND hwnd) {
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	if (pCC->ccp_lineHeight == 0) {
		return;
	}
	HDC hdc = GetWindowDC(hwnd);
	RECT rect;
	SCROLLINFO info = { sizeof(info)};

	GetClientRect(hwnd, &rect);
	//rect.top += CC_PADDING;
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
static ARRAY_LIST* _actionList;

static int codecomplete_compareScore(const CODE_ACTION** p1, const CODE_ACTION** p2) {
	return (*p2)->ca_score - (*p1)->ca_score;
}

static CODE_ACTION* codecomplete_addTagsWithAlloc(ANALYZER_CALLBACK_PARAM* bParam, BOOL bAlloc) {
	const char* pszTagName = bParam->acp_recommendation;
	const char* (*fHelpCB)(const char* pszCompletion, void* pParam) = bParam->acp_help;
	const char* (*fGetHelpForLinkCB)(const char* pszUrl) = bParam->acp_getHyperlinkText;
	void* nParam = bParam->acp_object;
	if (hashmap_containsKey(_suggestions, pszTagName)) {
		return NULL;
	}
	unsigned char* pszCopy = bAlloc ? _strdup(pszTagName) : (unsigned char* )pszTagName;
	CODE_ACTION* pAction = (CODE_ACTION*)calloc(1, sizeof * pAction);
	pAction->ca_name = pszCopy;
	pAction->ca_type = CA_TAG;
	pAction->ca_param.text = pszCopy;
	pAction->ca_replaceWord = TRUE;
	pAction->ca_freeName = bAlloc;
	pAction->ca_score = bParam->acp_score;
	pAction->ca_replacedTextLength = bParam->acp_replacedTextLength;
	pAction->ca_helpCB = fHelpCB;
	pAction->ca_getHyperlinkHelp = fGetHelpForLinkCB;
	pAction->ca_object = nParam;
	hashmap_put(_suggestions, pszCopy, (intptr_t)pAction);
	arraylist_add(_actionList, pAction);
	return pAction;
}

static void codecomplete_hideWindow(HWND hwnd) {
	ShowWindow(hwnd, SW_HIDE);
	HWND hwndSecondary = (HWND)GetWindowLongPtr(hwnd, GWL_SECONDARY_WINDOW);
	if (hwndSecondary) {
		ShowWindow(hwndSecondary, SW_HIDE);
	}

}

/*
 * The current identifier under the cursor.
 */
static char szIdent[100];
static int codecomplete_matchWord(const char* pszWord) {
	// add all words to the completion list, which are not identical to the word searched, but where the word
	// searched / completed is a substring.
	return string_strcasestr(pszWord, szIdent) != NULL;
}

/*
 * Calculate a score for a match during code completion. The 'pszSearch' variable contains
 * the word to search, the 'pszFound' variable the recommendation.
 */
int codecomplete_calculateScore(const char* pszSearch, const char* pszFound) {
	int lFound = (int)strlen(pszFound);
	if (strstr(pszFound, pszSearch) == pszFound) {
		return 100 + lFound;
	}
	if (string_strcasestr(pszFound, pszSearch) == pszFound) {
		return 50 + lFound;
	}
	long l = (long)strlen(pszSearch);
	return 20 - levenshtein_calculate(pszSearch, l, pszFound, l, 0);
}

static void codecomplete_addTags(ANALYZER_CALLBACK_PARAM* bParam) {
	if (bParam->acp_score == 0) {
		bParam->acp_score = codecomplete_calculateScore(szIdent, bParam->acp_recommendation);
	}
	codecomplete_addTagsWithAlloc(bParam, FALSE);
}

static void codecomplete_analyzerCallback(ANALYZER_CALLBACK_PARAM *bParam) {
	if (bParam->acp_score == 0) {
		bParam->acp_score = codecomplete_calculateScore(szIdent, bParam->acp_recommendation);
	}
	codecomplete_addTagsWithAlloc(bParam, TRUE);
}

/*
 * Destroy actions previously allocated.
 */
static int codecomplete_destroyAction(CODE_ACTION* pAction) {
	if (pAction->ca_freeName) {
		free((char*)pAction->ca_name);
	}
	return 1;
}

static void codecomplete_destroyActions(CODE_COMPLETION_PARAMS* pCC) {
	ll_destroy((LINKED_LIST**)&pCC->ccp_actions, codecomplete_destroyAction);
}

static char* codecomplete_helpForTemplate(const char* pszCompletion, void* pText) {
	return pText == NULL ? NULL : _strdup(pText);
}

/*
 * Tries to find the longest matching pattern close to the caret from the list of 
 * patterns to match.
 */
static void codecomplete_matchPatterns(WINFO* wp, UCLIST* up) {
	char matched[128];
	szIdent[0] = 0;
	while (up) {
		template_matchIdentifier(wp, up->uc_pattern.pattern, matched, sizeof matched);
		if (strlen(matched) > strlen(szIdent)) {
			strcpy(szIdent, matched);
		}
		up = up->next;
	}
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

	codecomplete_destroyActions(pCC);
	pCC->ccp_topRow = 0;
	codecomplete_matchPatterns(wp, up);
	_actionList = arraylist_create(37);
	while (up) {
		if (up->action == UA_TEMPLATE && codecomplete_matchWord(up->uc_pattern.pattern)) {
			CODE_ACTION* pAction = (CODE_ACTION*)calloc(1, sizeof * pAction);
			pAction->ca_name = up->uc_pattern.pattern;
			pAction->ca_type = CA_TEMPLATE;
			pAction->ca_param.template = up;
			pAction->ca_score = 200;
			pAction->ca_helpCB = codecomplete_helpForTemplate;
			pAction->ca_object = up->uc_pattern.helpText;
			pAction->ca_replaceWord = strstr(up->p.uc_template, "${word_selection}") != NULL;
			arraylist_add(_actionList, pAction);
		}
		up = up->next;
	}
	_suggestions = hashmap_create(37, NULL, NULL);
	GRAMMAR* pGrammar = fp->documentDescriptor->grammar;
	char* pszAnalyzer = grammar_getCodeAnalyzer(pGrammar);
	analyzer_performAnalysis(pszAnalyzer, wp, codecomplete_analyzerCallback);
	xref_forAllTagsDo(wp, codecomplete_matchWord, codecomplete_addTags);
	grammar_addSuggestionsMatching(pGrammar, codecomplete_matchWord, codecomplete_addTags);
	arraylist_sort(_actionList, codecomplete_compareScore);
	hashmap_destroy(_suggestions, NULL);
	_suggestions = NULL;
	// Process in reverse order to efficiently create linked list.
	size_t nSize = arraylist_size(_actionList);
	for (int i = (int)nSize; --i >= 0; ) {
		CODE_ACTION* pAction = arraylist_get(_actionList, i);
		pAction->ca_next = pCC->ccp_actions;
		pCC->ccp_actions = pAction;
	}
	arraylist_destroy(_actionList);
	pCC->ccp_size = ll_size((LINKED_LIST*)pCC->ccp_actions);
	codecomplete_updateScrollbar(wp->codecomplete_handle);
	codecomplete_changeSelection(wp->codecomplete_handle, pCC, 0);
	InvalidateRect(wp->codecomplete_handle, NULL, TRUE);
}

static MARKDOWN_RENDERER_DATA* codecompletehelp_getData(HWND hwnd) {
	return (MARKDOWN_RENDERER_DATA*)GetWindowLongPtr(hwnd, GWL_HELPWINDOW_VIEWPARTS);
}

/*
 * Paint the help window.
 */
static void codecomplete_paintHelp(HWND hwnd) {
	PAINTSTRUCT paint;

	BeginPaint(hwnd, &paint);
	MARKDOWN_RENDERER_DATA* pData = codecompletehelp_getData(hwnd);
	if (pData) {
		SCROLLINFO info = {
			.cbSize = sizeof info,
			.fMask = SIF_POS,
		};
		GetScrollInfo(hwnd, SB_VERT, &info);
		mdr_renderMarkdownData(hwnd, &paint, info.nPos, pData);
	}
}

/*
 * Update the size and position of the scrollbar as either the window size has changed
 * or a WM_VSCROLL message had been sent to the help window. In the later case bScrollChanged is 1
 * and wParam contains the details about the message.
 */
static void codecomplete_helpWindowUpdateScrollbar(HWND hwnd, int bScrollChanged, WPARAM wParam) {
	MARKDOWN_RENDERER_DATA* pData = codecompletehelp_getData(hwnd);
	if (pData) {
		mdr_scrolled(hwnd, pData, wParam, bScrollChanged);
	}
}

/*
 * The size of the hwlp window has changed. Invalidate the view parts and recalculate the total size to
 * update the scrollbars.
 */
static void codecomplete_helpWindowSizeChanged(HWND hwnd) {
	MARKDOWN_RENDERER_DATA* pData = codecompletehelp_getData(hwnd);
	if (pData) {
		mdr_invalidateViewpartsLayout(pData);
		InvalidateRect(hwnd, 0, FALSE);
		UpdateWindow(hwnd);
		codecomplete_helpWindowUpdateScrollbar(hwnd, 0, 0);
	}
}

/*
 * Paint the list of selectable code action. 
 */
static void codecomplete_paint(HWND hwnd) {
	PAINTSTRUCT paint;
	THEME_DATA* pTheme = theme_getCurrent();
	HICON hIconTemplate = LoadIcon(hInst, "TEMPLATE");
	HICON hIconTag = LoadIcon(hInst, "CROSSREFERENCE");
	TEXTMETRIC textmetric;
	CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
	CODE_ACTION* up = pCC->ccp_actions;
	long nSelectedIndex = pCC->ccp_selection;

	BeginPaint(hwnd, &paint);
	SetBkMode(paint.hdc, TRANSPARENT);
	HFONT hFont = SelectObject(paint.hdc, theme_createSmallFixedFont());
	GetTextMetrics(paint.hdc, &textmetric);
	int oldLineHeight = pCC->ccp_lineHeight;
	pCC->ccp_lineHeight = textmetric.tmHeight + CC_PADDING;
	HBRUSH hBrush = CreateSolidBrush(pTheme->th_dialogBackground);
	FillRect(paint.hdc, &paint.rcPaint, hBrush);
	DeleteObject(hBrush);
	if (pCC->ccp_size == 0) {
		RECT rect;
		GetClientRect(hwnd, &rect);
		InflateRect(&rect, -5, -5);
		SetTextColor(paint.hdc, pTheme->th_dialogDisabled);
		char *pszBuf;
		if ((pszBuf = dlg_getResourceString(IDS_NO_TEMPLATES_DEFINED)) != NULL) {
			DrawText(paint.hdc, pszBuf, -1, &rect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
		}
	} else {
		int x = CC_PADDING;
		int y = 0;
		int nDelta = CC_PADDING;
		int nIconSize = dpisupport_getSize(16);
		for (int i = 0; up; i++) {
			if (i >= pCC->ccp_topRow) {
				if (i == nSelectedIndex) {
					RECT rect;
					rect.left = 0;
					rect.right = paint.rcPaint.right;
					rect.top = y;
					rect.bottom = y + textmetric.tmHeight + nDelta;
					HBRUSH hBrushFill = CreateSolidBrush(pTheme->th_dialogHighlight);
					FillRect(paint.hdc, &rect, hBrushFill);
					DeleteObject(hBrushFill);
					SetTextColor(paint.hdc, pTheme->th_dialogHighlightText);
				}
				else {
					SetTextColor(paint.hdc, pTheme->th_dialogForeground);
				}
				const char* pszDescription = up->ca_name;
				DrawIconEx(paint.hdc, x, y+nDelta/2, up->ca_type == CA_TEMPLATE ? hIconTemplate : hIconTag, textmetric.tmHeight, textmetric.tmHeight, 0, NULL, DI_NORMAL);
				TextOut(paint.hdc, x + nIconSize + 4, y + (nDelta / 2), pszDescription, (int)strlen(pszDescription));
				y += textmetric.tmHeight + nDelta;
				if (y > paint.rcPaint.bottom) {
					break;
				}
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

static void codecomplete_invalidateIndex(HWND hwnd, RECT* pRect, CODE_COMPLETION_PARAMS* pCC, int idx) {
	RECT r;
	GetClientRect(hwnd, &r);
	idx -= pCC->ccp_topRow;
	r.top += idx * pCC->ccp_lineHeight;
	r.bottom = r.top + pCC->ccp_lineHeight + CC_PADDING;
	InvalidateRect(hwnd, &r, TRUE);
}


/*
 * Creates the help (code completion secondary) window.
 */
static HWND codecomplete_createHelpWindow(HWND hwndParent) {
	HWND hwnd = CreateWindow(CLASS_CODE_COMPLETION_HELP, NULL, WS_POPUP | WS_SIZEBOX | WS_VSCROLL, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hwndParent, NULL, hInst, 0);
	if (hwnd) {
		SetWindowLong(hwnd, GWL_STYLE, WS_BORDER);
		theme_enableDarkMode(hwnd);
	}
	return hwnd;
}

/*
 * Update the window position of the help window
 */
static void codecomplete_updateHelpWindowPosition(HWND hwnd) {
	HWND hwndSecondary = (HWND)GetWindowLongPtr(hwnd, GWL_SECONDARY_WINDOW);
	if (hwndSecondary) {
		RECT rect;
		GetWindowRect(hwnd, &rect);
		RECT rcOld;
		GetClientRect(hwndSecondary, &rcOld);
		int nFlags = SWP_SHOWWINDOW | SWP_NOACTIVATE;
		if (rcOld.right - rcOld.left >= dpisupport_getSize(100)) {
			nFlags |= SWP_NOSIZE;
		}
		SetWindowPos(hwndSecondary, NULL, rect.right + 5, rect.top, dpisupport_getSize(CC_HELP_LINE_LEN * 7), rect.bottom - rect.top, nFlags);
	}
}

/*
 * Update the help window contents (HTML formatted text) to display.
 */
static void codecomplete_setHelpContents(HWND hwnd, CODE_COMPLETION_PARAMS* pCC, const char* pszHelp) {
	MARKDOWN_RENDERER_DATA* pData = codecompletehelp_getData(hwnd);
	if (pData) {
		mdr_destroyRendererData(pData);
		SetWindowLongPtr(hwnd, GWL_HELPWINDOW_PARAMS, 0);
		SetWindowLongPtr(hwnd, GWL_HELPWINDOW_VIEWPARTS, 0);
	}
	if (pszHelp && *pszHelp) {
		INPUT_STREAM* pStream = streams_createStringInputStream(pszHelp);
		pData = mdr_parseHTML(pStream, hwnd, "");
		pStream->is_destroy(pStream);
		SetWindowLongPtr(hwnd, GWL_HELPWINDOW_VIEWPARTS, (LONG_PTR)pData);
		SetWindowLongPtr(hwnd, GWL_HELPWINDOW_PARAMS, (LONG_PTR)pCC);
		codecomplete_helpWindowSizeChanged(hwnd);
		//InvalidateRect(hwnd, 0, 1);
	}
}

/*
 * Update the code completion secondary window and display the help for a selected code action.
 */
static void codecompletehelp_displayHelpText(HWND hwnd, HWND hwndHelp, CODE_COMPLETION_PARAMS* pParam, const char* pszHelp) {
	if (!pszHelp || !*pszHelp) {
		if (hwndHelp) {
			codecomplete_setHelpContents(hwndHelp, 0, 0);
			ShowWindow(hwndHelp, SW_HIDE);
		}
		return;
	}
	if (!hwndHelp) {
		if (!hwnd) {
			return;
		}
		hwndHelp = codecomplete_createHelpWindow(GetParent(hwnd));
		if (!hwndHelp) {
			return;
		}
		SetWindowLongPtr(hwnd, GWL_SECONDARY_WINDOW, (LONG_PTR)hwndHelp);
	}
	codecomplete_updateHelpWindowPosition(hwnd);
	codecomplete_setHelpContents(hwndHelp, pParam, pszHelp);
	InvalidateRect(hwndHelp, NULL, FALSE);

}

/*
 * Displays a help for the current code completion suggestion-
 */
static void codecomplete_displayHelpFor(HWND hwnd, CODE_COMPLETION_PARAMS* pParam) {
	HWND hwndSecondary = (HWND)GetWindowLongPtr(hwnd, GWL_SECONDARY_WINDOW);
	const char* pszHelp = NULL;
	if (pParam->ccp_selection >= 0) {
		CODE_ACTION* pSelected = (CODE_ACTION*)ll_at((LINKED_LIST*)pParam->ccp_actions, pParam->ccp_selection);
		if (pSelected) {
			if (pSelected->ca_helpCB) {
				pszHelp = pSelected->ca_helpCB(pSelected->ca_name, pSelected->ca_object);
			}
			else if (pSelected->ca_type == CA_TEMPLATE) {
				pszHelp = template_expandCodeTemplateFor(pSelected->ca_param.template);
			}
		}
	}
	codecompletehelp_displayHelpText(hwnd, hwndSecondary, pParam, pszHelp);
	free((char*)pszHelp);
}

/*
 * Hyperlinking in the help secondary window.
 */
static void codecompletehelp_navigate(HWND hwnd, CODE_COMPLETION_PARAMS* pParam, const char* pszLink) {
	CODE_ACTION* pAction = pParam->ccp_actions;
	if (pAction && pAction->ca_getHyperlinkHelp) {
		char* pszHelp = (char*)pAction->ca_getHyperlinkHelp(pszLink);
		if (pszHelp) {
			// allow to "re-select" the original help text.
			pParam->ccp_selection = -1;
			codecompletehelp_displayHelpText(0, hwnd, pParam, pszHelp);
			free(pszHelp);
		}
	}
}

static void codecomplete_changeSelection(HWND hwnd, CODE_COMPLETION_PARAMS* pCC, int nNewSelection) {
	if (pCC->ccp_selection != nNewSelection || nNewSelection == 0) {
		pCC->ccp_selection = nNewSelection;
		codecomplete_displayHelpFor(hwnd, pCC);
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
	int nOld = pCC->ccp_selection;
	codecomplete_changeSelection(hwnd, pCC, nTo);
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
		int nDelta = newTop - pCC->ccp_topRow;
		pCC->ccp_topRow = newTop;
		codecomplete_updateScrollbar(hwnd);
		if (nDelta >= -3 && nDelta <= 3) {
			ScrollWindow(hwnd, 0, -nDelta*pCC->ccp_lineHeight, NULL, NULL);
		} else {
			InvalidateRect(hwnd, NULL, TRUE);
			return;
		}
	} 
	RECT r;
	GetClientRect(hwnd, &r);
	codecomplete_invalidateIndex(hwnd, &r, pCC, nOld);
	codecomplete_invalidateIndex(hwnd, &r, pCC, nTo);
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
			template_insertCodeTemplate(wp, cap->ca_param.template, cap->ca_replacedTextLength, TRUE);
		} else {
			UCLIST uclTemp;
			uclTemp.action = UA_ABBREV;
			uclTemp.p.uc_template = cap->ca_param.text;
			template_insertCodeTemplate(wp, &uclTemp, cap->ca_replacedTextLength, cap->ca_replaceWord);
		}
	}
	codecomplete_hideWindow(hwnd);
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
			codecomplete_changeSelection(hwnd, pCC, pCC->ccp_topRow);
		} else if (pCC->ccp_selection >= pCC->ccp_topRow + pCC->ccp_pageSize) {
			codecomplete_changeSelection(hwnd, pCC, pCC->ccp_topRow);
		}
		codecomplete_updateScrollbar(hwnd);
		InvalidateRect(hwnd, NULL, TRUE);
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
		codecomplete_hideWindow(hwnd);
		return TRUE;
	}
	if (wParam == VK_RETURN) {
		codecomplete_action(hwnd);
		return TRUE;
	}

	return FALSE;
}

/*
 * Window procedure of the code completion help window.
 */
static LRESULT codecomplete_helpWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_ERASEBKGND:
		return 0;
	case WM_PAINT:
		codecomplete_paintHelp(hwnd);
		return 0;
	case WM_MOUSEWHEEL: {
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			codecomplete_helpWindowUpdateScrollbar(hwnd, 1, zDelta < 0 ? SB_PAGEDOWN : SB_PAGEUP);
		}
		break;
	case WM_MOUSEMOVE: {
		MARKDOWN_RENDERER_DATA* pData = codecompletehelp_getData(hwnd);
		if (pData != 0) {
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			mdr_mouseMove(hwnd, pData, xPos, yPos);
		}
		}
		return 0;
	case WM_LBUTTONDOWN: {
		MARKDOWN_RENDERER_DATA* pData = codecompletehelp_getData(hwnd);
		if (pData != 0) {
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			void* pLink = mdr_linkClicked(pData, xPos, yPos);
			if (pLink) {
				CODE_COMPLETION_PARAMS* pCC = (CODE_COMPLETION_PARAMS * )GetWindowLongPtr(hwnd, GWL_HELPWINDOW_PARAMS);
				if (pCC) {
					codecompletehelp_navigate(hwnd, pCC, pLink);
				}
			}
 		}
		}
		break;
	case WM_VSCROLL:
		codecomplete_helpWindowUpdateScrollbar(hwnd, 1, wParam);
		break;
	case WM_SIZE:
		codecomplete_helpWindowSizeChanged(hwnd);
		break;
	case WM_DESTROY:
		codecomplete_setHelpContents(hwnd, 0, 0);
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
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
		case WM_DESTROY: {
				HWND hwndSecondary = (HWND)GetWindowLongPtr(hwnd, GWL_SECONDARY_WINDOW);
				if (hwndSecondary) {
					DestroyWindow(hwndSecondary);
				}
				pCC = (CODE_COMPLETION_PARAMS*)GetWindowLongPtr(hwnd, GWL_PARAMS);
				if (pCC) {
					codecomplete_destroyActions(pCC);
					free(pCC);
					SetWindowLongPtr(hwnd, GWL_PARAMS, (LONG_PTR)NULL);
				}
			}
			break;
		case WM_SIZE:
			codecomplete_updateScrollbar(hwnd);
			break;

		case WM_ERASEBKGND: 
			return 0;
		case WM_PAINT:
			codecomplete_paint(hwnd);
			return 0;
		case WM_LBUTTONDBLCLK: 
		case WM_LBUTTONDOWN: {
				int y = GET_Y_LPARAM(lParam);
				codecomplete_selectByMouse(hwnd, y);
				if (message == WM_LBUTTONDBLCLK) {
					codecomplete_action(hwnd);
				}
			}
			break;
		case WM_WINDOWPOSCHANGED: {
			HWND hwndSecondary = (HWND)GetWindowLongPtr(hwnd, GWL_SECONDARY_WINDOW);
			if (hwndSecondary && IsWindowVisible(hwnd)) {
				WINDOWPOS* pWinpos = (WINDOWPOS*)lParam;
				BOOL bHasContents = codecompletehelp_getData(hwndSecondary) != 0;
				if (!bHasContents || (pWinpos->flags & SWP_HIDEWINDOW)) {
					ShowWindow(hwndSecondary, SW_HIDE);
				} else {
					codecomplete_updateHelpWindowPosition(hwnd);
				}
			}
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
	wc.style = CS_DBLCLKS|CS_DROPSHADOW;
	wc.lpfnWndProc = codecomplete_wndProc;
	wc.cbWndExtra = GWL_SECONDARY_WINDOW+sizeof(HWND);
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wc.lpszClassName = CLASS_CODE_COMPLETION;
	if (!RegisterClass(&wc)) {
		return 0;
	}

	wc.lpfnWndProc = codecomplete_helpWndProc;
	wc.cbWndExtra = GWL_HELPWINDOW_EXTRA;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wc.lpszClassName = CLASS_CODE_COMPLETION_HELP;
	return RegisterClass(&wc);
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
		codecomplete_hideWindow(wp->codecomplete_handle);
	}
}

 /*--------------------------------------------------------------------------
  * codecomplete_showSuggestionWindow()
  */
int codecomplete_showSuggestionWindow(void) {
	WINFO* wp;

	wp = ww_getCurrentEditorWindow();
	if (wp == NULL) {
		return 0;
	}
	POINT pt;
	int width = dpisupport_getSize(CC_WIDTH);
	int height = dpisupport_getSize(CC_HEIGHT);
	if (wp->codecomplete_handle == NULL) {
		codecomplete_calculateWindowPos(wp, &pt, height);
		wp->codecomplete_handle = CreateWindow(CLASS_CODE_COMPLETION, NULL, WS_POPUP|WS_SIZEBOX|WS_VSCROLL|WS_BORDER, 
			pt.x, pt.y, width, height, wp->ww_handle, NULL, hInst, wp);
		SetWindowLong(wp->codecomplete_handle, GWL_STYLE, WS_BORDER);
	} else {
		RECT rect;
		GetWindowRect(wp->codecomplete_handle, &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		codecomplete_calculateWindowPos(wp, &pt, height);
		MoveWindow(wp->codecomplete_handle, pt.x, pt.y, width, height, TRUE);
	}
	theme_enableDarkMode(wp->codecomplete_handle);
	if (!IsWindowVisible(wp->codecomplete_handle)) {
		codecomplete_updateCompletionList(wp, TRUE);
		ShowWindow(wp->codecomplete_handle, SW_SHOWNA);
	}
	return 1;
}


