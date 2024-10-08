/*
 * EditorWindow.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: terminal emulation stuff
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#include "customcontrols.h"
#include "caretmovement.h"
#include "documentmodel.h"
#include "winfo.h"
#include "trace.h"
#include "edierror.h"

#define MAXCOL		FBUFSIZE/2

extern void 	caret_extendSelection(WINFO *wp);
extern void 	st_redraw(BOOL bErase);

typedef enum { CUR_HIDDEN, CUR_INSERT, CUR_OVERRIDE } CURSOR_TYPE;

/*------------------------------------------------------------
 * caret_hide()
 */
static int caret_hide(HWND hwnd)
{
     HideCaret(hwnd);
     DestroyCaret();
     return 1;
}

/*------------------------------------------------------------
 * caret_updatePositionAndVisibility()
 */
static int caret_updatePositionAndVisibility(WINFO *wp, CURSOR_TYPE type, int visible) {
     if (!visible) {
		if (type == CUR_OVERRIDE) {
			CreateCaret(wp->ww_handle,NULL,wp->cwidth, wp->cheight);
		} else {
			CreateCaret(wp->ww_handle, NULL, 2, wp->cheight);
		}
	}

	SetCaretPos(wp->cx,wp->cy);

	if (!visible)
		ShowCaret(wp->ww_handle);

	return 1;
}

/*
 * Update the x and y coordinates in which the caret is shown and optionally its sizes. 
 */
static void render_updateCaretUIPosition(WINFO* wp) {
	wp->cx = (int)(wp->caret.col - wp->mincol) * wp->cwidth;
	wp->cy = (int)(wp->caret.ln - wp->minln) * wp->cheight;
}

/*------------------------------------------------------------
 * render_updateCaret()
 * macro_updateSelectedMacro the current caret for the passed editor window (dependening on insert mode
 * etc...)
 */
static struct olc {
	CURSOR_TYPE type;
	int cheight;
} _olcurs = { CUR_HIDDEN };
void render_updateCaret(WINFO *wp) {
	struct olc *op = &_olcurs;
	CURSOR_TYPE type;

	type = wp->ctype;
	if (type) {
		type = (wp->workmode & WM_INSERT) ?
			CUR_INSERT : CUR_OVERRIDE;

	}

	if (wp->renderer->r_updateCaretUI) {
		wp->renderer->r_updateCaretUI(wp, &wp->cx, &wp->cy, &wp->cwidth, &wp->cheight);
	} else {
		render_updateCaretUIPosition(wp);
	}

	if (type != op->type || wp->cheight != op->cheight) {
		caret_hide(wp->ww_handle);
		op->type = 0;
	}

	if (type) {
		caret_updatePositionAndVisibility(wp, type, op->type);
	}

	op->type = type;
	op->cheight = wp->cheight;
}

/*------------------------------------------------------------
 * render_calculateScrollDelta()
 */
int render_calculateScrollDelta(long val, long minval, long maxval, int scrollBy)
{	long delta;

	if (val > maxval) {
		delta = (val - maxval)+scrollBy;
	}
	else if (val < minval) {
		delta = (val - minval)-scrollBy;
	}
	else delta = 0;
	return delta;
}

/*------------------------------------------------------------
 * render_makeCaretVisible()
 * scroll the window, so the given caret is visible.
 */
int render_makeCaretVisible(WINFO* wp, CARET* pCaret) {
	long dx, dy;

	long minln = wp->mincursln;
	long maxln = wp->maxcursln;
	if (maxln == minln) {
		// during startup - cannot adjust scroll bounds with 0 size.
		return 0;
	}
	dy = render_calculateScrollDelta(pCaret->ln, minln, maxln, wp->vscroll);
	dx = render_calculateScrollDelta(pCaret->col, wp->mincurscol, wp->maxcurscol, wp->hscroll);

	if (dx || dy) {
		EdTRACE(log_message(DEBUG_TRACE, "render_makeCaretVisible(scrollDeltaX=%ld,scrollDeltaY=%ld)", dx, dy));
		if (sl_scrollwinrange(wp, &dy, &dx))
			sl_winchanged(wp, dy, dx);
		return 1;
	}
	return 0;
}


/*------------------------------------------------------------
 * render_adjustScrollBounds()
 */
int render_adjustScrollBounds(WINFO *wp) {
	return render_makeCaretVisible(wp, &wp->caret);
}

/*
 * Called initially once after the main frame has been shown to adjust the scroll bounds
 * of all windows.
 */
void ww_adjustWindowSizes() {
	for (WINFO* wp = ww_getCurrentEditorWindow(); wp; ) {
		RENDERER_SCROLL_SET_BOUNDS pFunc = wp->renderer->r_adjustScrollBounds;
		if (pFunc) {
			pFunc(wp);
		}
		wp = wp->next;
	}
}

/*------------------------------------------------------------
 * wt_curpos()
 */
void wt_curpos(WINFO *wp, long ln, long col)
{
	caret_moveToLine(wp, ln);
	wp->caret.col = col;
	RENDERER_SCROLL_SET_BOUNDS pFunc = wp->renderer->r_adjustScrollBounds;
	if (pFunc) {
		pFunc(wp);
	}
	render_updateCaret(wp);
	if (wp->bXtndBlock) {
		caret_extendSelection(wp);
	}

}

/*------------------------------------------------------------
 * wt_setCaretVisibility()
 * Show hide the current main input caret.
 */
void wt_setCaretVisibility(WINFO *wp,int type)
{
	if (type != wp->ctype) {
		wp->ctype = type;
		render_updateCaret(wp);
	}
}

/*------------------------------------------------------------
 * wt_scrollxy()
 * Scroll the current window by a number of lines and columns.
 */
void wt_scrollxy(WINFO *wp,int nlines, int ncolumns) {
	 int yDelta = -nlines * wp->cheight;
	 ScrollWindowEx(wp->ww_handle,-ncolumns*wp->cwidth,yDelta,
			   (LPRECT)0,(LPRECT)0,(HRGN)0,(LPRECT)0, SW_INVALIDATE);
	 UpdateWindow(wp->ww_handle);

	if (ncolumns == 0 && wp->lineNumbers_handle) {
		ScrollWindow(wp->lineNumbers_handle, 0, yDelta,
			(LPRECT)0, (LPRECT)0);
	}
}

/*------------------------------------------------------------
 * wt_scrollpart()
 */
void wt_scrollpart(WINFO *wp, int nCaretLine, int from_top, int nlines) {
	RECT r;
	int  dh;

	GetClientRect(wp->ww_handle, &r);
	r.top  = (int)(nCaretLine-wp->minln+from_top)*wp->cheight;
	dh = (nlines)*wp->cheight;

	if (r.top+dh <= r.bottom-2*wp->cheight) {
		ScrollWindow(wp->ww_handle,0,dh,&r,(LPRECT)0);
     } else {
		r.top = (int)(nCaretLine-wp->minln)*wp->cheight;
		InvalidateRect(wp->ww_handle, &r, 0);
	}
	if (wp->lineNumbers_handle) {
		InvalidateRect(wp->lineNumbers_handle, NULL,0);
	}
}

/*------------------------------------------------------------
 * wt_insline()
 */
struct tagLINES_SCROLLED {
	long caretLine;
	long lnfirst;
	long nlines;
};
static int wt_linesScrolled(WINFO* wp, struct tagLINES_SCROLLED* pParam) {
	wt_scrollpart(wp, pParam->caretLine, pParam->lnfirst, pParam->nlines);
	return 1;
}
void wt_insline(FTABLE *fp, int caretLine, int nlines) {
	ft_forAllViews(fp, wt_linesScrolled, &(struct tagLINES_SCROLLED){caretLine, 0, nlines});
}

/*------------------------------------------------------------
 * wt_deleteline()
 */
void wt_deleteline(FTABLE *fp, int caretLine, int additional, int nlines) {
	ft_forAllViews(fp, wt_linesScrolled, &(struct tagLINES_SCROLLED){caretLine, additional + nlines, -nlines});
}
