/*
 * EditorWindow.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: terminal emulation stuff
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */


#include "customcontrols.h"
#include "lineoperations.h"
#include "winfo.h"
#include "trace.h"
#include "edierror.h"

#define MAXCOL		FBUFSIZE/2

extern void 	caret_extendSelection(FTABLE *fp);
extern void 	st_redraw(BOOL bErase);
extern int 	ft_checkSelection(FTABLE *fp);

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
static int EdUpdateCaret(WINFO *wp, CURSOR_TYPE type, int visible)
{
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
 * render_updateCustomCaret()
 * When a custom caret is being displayed - re-render it now.
 */
void render_updateCustomCaret(WINFO *wp, HDC hdc)
{
	if (!wp->owncursor)
		return;
	cust_drawOutline(hdc,wp->cx,wp->cy,wp->cwidth,wp->cheight);
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

	wp->cx = (int) (wp->col-wp->mincol) * wp->cwidth;
	wp->cy = (int) (wp->ln -wp->minln)  * wp->cheight;

	if (type) {
		st_redraw(FALSE);
		EdUpdateCaret(wp, type, op->type);
	}

	op->type = type;
}

/*------------------------------------------------------------
 * wt_cursrange()
 */
int render_calculateScrollDelta(register long val,  register long minval, 
			    register long maxval, int d)
{	long delta;

	if (val > maxval) {
		delta = (val - maxval)+d;
	}
	else if (val < minval) {
		delta = (val - minval)-d;
	}
	else delta = 0;
	return delta;
}

/*------------------------------------------------------------
 * render_adjustScrollBounds()
 */
static int render_adjustScrollBounds(WINFO *wp) {
	long dx,dy;

	dy = render_calculateScrollDelta(wp->ln,wp->mincursln,wp->maxcursln,wp->vscroll);
	dx = render_calculateScrollDelta(wp->col,wp->mincurscol,wp->maxcurscol,wp->hscroll);

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
	int oldln = wp->ln;
	wp->ln = ln;
	wp->col = col;
	render_adjustScrollBounds(wp);
	render_updateCaret(wp);
	if (wp->bXtndBlock) {
		caret_extendSelection(FTPOI(wp));
	}

	if (oldln != wp->ln) {
		int ln1;
		int ln2;
		if (oldln < wp->ln) {
			ln1 = oldln;
			ln2 = wp -> ln;
		} else {
			ln1 = wp->ln;
			ln2 = oldln;
		}
		render_repaintFromLineTo(wp, ln1, ln2);
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
void wt_scrollxy(WINFO *wp,int nlines, int ncolumns)
{
     EdTRACE(log_errorArgs(DEBUG_TRACE,"wt_scrollxy (%lx,%d,%d)",wp,nlines,ncolumns));
	 int yDelta = -nlines * wp->cheight;
	ScrollWindow(wp->ww_handle,-ncolumns*wp->cwidth,yDelta,
			   (LPRECT)0,(LPRECT)0);
	UpdateWindow(wp->ww_handle);

	if (ncolumns == 0 && wp->lineNumbers_handle) {
		ScrollWindow(wp->lineNumbers_handle, 0, yDelta,
			(LPRECT)0, (LPRECT)0);
	}
}

/*------------------------------------------------------------
 * wt_scrollpart()
 */
void wt_scrollpart(WINFO *wp,int from_top, int nlines)
{
	RECT r;
	int  dh;

	GetClientRect(wp->ww_handle, &r);
	r.top  = (int)(wp->ln-wp->minln+from_top)*wp->cheight;
	dh = (nlines)*wp->cheight;

	if (r.top+dh <= r.bottom-2*wp->cheight) {
		ScrollWindow(wp->ww_handle,0,dh,&r,(LPRECT)0);
     } else {
		r.top = (int)(wp->ln-wp->minln)*wp->cheight;
		InvalidateRect(wp->ww_handle, &r, 0);
	}
	if (wp->lineNumbers_handle) {
		InvalidateRect(wp->lineNumbers_handle, NULL,0);
	}
}

/*------------------------------------------------------------
 * wt_insline()
 */
void wt_insline(WINFO *wp,int nlines)
{
	wt_scrollpart(wp,0,nlines);
}

/*------------------------------------------------------------
 * wt_deleteline()
 */
void wt_deleteline(WINFO *wp,int additional, int nlines)
{
	wt_scrollpart(wp, additional+nlines,-nlines);
}
