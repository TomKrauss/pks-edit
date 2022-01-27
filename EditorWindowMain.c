/*
 * EditorWindowMain.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: editor windows - painting etc...
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <CommCtrl.h>
#include "alloc.h"
#include "trace.h"
#include "linkedlist.h"
#include "caretmovement.h"
#include "edierror.h"
#include "regexp.h"
#include "errordialogs.h"
#include "winfo.h"
#include "edfuncs.h"
#include "winterf.h"
#include "pksedit.h"
#include "context.h"
#include "iccall.h"
#include "stringutil.h"
#include "arraylist.h"
#include "editorconfiguration.h"
#include "propertychange.h"
#include "winutil.h"
#include "mouseutil.h"
#include "grammar.h"
#include "actions.h"
#include "menu.h"
#include "themes.h"
#include "mainframe.h"
#include "hashmap.h"

#define	TABTHERE(indent,i)		(indent->tbits[i >> 3] &  bittab[i & 07])
#define	TABPLACE(indent,i)		indent->tbits[i >> 3] |= bittab[i & 07]
#define	TABCLEAR(indent,i)		indent->tbits[i >> 3] &= (~bittab[i & 07])

#define	WT_WORKWIN		0
#define	WT_RULERWIN		1
#define	GWL_VIEWPTR		GWL_ICCLASSVALUES+sizeof(void*)
#define	GWL_WWPTR		0

#define	PROF_OFFSET		1
#define COMPARISON_ANNOTATION_WIDTH 18

typedef struct xywh {
	int x,y,w,h;
} XYWH;

static WINFO *_winlist;

extern int  mouse_onMouseClicked(WINFO *fp, int x,int y,int b, int nclicks,int shift);
extern void macro_selectDefaultBindings(void);
extern RENDERER* hex_getRenderer();
extern RENDERER* mdr_getRenderer();

#define  LINE_ANNOTATION_WIDTH			5
#define  LINE_ANNOATION_PADDING			2

static int lineNumberWindowWidth = 50 + LINE_ANNOTATION_WIDTH + (2*LINE_ANNOATION_PADDING);
static int rulerWindowHeight = 20;

static char   szEditClass[] = "EditWin";
static char   szLineNumbersClass[] = "LineNumbers";
static char   szRulerClass[] = "RulerWin";
static char   szWorkAreaClass[] = "WorkWin";
static HASHMAP *_renderers;

/*
 * Register a renderer. 
 */
void ww_registerRenderer(const char* pszName, RENDERER* pRenderer) {
	if (!_renderers) {
		_renderers = hashmap_create(17, NULL, NULL);
	}
	hashmap_put(_renderers, (intptr_t)pszName, (intptr_t) pRenderer);
}

/*------------------------------------------------------------
 * ww_createOrDestroyChildWindowOfEditor()
 */
static int ww_createOrDestroyChildWindowOfEditor(
	HWND  hwnd, 
	DWORD winFlags,
	BOOL  visible,
	HWND  *hwndChild,
	char  *szClass, 
	XYWH  *XYPxy,
	WINFO *wp
)
{
	if (visible == FALSE) {
		if (*hwndChild) {
			DestroyWindow(*hwndChild);
			*hwndChild = 0;
		}
		return 0;
	}

	if (*hwndChild) {
		return 1;
	}
	*hwndChild = CreateWindow( szClass, NULL,
		winFlags, XYPxy->x, XYPxy->y, XYPxy->w, XYPxy->h, 
		hwnd, NULL, hInst, (LPVOID)wp);
	return *hwndChild != 0;
}

static int ruler_getLeft(WINFO* wp) {
	if ((wp->dispmode & SHOWLINENUMBERS) == 0 && wp->comparisonLink == NULL) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	int nWidth = fp->nlines > 99999 ? (lineNumberWindowWidth * 5 / 4) : lineNumberWindowWidth;
	if (wp->comparisonLink != NULL) {
		nWidth += COMPARISON_ANNOTATION_WIDTH;
	}
	return nWidth;
}

static int ww_useDisplayMode(WINFO* wp, int aFlag) {
	return ((wp->dispmode & aFlag) && wp->renderer->r_supportsMode(aFlag));
}

/*-----------------------------------------------------------
 * ww_createSubWindows()
 */
static int ww_createSubWindows(HWND hwnd, WINFO *wp, XYWH *pWork, XYWH *pRuler, XYWH *pLineInfo) {
	RECT   	rect;
	BOOL	rulerVisible;
	BOOL	lineNumbersVisible;
	int		w;
	int		h;
	int		rh;
	int		rLineNumbers;

	if (wp == 0) {
		return 0;
	}

	GetClientRect(hwnd,&rect);
	w = rect.right-rect.left;
	h = rect.bottom-rect.top;

	rh = rulerWindowHeight;
	pRuler->x = 0;
	pRuler->w = w;
	pRuler->y = 0;
	pRuler->h = rh;

	rulerVisible = (h > rh && ww_useDisplayMode(wp, SHOWRULER));
	if (!ww_createOrDestroyChildWindowOfEditor(hwnd,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		rulerVisible, &wp->ru_handle, szRulerClass, pRuler, wp)) {
		pRuler->h = 0;
	}
	rLineNumbers = ruler_getLeft(wp);
	pLineInfo->x = 0;
	pLineInfo->w = rLineNumbers;
	pLineInfo->y = pRuler->h;
	pLineInfo->h = h - pRuler->h;
	lineNumbersVisible = (w > rLineNumbers && (ww_useDisplayMode(wp, SHOWLINENUMBERS) || wp->comparisonLink != NULL));
	if (!ww_createOrDestroyChildWindowOfEditor(hwnd,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		lineNumbersVisible, &wp->lineNumbers_handle, szLineNumbersClass, pLineInfo, wp)) {
		pLineInfo->w = 0;
	}


	pWork->x = pLineInfo->w;
	pWork->w = w - pLineInfo->w;
	pWork->y = pRuler->h;
	pWork->h = h - pRuler->h;
	int ret = ww_createOrDestroyChildWindowOfEditor(hwnd,
		WS_HSCROLL|WS_VSCROLL|WS_CHILD|WS_VISIBLE/*|WS_CLIPSIBLINGS*/,
		TRUE,&wp->ww_handle,szWorkAreaClass,pWork, wp);
	if (ret) {
		theme_enableDarkMode(wp->ww_handle);
	}
	return ret;
}

/*-----------------------------------------------------------
 * ww_snapPositionToCharGrid()
 */
static int ww_snapPositionToCharGrid(int x, int grid)
{
	if (grid <= 0)
		return 0;
	return (x + grid - 1) / grid;
}

/*------------------------------------------------------------
 * ww_updateRangeAndCheckBounds()
 * calculate scrollops checking bounds
 */
static void ww_updateRangeAndCheckBounds(long nMinLineOrCol, int nDefaultDeltaXorY, int nSizeInPixels,
				  long *max, long *maxcurs, long *mincurs, 
				  int *scroll_d)
{	int delta,d2;

	if (!nDefaultDeltaXorY)
		return;
	*max = nMinLineOrCol + (delta = ww_snapPositionToCharGrid(nSizeInPixels,nDefaultDeltaXorY));
	delta /= 2;
	d2 = (nSizeInPixels % nDefaultDeltaXorY) ? 2 : 1;
	delta -= d2;
	if (delta < 0)
		delta = 1;
	if (*scroll_d > delta)
		*scroll_d = delta;
	*mincurs = (nMinLineOrCol < *scroll_d) ? nMinLineOrCol : nMinLineOrCol + *scroll_d;
	*maxcurs = *max - (*scroll_d + d2);

     if (*mincurs+*scroll_d > *maxcurs) {
     	*mincurs = *maxcurs = (*max-nMinLineOrCol) / 2;
		*scroll_d = 1;
	}
}

/*------------------------------------------------------------
 * ww_setScrollCheckBounds()
 * calculate scroll checking bounds
 */
void ww_setScrollCheckBounds(WINFO *wp) {
	RECT rect;
	GetClientRect(wp->ww_handle, &rect);
	int dy = wp->scroll_dy;
	ww_updateRangeAndCheckBounds(wp->minln, wp->cheight, rect.bottom-rect.top,
			&wp->maxln, &wp->maxcursln, &wp->mincursln, &dy); 
			
	ww_updateRangeAndCheckBounds(wp->mincol, wp->cwidth, rect.right - rect.left,
			&wp->maxcol, &wp->maxcurscol, &wp->mincurscol, &wp->scroll_dx); 
}

/*
 * Creates an editor window with the given title, instance count, creation parameter and a textual
 * hint defining the way the window is activated.
 */
HWND ww_createEditWindow(char* pTitle, LPVOID lParam, const char* pszHint) {
	char szBuffer[128];
	OPEN_HINT hHint;
	if (pszHint == NULL) {
		hHint = mainframe_parseOpenHint(NULL);
	} else {
		strcpy(szBuffer, pszHint);
		hHint = mainframe_parseOpenHint(szBuffer);
	}
	return mainframe_addWindow(&hHint, szEditClass, pTitle, lParam);
}

/*--------------------------------------------------------------------------
 * win_getstate()
 * Get the current window placement.
 */
EXPORT void win_getstate(HWND hwnd, WINDOWPLACEMENT* wsp)
{
	wsp->length = sizeof * wsp;
	GetWindowPlacement(hwnd, wsp);
}

/*-----------------------------------------------------------
 * ww_isColumnSelectionMode()
 */
EXPORT int ww_isColumnSelectionMode(WINFO *wp) {
	return (wp->workmode & WM_COLUMN_SELECTION) ? 1 : 0;
}

/*------------------------------------------------------------
 * ww_findwinid2()
 */
EXPORT WINFO *ww_findwinid(int winid)
{	WINFO  *wp;
	
	for (wp = _winlist; wp; wp = wp->next) {
		if (wp->win_id == winid) {
			return wp;
		}
	}
	return NULL;
}

/*--------------------------------------------------------------------------
 * ww_workWinHasFocus()
 * return TRUE, when a work window has the input focus
 */
EXPORT BOOL ww_workWinHasFocus(void) {
	if (!_winlist || GetFocus() != _winlist->ww_handle) {
		return FALSE;
	}
	return TRUE;
}

/*------------------------------------------------------------
 * ww_requestFocusInTopWindow()
 * Request focus in the top most window.
 */
void ww_requestFocusInTopWindow(void)
{
	if (_winlist) {
		SetFocus(_winlist->ww_handle);
	}
}

/*-----------------------------------------------------------
 * ww_redrawAllWindows()
 * Redraw all editor windows. If update is true, not only send
 * a repaint message but also update by painting right away.
 */
void ww_redrawAllWindows(int update)
{	WINFO *wp;

	for (wp = _winlist; wp; wp = wp->next) {
		render_repaintWindow(wp);
		if (update) {
			UpdateWindow(wp->ww_handle);
		}
	}
}

/*-----------------------------------------------------------
 * ww_setwindowtitle()
 * Update the title of a window.
 */
int ww_setwindowtitle(WINFO *wp, BOOL bRepaint) {	
	int nr;
	char buf[512];

	if (!wp->edwin_handle)
		return 1;
	FTABLE* fp = wp->fp;
	nr = wp->win_id;
	if (fp->title) {
		strcpy(buf, fp->title);
	} else {
		char* pName = ft_visibleName(fp);
		wsprintf(buf, "#%d %s", nr, (LPSTR)pName);
		if (ft_isFileModified(fp)) {
			memmove(buf + 2, buf, sizeof buf - 2);
			buf[0] = '*';
			buf[1] = ' ';
		}
	}
	SetWindowText(wp->edwin_handle,buf);
	SHFILEINFO sfi;
	SHGetFileInfo(fp->fname, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof sfi, SHGFI_ICON | SHGFI_SMALLICON);
	SendMessage(wp->edwin_handle, WM_SETICON, ICON_SMALL, (LPARAM)sfi.hIcon);
	if (bRepaint) {
		mainframe_windowTitleChanged();
	}
	return 1;
}

/**
 * A property of our editor document has changed. Update the window appropriately.
 */
int ww_documentPropertyChanged(WINFO* wp, PROPERTY_CHANGE* pChange) {
	return ww_setwindowtitle(wp, TRUE);
}

/*--------------------------------------------------------------------------
 * indent_initTabStopBits()
 */
static void indent_initTabStopBits(INDENTATION* pIndentation) {
	int i, ts;

	ts = 0;
	i = 0;
	for (;;) {
		while (!TABTHERE(pIndentation, ts) && ts < MAXLINELEN)
			ts++;
		if (ts >= DIM(pIndentation->ts))
			return;
		while (i <= ts) {
			if (i >= DIM(pIndentation->ts))
				return;
			pIndentation->ts[i] = ts;
			i++;
		}
		ts++;
	}
}

/*--------------------------------------------------------------------------
 * indent_toggleTabStop()
 */
void indent_toggleTabStop(INDENTATION* indentation, int col) {
	if (TABTHERE(indentation, col))
		TABCLEAR(indentation, col);
	else
		TABPLACE(indentation, col);

	indent_initTabStopBits(indentation);

}

/*--------------------------------------------------------------------------
 * indent_calculateTabStop()
 * calculate the previous tabstop for a column given in screen indentation units.
 * Let's assume, the column is 7 and one has a tab size of 4, then 4 is returned.
 */
int indent_calculatePreviousTabStop(int col, INDENTATION* l) {
	while (col-- > 0 && !TABTHERE(l, col))
		;
	return col;
}

/*--------------------------------------------------------------------------
 * indent_calculateTabStop()
 * calculate next tabstop for a column given in screen indentation units.
 */
int indent_calculateTabStop(int col, INDENTATION* l) {
	if (col < DIM(l->ts) && l->ts[col])
		return l->ts[col];

	while (++col < MAXLINELEN && !TABTHERE(l, col))
		;
	return col;
}

/*--------------------------------------------------------------------------
 * indent_calculateNextTabStop()
 * calculate next tabstop after the given column.
 */
int indent_calculateNextTabStop(int col, INDENTATION* pIndentation) {
	if (col < DIM(pIndentation->ts) && pIndentation->ts[col]) {
		int i = pIndentation->ts[col];
		if (i != col) {
			return i;
		}
	}
	while (++col < MAXLINELEN && !TABTHERE(pIndentation, col))
		;
	return col;
}

/*--------------------------------------------------------------------------
 * ww_tabsChanged()
 * The tab configuration has changed. Update our internal data structures
 * and possibly repaint.
 */
int ww_tabsChanged(WINFO* wp, EDIT_CONFIGURATION* lp) {
	INDENTATION* pIndentation = &wp->indentation;
	int i, nextIndent;
	int ts = lp->tabsize;
	int iTabSizes = 0;
	int tDelta;

	pIndentation->tabsize = ts;
	memset(pIndentation->tbits, 0, sizeof(pIndentation->tbits));
	tDelta = lp->tabulatorSizes[iTabSizes];
	if (tDelta == 0) {
		tDelta = ts;
	}
	else {
		iTabSizes++;
	}
	nextIndent = tDelta;
	for (i = 0, nextIndent = ts; i < MAXLINELEN; i++) {
		if (i == nextIndent) {
			TABPLACE(pIndentation, i);
			tDelta = lp->tabulatorSizes[iTabSizes];
			if (tDelta == 0) {
				tDelta = ts;
			}
			else {
				iTabSizes++;
			}
			nextIndent += tDelta;
		}
	}
	indent_initTabStopBits(pIndentation);
	if (wp->ru_handle != NULL) {
		win_sendRedrawToWindow(wp->ru_handle);
	}
	return 1;
}

static long ww_calculateMaxLine(WINFO* wp) {
	FTABLE* fp = FTPOI(wp);
	return fp->nlines;
}

static long ww_calculateMaxColumn(WINFO* wp, long ln, LINE* lp) {
	return lp->len;
}

static int ww_screenOffsetToBuffer(WINFO* wp, long ln, long col, INTERNAL_BUFFER_POS* pPosition) {
	if (wp->caret.ln == ln && wp->caret.col == col) {
		pPosition->ibp_lineOffset = wp->caret.offset;
		pPosition->ibp_logicalColumnInLine = wp->caret.offset;
		pPosition->ibp_lp = wp->caret.linePointer;
		// for now: just guessing - as this is expensive to calculate and called frequently.
		// will be filled correctly by other renderers, where this information is more relevant.
		pPosition->ibp_byteOffset = ln * 20 + col;
		return 1;
	}
	return 0;
}

static void ww_modelChanged(WINFO* wp, MODEL_CHANGE* pChange) {
}

/**
 * Calculate the length of the longest line on the fly.
 * Dumb version, which does not yet handle tabs / formatting etc.
 * Should cache the result and be notified by the model, when the model
 * changes to remove the cache.
 */
static int ww_calculateLongestLine(WINFO* wp) {
	FTABLE* fp = wp->fp;
	LINE* lp = fp->firstl;
	int max = 0;

	while (lp) {
		int len = caret_lineOffset2screen(wp, &(CARET){lp, lp->len});
		if (max < len) {
			max = len;
		}
		lp = lp->next;
	}
	return max;
}


static int ascii_rendererSupportsMode(int aMode) {
	return 1;
}

static RENDERER _asciiRenderer = {
	render_singleLineOnDevice,
	render_asciiMode,
	caret_placeCursorAndValidate,
	ww_calculateMaxLine,
	ww_calculateLongestLine,
	ww_calculateMaxColumn,
	caret_updateDueToMouseClick,
	ww_screenOffsetToBuffer,
	NULL,
	NULL,
	wt_scrollxy,
	ww_setScrollCheckBounds,
	render_adjustScrollBounds,
	NULL,
	ascii_rendererSupportsMode,
	caret_calculateOffsetFromScreen,
	TRUE,
	ww_modelChanged
};

/*
 * Destroy the data allocated privately by the renderer.
 */
static void ww_destroyRendererData(WINFO* wp) {
	if (wp->r_data && wp->renderer) {
		if (wp->renderer->r_destroy) {
			wp->renderer->r_destroy(wp);
		}
		else {
			free(wp->r_data);
		}
		wp->r_data = NULL;
	}
}

/*
 * Assign a new renderer. 
 */
static void ww_assignRenderer(WINFO* wp) {
	RENDERER* pOld = wp->renderer;
	RENDERER* pNew = NULL;
	FTABLE* fp = wp->fp;
	if (wp->dispmode & SHOWWYSIWYG) {
		const char* pRenderer = grammar_wysiwygRenderer(fp->documentDescriptor->grammar);
		if (pRenderer) {
			pNew = (RENDERER*)hashmap_get(_renderers, (intptr_t)pRenderer);
		}
	}
	if (!pNew && wp->dispmode & SHOWHEX) {
		pNew = (RENDERER*)hashmap_get(_renderers, (intptr_t)"hex");
	}
	if (!pNew) {
		pNew = &_asciiRenderer;
	}
	if (pNew == pOld) {
		return;
	}
	ww_destroyRendererData(wp);
	wp->renderer = pNew;
	wp->maxVisibleLineLen = -1;
	if (wp->renderer->r_create) {
		wp->r_data = wp->renderer->r_create(wp);
	}
	wp->actionContext[0] = 0;
	if (wp->renderer->r_context) {
		strcpy(wp->actionContext, wp->renderer->r_context);
	}
	if (!wp->actionContext[0]) {
		strcpy(wp->actionContext, fp->documentDescriptor->actionContext);
	}
	if (pOld) {
		SendMessage(wp->edwin_handle, WM_EDWINREORG, 0, 0L);
	}
}

/*
 * The display / workmode of a window has changed - update appropriately.
 */
void ww_modeChanged(WINFO* wp) {
	ww_assignRenderer(wp);
	SendMessage(wp->edwin_handle, WM_EDWINREORG, 0, 0L);
	if (wp->ww_handle) {
		sl_size(wp);
		font_selectStandardFont(wp->ww_handle, wp);
		wt_tcursor(wp, 0);
		wt_tcursor(wp, 1);
		caret_placeCursorForFile(wp, wp->caret.ln, wp->caret.offset, wp->caret.col, 0);
	}

	wp->scroll_dx = 4;
	wp->renderer->r_windowSizeChanged(wp);
	wp->renderer->r_adjustScrollBounds(wp);
	win_sendRedrawToWindow(wp->ww_handle);
	render_updateCaret(wp);
}

/*
 * Force a window to use a new display mode.
 */
void ww_changeDisplayMode(WINFO* wp, int nNewDisplayMode) {
	if (wp->dispmode != nNewDisplayMode) {
		wp->dispmode = nNewDisplayMode;
		ww_modeChanged(wp);
		op_updateall();
	}
}

/*-----------------------------------------------------------
 * ww_applyDisplayProperties()
 * Apply all flags from the document descriptor of the edited document on the 
 * window referred to by the passed pointer.
 */
void ww_applyDisplayProperties(WINFO* wp) {
	FTABLE* fp = wp->fp;
	EDIT_CONFIGURATION* linp = fp->documentDescriptor;

	wp->dispmode = linp->dispmode;
	wp->workmode = linp->workmode;
	wp->tabDisplayFillCharacter = linp->tabDisplayFillCharacter;
	if (wp->highlighter) {
		highlight_destroy(wp->highlighter);
	}
	wp->statusline = linp->statusline[0] ? linp->statusline : NULL;
	wp->scrollflags = linp->scrollflags;
	wp->cursaftersearch = linp->cursaftersearch;
	wp->vscroll = linp->vscroll;
	wp->scroll_dy = linp->scroll_dy;
	wp->hscroll = linp->hscroll;
	wp->lmargin = linp->lmargin;
	wp->rmargin = linp->rmargin;
	ww_tabsChanged(wp, linp);
	wp->highlighter = highlight_createHighlighter(linp->grammar);
	ww_modeChanged(wp);
}

/*
 * Close a window 
 */
static int ww_recycleWindow() {
	WINFO* wpFound = NULL;
	for (WINFO* wp = _winlist; wp; wp = wp->next) {
		if (!(wp->workmode & WM_STICKY) && wp->fp && !ft_isFileModified(wp->fp)) {
			wpFound = wp;
		}
	}
	if (wpFound) {
		ww_close(wpFound);
		return 1;
	}
	return 0;
}

/*
 * Connect a view with a file - set the model and add the view as a dependent. 
 */
void ft_connectViewWithFT(FTABLE* fp, WINFO* wp) {
	ARRAY_LIST* pViews;
	if ((pViews = fp->views) == NULL) {
		pViews = arraylist_create(3);
		fp->views = pViews;
	}
	arraylist_add(pViews, wp);
	wp->fp = fp;
	// Ensure caret does not point into nirwana.
	wp->caret.linePointer = fp->firstl;
	wp->caret.col = 0;
}

/*
 * Returns the indices of the first selected line and the last selected line.
 * If no selection exists, return 0 and return -1 in pFirstIndex and pLastIndex.
 */
int ww_getSelectionLines(WINFO* wp, long* pFirstIndex, long* pLastIndex) {
	*pFirstIndex = -1;
	*pLastIndex = -1;
	if (wp->blstart && wp->blend) {
		*pFirstIndex = ln_indexOf(wp->fp, wp->blstart->m_linePointer);
		if (wp->blstart->m_linePointer == wp->blend->m_linePointer) {
			*pLastIndex = *pFirstIndex;
		} else {
			int nDelta = ll_indexOf((LINKED_LIST*)wp->blstart->m_linePointer, (LINKED_LIST*)wp->blend->m_linePointer);
			if (wp->blend->m_column == 0) {
				nDelta--;
			}
			*pLastIndex = nDelta +*pFirstIndex;
		}
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------
 * ww_new()
 */
static int nwindows;
static FSTYLE _fstyles[2] = {
	5,			BLACK_BRUSH,	/* Or */
	5,			BLACK_BRUSH		/* Invert */
};
static WINFO *ww_new(FTABLE *fp,HWND hwnd) {
	WINFO  *wp;

	int nMax = GetConfiguration()->maximumNumberOfOpenWindows;
	while (nMax > 0 && nwindows >= nMax) {
		if (!ww_recycleWindow()) {
			break;
		}
	}
	if ((wp = ll_insert(&_winlist,sizeof *wp)) == 0) {
		return 0;
	}

	wp->markstyles[FS_XMARKED] = _fstyles[0];
	wp->markstyles[FS_BMARKED] = _fstyles[1];
	ft_connectViewWithFT(fp, wp);
	wp->edwin_handle = hwnd;

	ww_applyDisplayProperties(wp);
	wp->zoomFactor = 1.0;
	wp->win_id = ++nwindows;
	wp->maxVisibleLineLen = -1;
	return wp;
}

/*
 * Invoke a callback for every view of a editor document model.
 * The callback may return 0 to abort the iteration process.
 * The callback is invoked with the WINFO pointer an an optional parameter
 * passed as the last argument.
 */
void ft_forAllViews(FTABLE* fp, int (*callback)(WINFO* wp, void* pParameterPassed), void* parameter) {
	ARRAY_LIST* pViews;
	if ((pViews = fp->views) == NULL) {
		return;
	}
	ARRAY_ITERATOR pIter = arraylist_iterator(pViews);
	while (pIter.i_buffer < pIter.i_bufferEnd) {
		WINFO* wp = *pIter.i_buffer;
		if (!(*callback)(wp, parameter)) {
			return;
		}
		pIter.i_buffer++;
	}
}

/*
 * Invoked, when one of the views viewing on this document is closed.
 */
static void ww_windowClosed(WINFO* wp) {
	FTABLE* fp = wp->fp;
	ARRAY_LIST* pList = fp->views;
	if (pList == NULL) {
		return;
	}
	arraylist_remove(pList, wp);
	if (arraylist_size(pList) == 0) {
		arraylist_destroy(fp->views);
		fp->views = NULL;
		ft_destroy(fp);
	}
}

/**
 * Checks, whether the given window is a view of the file
 */
BOOL ft_hasView(FTABLE* fp, WINFO* wp) {
	if (fp->views == NULL) {
		return FALSE;
	}
	return arraylist_indexOf(fp->views, wp) >= 0;
}

/*
 * Return the primary view displaying a file - if any. 
 */
WINFO* ft_getPrimaryView(FTABLE* fp) {
	ARRAY_LIST* pList = fp->views;
	if (pList == NULL) {
		return NULL;
	}
	return arraylist_get(pList, 0);
}

static void ww_detachFromComparison(WINFO* wp) {
	FTABLE* fp = wp->fp;
	LINE* lp = fp->firstl;
	ln_removeFlag(lp, NULL, LN_COMPARE_DIFFERENT);
	wp->comparisonLink = NULL;
	SendMessage(wp->edwin_handle, WM_EDWINREORG, 0, 0L);
	render_repaintAllForFile(fp);
}

/*
 * One window of two windows being compared with each other is closed. Perform the proper cleanup. 
 */
void ww_releaseComparisonLink(WINFO* wp, BOOL bDetachSource) {
	COMPARISON_LINK* cpl = wp->comparisonLink;
	if (cpl == NULL) {
		return;
	}
	if (bDetachSource || wp != cpl->cl_wpLeft) {
		ww_detachFromComparison(cpl->cl_wpLeft);
	}
	if (bDetachSource || wp != cpl->cl_wpRight) {
		ww_detachFromComparison(cpl->cl_wpRight);
	}
	free(cpl);
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
}

/*
 * Connect two windows with a comparison link. This is used to allow for synchronized scrolling
 * etc to provide a consistent view on the differences of two files.
 */
void ww_connectWithComparisonLink(WINFO* wp1, WINFO* wp2) {
	if (wp1->comparisonLink) {
		if (wp1->comparisonLink->cl_wpLeft == wp1 && wp1->comparisonLink->cl_wpRight == wp2) {
			return;
		}
		ww_releaseComparisonLink(wp1, FALSE);
	}
	if (wp2->comparisonLink) {
		ww_releaseComparisonLink(wp2, FALSE);
	}
	COMPARISON_LINK* pLink = calloc(1, sizeof * pLink);
	pLink->cl_wpLeft = wp1;
	pLink->cl_wpRight = wp2;
	pLink->cl_synchronizeCaret = TRUE;
	wp1->comparisonLink = pLink;
	wp2->comparisonLink = pLink;
	SendMessage(wp1->edwin_handle, WM_EDWINREORG, 0, 0L);
	SendMessage(wp2->edwin_handle, WM_EDWINREORG, 0, 0L);
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
}

/*-----------------------------------------------------------
 * ww_destroy()
 */
void ww_destroy(WINFO *wp) {	
	int   nId;

	if (wp == NULL) {
		return;
	}
	if (wp->fp != NULL) {
		ww_windowClosed(wp);
	}
	if (wp->comparisonLink != NULL) {
		ww_releaseComparisonLink(wp, FALSE);
	}
	if (wp->highlighter) {
		highlight_destroy(wp->highlighter);
	}
	ww_destroyRendererData(wp);
	ll_destroy((LINKED_LIST**)&wp->fmark, (int (*)(void* elem))0);
	wp->blstart = 0;
	wp->blend = 0;
	wp->fp = NULL;
	nId = wp->win_id;
	SetWindowLongPtr(wp->ww_handle, GWL_WWPTR, 0);
	SetWindowLongPtr(wp->edwin_handle, GWL_VIEWPTR, 0);
	if (!ll_delete(&_winlist,wp)) {
		EdTRACE(log_errorArgs(DEBUG_ERR,"failed deleting window props"));
	}

	/* update window handles */
	for (wp = _winlist; wp; wp = wp->next) {
		if (wp->win_id > nId) {
			wp->win_id--;
			ww_setwindowtitle(wp, FALSE);
		}
	}
	mainframe_windowTitleChanged();
	nwindows--;
	EdTRACE({
		if (nwindows == 0 && _winlist)
			log_errorArgs(DEBUG_ERR,"bad winlist");
	});
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
}

/*-----------------------------------------------------------
 * ww_getNumberOfOpenWindows()
 * Returns the number of currently open windows.
 */
int ww_getNumberOfOpenWindows(void)
{
	return nwindows;
}

/*------------------------------------------------------------
 * ww_close()
 * Close the passed editor window.
 */
int ww_close(WINFO *wp)
{	HWND hwndEdit;

	if (wp == NULL) {
		return 0;
	}
	hwndEdit = wp->edwin_handle;

	if (!SendMessage(hwndEdit, WM_QUERYENDSESSION, (WPARAM)0, (LPARAM)0L)) {
		return 0;
	}
	return SendMessage(hwndEdit, WM_CLOSE, 0, 0) != 0;
}

/*
 * Returns the "active" editor window having the focus. 
 */
WINFO* ww_getCurrentEditorWindow() {
	return _winlist;
}

/*
 * Returns the WINFO data structure for a HWND. 
 */
WINFO* ww_getWinfoForHwnd(HWND hwnd) {
	return (WINFO*)GetWindowLongPtr(hwnd, GWL_VIEWPTR);
}

/*------------------------------------------------------------
 * EditWndProc()
 */
WINFUNC EditWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	) {
	WINFO * wp = ww_getWinfoForHwnd(hwnd);

	if (message == WM_CREATE || wp != NULL)
   	switch(message) {
	case WM_CREATE:
		{
		XYWH xyWork,xyRuler,xyLineInfo;
		LPCREATESTRUCT pCreate = (LPCREATESTRUCT)lParam;
		FTABLE* fp = pCreate->lpCreateParams;
		if ((wp = ww_new(fp, hwnd)) == 0) {
			DestroyWindow(hwnd);
			return 0;
		}

		ShowWindow(hwnd, SW_HIDE);
		ww_createSubWindows(hwnd, wp, &xyWork, &xyRuler, &xyLineInfo);
		ww_setwindowtitle(wp, TRUE);
		SetWindowLongPtr(hwnd,GWL_ICPARAMS, (LONG_PTR)fp->fname);
		SetWindowLongPtr(hwnd,GWL_VIEWPTR, (LONG_PTR) wp);
		return 0;
		}

	case WM_SETFOCUS: {
		if (!IsIconic(hwnd)) {
			SetFocus(wp->ww_handle);
		}
		break;
	}

	case WM_MOVE:
		/* drop through */
	case WM_SIZE:
		if (message == WM_MOVE)
			break;
		if (wParam == SIZEICONIC) {
			break;
		}

	case WM_EDWINREORG:
		{
		XYWH xyWork, xyRuler, xyLineWindowSize;

		ww_createSubWindows(hwnd, wp,&xyWork,&xyRuler, &xyLineWindowSize);
		if (wp->ww_handle) {
			MoveWindow(wp->ww_handle,xyWork.x,xyWork.y,
					xyWork.w,xyWork.h,1);
		}
		if (wp->ru_handle) {
			MoveWindow(wp->ru_handle,xyRuler.x,xyRuler.y,
					xyRuler.w,xyRuler.h,1);
		}
		if (wp->lineNumbers_handle) {
			MoveWindow(wp->lineNumbers_handle, xyLineWindowSize.x, xyLineWindowSize.y,
				xyLineWindowSize.w, xyLineWindowSize.h, 1);
		}
		if (message == WM_EDWINREORG) {
			if (wp->ru_handle) {
				win_sendRedrawToWindow(wp->ru_handle);
			}
			return 1;
		}
		if (wp->ru_handle) {
			RedrawWindow(wp->ru_handle, NULL, NULL, RDW_INVALIDATE);
		}
		break;
		}
	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if (!ww_requestToClose(wp)) {
			return 0;
		}
 		break;

	case WM_DESTROY:
		// Destroy the view window - the document should be destroyed only if this is the last window.
		ww_destroy(wp);
		if (!ww_getNumberOfOpenWindows()) {
			menu_selectActionContext(DEFAULT_ACTION_CONTEXT);
		}
		return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

/*------------------------------------------------------------
 * ww_updateWindowBounds()
 */
static int ww_updateWindowBounds(WINFO *wp, int w, int h) {
	wp->renderer->r_windowSizeChanged(wp);
	EdTRACE(log_errorArgs(DEBUG_TRACE,"set window scroll bounds to (minln = %ld, mincol = %ld, maxln = %ld, maxcol = %ld)",
		   wp->minln,wp->mincol,wp->maxln,wp->maxcol));
	return 1;
}

/*----------------------------------------------------------
 * do_mouse()
 */
int do_mouse(HWND hwnd, int nClicks, UINT message, WPARAM wParam, LPARAM lParam) {
	int			ret;
	int			button = 0;
	int			nModifier;
	int			x;
	int			y;
	WINFO *		wp;

	if ((wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON)) == 0) {
		return 0;
	}
	if ((wp = (WINFO *) GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0) {
		x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
	} else {
		return 0;
	}

	if (wParam & MK_RBUTTON) {
		button |= MBUT_R;
	}
	if (wParam & MK_MBUTTON) {
		button |= MBUT_M;
	}
	if (wParam & MK_LBUTTON) {
		button |= MBUT_L;
	}

	nModifier = 0;
	if (wParam & MK_CONTROL) {
		nModifier |= K_CONTROL;
	}
	if (wParam & MK_SHIFT) {
		nModifier |= K_SHIFT;
	}
	if (GetKeyState(VK_MENU) < 0) {
		nModifier |= K_ALTERNATE;
	}
	ret = mouse_onMouseClicked(wp, x, y, button, nClicks, nModifier);
	return ret;
}

/*------------------------------------------------------------
 * do_slide()
 */
static int do_slide(WINFO *wp, UINT message, int nScrollCode, int nScrollPos) {	
	int 	delta;
	int		deltapage;

	if (message == WM_HSCROLL) {
		deltapage = (wp->maxcol - wp->mincol);
	} else {
		deltapage = (wp->maxln - wp->minln);
	}
	
	switch(nScrollCode) {
		case SB_LINEUP:	delta = -1; break;
		case SB_LINEDOWN:	delta =  1; break;
		case SB_PAGEUP:	delta = -deltapage; break;
		case SB_PAGEDOWN:	delta =  deltapage; break;
		case SB_THUMBTRACK:
			if ((wp->scrollflags & SC_THUMBTRACK) == 0)
				return 0;
		case SB_THUMBPOSITION:	
			delta = nScrollPos -
					((message == WM_VSCROLL) ? 
						wp->minln : wp->mincol);
			break;
		default: return 0;
	}
	
	sl_moved(wp,(message == WM_HSCROLL) ? 0L : (long)delta,
			  (message == WM_HSCROLL) ? (long)delta : 0L, 
			  (wp->scrollflags & SC_CURSORCATCH));
	return 1;
}

/*
 * Button down handler, which will helps listening to tripple clicks.
 */
static void onButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static RECT _lastMouseClickRect = {0};
	static DWORD _tmLastClick = 0;
	static int _cClicks = 0;
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	POINT pt = { x, y };

	DWORD tmClick = GetMessageTime();
	if (message == WM_LBUTTONDBLCLK || message == WM_RBUTTONDBLCLK || message == WM_MBUTTONDBLCLK) {
		_cClicks = 1; 
	} else if (!PtInRect(&_lastMouseClickRect, pt) ||
		tmClick - _tmLastClick > GetDoubleClickTime()) {
		_cClicks = 0;
	}
	_cClicks++;

	_tmLastClick = tmClick;
	SetRect(&_lastMouseClickRect, x, y, x, y);
	InflateRect(&_lastMouseClickRect,
		GetSystemMetrics(SM_CXDOUBLECLK) / 2,
		GetSystemMetrics(SM_CYDOUBLECLK) / 2);
	do_mouse(hwnd, _cClicks, message, wParam, lParam);
}

/*------------------------------------------------------------
 * WorkAreaWndProc()
 */
static WINFUNC WorkAreaWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam) {
	WINFO *	wp;
	int zDelta;

	switch(message) {
	case WM_CREATE:
		{
		wp = (WINFO *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, GWL_WWPTR, (LONG_PTR) wp);
		wp->ww_handle = hwnd;
		font_selectStandardFont(hwnd, wp);
		return 0;
		}

	case WM_SETCURSOR: {
		// wParam == 0 -> posted by PKSEdit itself.
		if (wParam == 0 || LOWORD(lParam) == HTCLIENT) {
			if (GetAsyncKeyState(VK_CONTROL)) {
				mouse_setHandCursor();
			} else {
				if ((wp = (WINFO*)GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0 && !wp->renderer->r_canEdit) {
					mouse_setArrowCursor();
				}
				else {
					mouse_setDefaultCursor();
				}
			}
		} else {
			mouse_setArrowCursor();
		}
		return 1;
	}
		

	case WM_COMMAND: {
		int nCommand = (int)wParam;
		wp = (WINFO*)GetWindowLongPtr(hwnd, GWL_WWPTR);
		POINT pt = menu_getContextMenuPopupPosition();
		if (nCommand && macro_onMenuAction(wp, nCommand, &pt)) {
			return 1;
		}
	}
	// otherwise drop through
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_MENUSELECT:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_CHAR:
		return SendMessage(hwndMain,message,wParam,lParam);

	case WM_MOUSEMOVE:
		if ((wp = (WINFO*)GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0) {
			if (wp->renderer->r_mouseMove) {
				int xPos = GET_X_LPARAM(lParam);
				int yPos = GET_Y_LPARAM(lParam);
				wp->renderer->r_mouseMove(wp, xPos, yPos);
			}
		}
		break;

	case WM_MOUSEWHEEL:
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if ((wp = (WINFO*)GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0) {
			if (GetAsyncKeyState(VK_CONTROL)) {
				ww_zoomWindow(zDelta > 0);
			} else {
				long dy = zDelta > 0 ? -1 : 1;
				if (GetAsyncKeyState(VK_SHIFT)) {
					dy *= 4;
				}
				long dx = 0;
				sl_scrollwinrange(wp, &dy, &dx);
				sl_winchanged(wp, dy, dx);
			}
		}
		return TRUE;


	case WM_LBUTTONDOWN:
		if (GetFocus() != hwnd) {
			SetFocus(hwnd);
		}
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		onButtonDown(hwnd, message, wParam, lParam);
		return 0;

	case WM_HSCROLL:
	case WM_VSCROLL:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0) {
			int nCode = LOWORD(wParam);
			int nPos = HIWORD(wParam);
			if (nCode == SB_THUMBTRACK || nCode == SB_THUMBPOSITION) {
				SCROLLINFO info;
				ZeroMemory(&info, sizeof info);
				info.cbSize = sizeof info;
				info.fMask = SIF_TRACKPOS;
				if (!GetScrollInfo(hwnd, message == WM_HSCROLL ? SB_HORZ : SB_VERT, &info)) {
					break;
				}
				nPos = info.nTrackPos;
				log_debug1("thumbtrack getscrollinfo returns: %d", nPos);
			}
			if (do_slide(wp,message,nCode, nPos) == 0) {
				break;
			}
		} else {
			EdTRACE(log_errorArgs(DEBUG_TRACE,"WM_SLIDE in WorkWndProc without file"));
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_PAINT:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0) {
		   render_paintWindow(wp);
		}
		return 0;

	case WM_SIZE:
		if ((wp = (WINFO *) GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0) {
			if (!ww_updateWindowBounds(wp, LOWORD(lParam), HIWORD(lParam))) {
				return 0;
			}
			sl_size(wp);
	    }
	    break;

	case WM_KILLFOCUS: {
			if ((wp = (WINFO*)GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0) {
				wt_tcursor(wp, 0);
			}
		}
		break;

	case WM_SETFOCUS: {
		WINFO* wpOld = _winlist;
		if ((wp = (WINFO*)GetWindowLongPtr(hwnd, GWL_WWPTR)) != 0) {
			ll_moveElementToFront((LINKED_LIST**)&_winlist, wp);
			mainframe_windowActivated(wpOld != NULL ? wpOld->edwin_handle : NULL, wp->edwin_handle);
			wt_tcursor(wp, 1);
			FTABLE* fp = wp->fp;
			ft_currentFileChanged(fp);
		} else {
			EdTRACE(log_errorArgs(DEBUG_TRACE,
				"WM_SETFOCUS in WorkWndProc without file"));
		}
		return 0;
	}

	case WM_DESTROY:
		SetWindowLongPtr(hwnd, GWL_WWPTR, 0);
		return 0;

    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

/*----------------------------*/
/* mouse_onRulerClicked()			*/
/*----------------------------*/
static int mouse_onRulerClicked(WINFO* wp, int x, int y, int msg, int shift) {
	long 	ln;
	long		col;
	char		szBuf[100];
	FTABLE* fp = wp->fp;

	x -= ruler_getLeft(wp);
	wp->renderer->r_hitTest(wp, x + wp->cwidth / 2, y, &ln, &col);
	wsprintf(szBuf, /*STR*/"SPALTE: %4ld", col + 1);
	st_setStatusLineMessage(szBuf);

	if (msg == WM_RBUTTONDOWN) {
		wp->rmargin = col;
		fp->documentDescriptor->rmargin = col;
	} else {
		// TODO: update the editor configuration and possibly save to make changes persistent.
		indent_toggleTabStop(&wp->indentation, col);
	}
	render_repaintAllForFile(fp);
	win_sendRedrawToWindow(wp->ru_handle);

	return 1;
}


/*--------------------------------------------------------------------------
 * draw_ruler()
 * Does the actual painting of the ruler.
 */
static void draw_ruler(WINFO *wp) {
	int 			col;
	int			xPos;
	int			rmargin;
	int			width;
	int			nMiddle;
	RECT			rect;
	FTABLE *		fp = wp->fp;
	HDC 		hdc;
	PAINTSTRUCT ps;
	THEME_DATA* pTheme = theme_getCurrent();

	memset(&ps, 0, sizeof ps);
	hdc = BeginPaint(wp->ru_handle, &ps);
	GetClientRect(wp->ru_handle, &rect);
	HBRUSH bgBrush = CreateSolidBrush(pTheme->th_rulerBackgroundColor);
	FillRect(hdc, &ps.rcPaint, bgBrush);
	DeleteObject(bgBrush);
	nMiddle = rect.bottom / 2 - 3;
	width = wp->cwidth;
	rmargin = ww_getRightMargin(wp);

	HPEN fatMarkerPen = CreatePen(PS_SOLID, 3, pTheme->th_rulerForegroundColor);
	HPEN markerPen = CreatePen(PS_SOLID, 1, pTheme->th_rulerForegroundColor);
	HPEN hPenOld = SelectObject(hdc, markerPen);
	xPos = ruler_getLeft(wp);
	int xMin = ps.rcPaint.left - 20;
	int xMax = ps.rcPaint.right + 20;
	for (col = wp->mincol; col <= wp->maxcol; col++, xPos += width) {
		if (xPos < xMin || xPos > xMax) {
			continue;
		}
		if (TABTHERE((&wp->indentation),col) || col == wp->lmargin || col == wp->rmargin) {
			MoveTo(hdc, xPos, nMiddle - 3);
			if (col == wp->lmargin || col == rmargin) {
				SelectObject(hdc, fatMarkerPen);
			}
			else {
				SelectObject(hdc, markerPen);
			}
			LineTo(hdc, xPos, nMiddle + 3);
		}
	}
	HPEN borderPen = CreatePen(PS_SOLID, 1, pTheme->th_rulerBorderColor);
	SelectObject(hdc, borderPen);
	MoveTo(hdc, rect.left, rect.bottom-1);
	LineTo(hdc, rect.right, rect.bottom-1);
	SelectObject(hdc, hPenOld);
	DeleteObject(fatMarkerPen);
	DeleteObject(borderPen);
	DeleteObject(markerPen);
	EndPaint(wp->ru_handle,&ps);
}

/*------------------------------------------------------------
 * RulerWndProc()
 * Window Procedure for handling the ruler.
 */
static WINFUNC RulerWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
) {
	WINFO* wp;

	switch(message) {

	case WM_ERASEBKGND:
		return 1;
	case WM_PAINT:
			if ((wp = (WINFO *)GetWindowLongPtr(hwnd,0)) != 0) {
		   		draw_ruler(wp);
			} else {
				EdTRACE(log_errorArgs(DEBUG_TRACE,"WM_PAINT in RulerWndProc without file"));
			}
			break;

		case WM_CREATE:
			wp = (WINFO*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			SetWindowLongPtr(hwnd, 0, (LONG_PTR)wp);
			return 0;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			if ((wp = (WINFO *) GetWindowLongPtr(hwnd,0)) != 0) {
				mouse_onRulerClicked(wp, (int)LOWORD(lParam), (int)HIWORD(lParam), 
					(int)message, (int)wParam);
			}
			return 0;

		case WM_DESTROY:
			return 0;
	}
	return (DefWindowProc(hwnd, message, wParam, lParam));
}

/*--------------------------------------------------------------------------
 * draw_LineNumbers()
 * Does the painting of the line numbers.
 */
static void draw_lineNumbers(WINFO* wp) {
	int 		row;
	int			yPos;
	RECT			rect;
	size_t		textLen;
	FTABLE* fp = wp->fp;
	int maxln = wp->maxln;
	EDIT_CONFIGURATION* lin = fp->documentDescriptor;
	HDC 		hdc;
	PAINTSTRUCT ps;
	THEME_DATA* pTheme = theme_getCurrent();

	hdc = BeginPaint(wp->lineNumbers_handle, &ps);
	GetClientRect(wp->lineNumbers_handle, &rect);
	HBRUSH bgBrush = CreateSolidBrush(pTheme->th_rulerBackgroundColor);
	HBRUSH hAnnotationBrush = CreateSolidBrush(pTheme->th_changedLineColor);
	HBRUSH hSavedBrush = CreateSolidBrush(pTheme->th_savedChangedLineColor);
	HBRUSH hUndoAfterSavedBrush = CreateSolidBrush(pTheme->th_undoAfterSaveChangedLineColor);
	FillRect(hdc, &ps.rcPaint, bgBrush);
	DeleteObject(bgBrush);

	font_selectFontStyle(pTheme, wp, 0, hdc);
	SetTextColor(hdc, pTheme->th_rulerForegroundColor);
	SetBkMode(hdc, TRANSPARENT);
	int padding = 3;
	char text[20];
	if (maxln > fp->nlines-1) {
		maxln = fp->nlines-1;
	}
	LINE* lp = ww_getMinLine(wp, wp->minln);
	int nRightPadding = LINE_ANNOTATION_WIDTH + (2 * LINE_ANNOATION_PADDING);
	if (wp->comparisonLink != NULL) {
		nRightPadding += COMPARISON_ANNOTATION_WIDTH;
	}
	for (yPos = rect.top, row = wp->minln; lp && row <= maxln && yPos < rect.top+rect.bottom; row++, yPos += wp->cheight) {
		if (yPos + wp->cheight < ps.rcPaint.top) {
			lp = lp->next;
			continue;
		}
		if (yPos > ps.rcPaint.bottom+wp->cheight) {
			break;
		}
		RECT textRect;
		textRect.top = yPos;
		textRect.bottom = yPos + wp->cheight;
		if (wp->comparisonLink != NULL && (lp->lflg & LN_COMPARE_DIFFERENT) != 0) {
			HBRUSH bgBrush2;
			char* pszText;
			if (lp->lflg & LN_COMPARE_MODIFIED) {
				pszText = "±";
				bgBrush2 = CreateSolidBrush(pTheme->th_compareModifiedColor);
			} else if (lp->lflg & LN_COMPARE_DELETED) {
				pszText = "-";
				bgBrush2 = CreateSolidBrush(pTheme->th_compareDeletedColor);
			} else {
				pszText = "+";
				bgBrush2 = CreateSolidBrush(pTheme->th_compareAddedColor);
			}
			textRect.left = rect.right - nRightPadding + 4;
			textRect.right = textRect.left + COMPARISON_ANNOTATION_WIDTH - 2;
			FillRect(hdc, &textRect, bgBrush2);
			DeleteObject(bgBrush2);
			SetTextColor(hdc, pTheme->th_dialogForeground);
			DrawText(hdc, pszText, 1, &textRect, DT_CENTER);
			SetTextColor(hdc, pTheme->th_rulerForegroundColor);
		}
		textRect.left = rect.left + padding;
		textRect.right = rect.right - nRightPadding;
		sprintf(text, "%d:", row + 1);
		textLen = strlen(text);
		DrawText(hdc, text, (int)textLen, &textRect, DT_RIGHT|DT_END_ELLIPSIS);
		if (lp->lflg & LNMODIFIED) {
			RECT r;
			r.left = rect.right - LINE_ANNOTATION_WIDTH - LINE_ANNOATION_PADDING;
			r.right = rect.right - 2;
			r.top = yPos;
			r.bottom = min(ps.rcPaint.bottom, yPos + wp->cheight);
			HBRUSH hBrush = hAnnotationBrush;
			if (lp->lflg & LNUNDO_AFTER_SAVE) {
				hBrush = hUndoAfterSavedBrush;
			} else if (lp->lflg & LNSAVED) {
				hBrush = hSavedBrush;
			}
			FillRect(hdc, &r, hBrush);
		}
		lp = lp->next;
	}
	HPEN markerPen = CreatePen(PS_SOLID, 1, pTheme->th_rulerBorderColor);
	HPEN hPenOld = SelectObject(hdc, markerPen);
	MoveTo(hdc, rect.right-1, rect.top);
	LineTo(hdc, rect.right-1, rect.bottom);
	SelectObject(hdc, hPenOld);
	DeleteObject(markerPen);
	DeleteObject(hUndoAfterSavedBrush);
	DeleteObject(hAnnotationBrush);
	DeleteObject(hSavedBrush);
	EndPaint(wp->lineNumbers_handle, &ps);
}

/*------------------------------------------------------------
 * LineNumberWndProc()
 * Window Procedure for handling the ruler.
 */
static WINFUNC LineNumberWndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
) {
	WINFO* wp;

	switch (message) {

	case WM_ERASEBKGND:
		return 1;

	case WM_PAINT:
		if ((wp = (WINFO*)GetWindowLongPtr(hwnd, 0)) != 0) {
			draw_lineNumbers(wp);
		}
		else {
			EdTRACE(log_errorArgs(DEBUG_TRACE, "WM_PAINT in LineNumberProc without WINFO pointer"));
		}
		break;

	case WM_CREATE:
		wp = (WINFO*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hwnd, 0, (LONG_PTR)wp);
		return 0;

	case WM_DESTROY:
		return 0;
	}
	return (DefWindowProc(hwnd, message, wParam, lParam));
}

/*-----------------------------------------------------------
 * ww_register()
 * Register the window classes for PKS edit editor windows.
 */
int ww_register(void) {
	if (!win_registerWindowClass(szWorkAreaClass, WorkAreaWndProc,
		(LPSTR)IDC_IBEAM, GetStockObject(WHITE_BRUSH), 0,
		sizeof(void*)) ||
		!win_registerWindowClass(szEditClass, EditWndProc,
			(LPSTR)IDC_ARROW, NULL, "Edit",
			GWL_VIEWPTR + sizeof(void*)) ||
		!win_registerWindowClass(szRulerClass, RulerWndProc,
			(LPSTR)IDC_CROSS, NULL, NULL,
			sizeof(void*)) ||
		!win_registerWindowClass(szLineNumbersClass, LineNumberWndProc,
			(LPSTR)IDC_ARROW, NULL, NULL,
			sizeof(void*))
#if 0
		||
		!win_registerWindowClass(szStatusClass, StatusWndProc,
			(LPSTR)IDC_ARROW, NULL, 0,
			2 * sizeof(LONG))
#endif
		) {
		return 0;
	}
	ww_registerRenderer("default", &_asciiRenderer);
	ww_registerRenderer("hex", hex_getRenderer());
	ww_registerRenderer("markdown", mdr_getRenderer());
	return 1;
}

/*
 * Final cleanup of the editor window sub-system.
 */
void ww_destroyAll() {
	hashmap_destroy(_renderers, NULL);
	_renderers = NULL;
}
