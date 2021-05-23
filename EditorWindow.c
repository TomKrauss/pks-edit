/*
 * EditorWindow.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: terminal emulation stuff
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */


#include <windows.h>
# pragma hdrstop

#include "lineoperations.h"
#include "winfo.h"
#include "trace.h"

#define MAXCOL		FBUFSIZE/2

extern void 	XtndBlock(FTABLE *fp);
extern void 	st_redraw(BOOL bErase);
extern int 	_chkblk(FTABLE *fp);

int 	cursor_width = 1;

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
static int EdUpdateCaret(WINFO *wp, int type, int visible)
{
     if (!visible) {
		if (type == 1) {
			CreateCaret(wp->ww_handle,NULL,wp->cwidth*cursor_width,wp->cheight);
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
 * UpdateOwnCaret()
 */
void UpdateOwnCaret(WINFO *wp, HDC hdc)
{
	if (!wp->owncursor)
		return;
	DrawOutline(hdc,wp->cx,wp->cy,wp->cwidth*cursor_width,wp->cheight);
}

/*------------------------------------------------------------
 * upatecursor
 */
static struct olc {
	int type,width;
} _olcurs = { 0, 1 };
void updatecursor(WINFO *wp) {
	struct olc *op = &_olcurs;
	int type;

	type = wp->ctype;
	if (type) {
		type = (cursor_width > 1 || _chkblk(FTPOI(wp)) == 0) ?
			1 : 2;
	}

	if (type != op->type || cursor_width != op->width) {
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
	op->width = cursor_width;
	cursor_width = 1;
}

/*------------------------------------------------------------
 * wt_cursrange()
 */
int wt_calculateScrollDelta(register long val,  register long minval, 
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
 * wt_adjustScrollBounds()
 */
static int wt_adjustScrollBounds(WINFO *wp) {
	long dx,dy;

	dy = wt_calculateScrollDelta(wp->ln,wp->mincursln,wp->maxcursln,wp->vscroll);
	dx = wt_calculateScrollDelta(wp->col,wp->mincurscol,wp->maxcurscol,wp->hscroll);

	if (dx || dy) {
		EdTRACE(Debug(DEBUG_WINMESS,"wt_adjustScrollBounds -> (%ld,%ld)",dx,dy));
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
	wp->ln = ln;
	wp->col = col;
	wt_adjustScrollBounds(wp);
	updatecursor(wp);
	if (wp->bXtndBlock) {
		XtndBlock(FTPOI(wp));
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
		updatecursor(wp);
	}
}

/*------------------------------------------------------------
 * wt_scrollxy()
 */
void wt_scrollxy(WINFO *wp,int nlines, int ncolumns)
{
     EdTRACE(Debug(DEBUG_TRACE,"wt_scrollxy (%lx,%d,%d)",wp,nlines,ncolumns));
     
	ScrollWindow(wp->ww_handle,-ncolumns*wp->cwidth,-nlines*wp->cheight,
			   (LPRECT)0,(LPRECT)0);

	UpdateWindow(wp->ww_handle);
}

	
/*------------------------------------------------------------
 * wt_scrollpart()
 */
void wt_scrollpart(WINFO *wp,int from_top, int nlines)
{
	RECT r;
	int  dh;

	r.left = 0; r.right = wp->workarea.g_w;
	r.top  = (int)(wp->ln-wp->minln+from_top)*wp->cheight;
	r.bottom = wp->workarea.g_h;
	dh = (nlines)*wp->cheight;

	if (r.top+dh <= r.bottom-2*wp->cheight) {
		ScrollWindow(wp->ww_handle,0,dh,&r,(LPRECT)0);
     } else {
		r.top = (int)(wp->ln-wp->minln)*wp->cheight;
		InvalidateRect(wp->ww_handle,&r,1);
	}

     UpdateWindow(wp->ww_handle);

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
