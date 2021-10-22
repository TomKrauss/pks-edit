/*
 * EditorWindow.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
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
 * EdHideCaret()
 */
static int EdHideCaret(HWND hwnd)
{
     HideCaret(hwnd);
     DestroyCaret();
     return 1;
}

/*------------------------------------------------------------
 * EdUpdateCaret()
 */
static int EdUpdateCaret(WINFO *wp, CURSOR_TYPE type, int visible) {
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

/*------------------------------------------------------------
 * render_updateCaret()
 * macro_updateSelectedMacro the current caret for the passed editor window (dependening on insert mode
 * etc...)
 */
static struct olc {
	CURSOR_TYPE type;
} _olcurs = { CUR_HIDDEN };
void render_updateCaret(WINFO *wp) {
	struct olc *op = &_olcurs;
	CURSOR_TYPE type;

	type = wp->ctype;
	if (type) {
		type = (wp->workmode & WM_INSERT) ?
			CUR_INSERT : CUR_OVERRIDE;

	}

	if (type != op->type) {
		if (op->type) {
			EdHideCaret(wp->ww_handle);
			op->type = 0;
		}
	}

	wp->cx = (int) (wp->caret.col-wp->mincol) * wp->cwidth;
	wp->cy = (int) (wp->caret.ln -wp->minln)  * wp->cheight;

	if (type) {
		st_redraw(FALSE);
		EdUpdateCaret(wp, type, op->type);
	}

	op->type = type;
}

/*------------------------------------------------------------
 * wt_cursrange()
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
 * render_adjustScrollBounds()
 */
static int render_adjustScrollBounds(WINFO *wp) {
	long dx,dy;

	dy = render_calculateScrollDelta(wp->caret.ln,wp->mincursln,wp->maxcursln,wp->vscroll);
	dx = render_calculateScrollDelta(wp->caret.col,wp->mincurscol,wp->maxcurscol,wp->hscroll);

	if (dx || dy) {
		EdTRACE(log_errorArgs(DEBUG_WINMESS,"render_adjustScrollBounds -> (%ld,%ld)",dx,dy));
		if (sl_scrollwinrange(wp,&dy,&dx))
			sl_winchanged(wp,dy,dx);
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------
 * wt_curpos()
 */
void wt_curpos(WINFO *wp, long ln, long col)
{
	int oldln = wp->caret.ln;
	caret_moveToLine(wp, ln);
	wp->caret.col = col;
	render_adjustScrollBounds(wp);
	render_updateCaret(wp);
	if (wp->bXtndBlock) {
		caret_extendSelection(wp);
	}

}

/*------------------------------------------------------------
 * wt_tcursor()
 * change textcursor form
 */
void wt_tcursor(WINFO *wp,int type)
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
