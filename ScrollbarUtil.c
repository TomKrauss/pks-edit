/*
 * ScrollbarUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS
 *
 * purpose: handle slider update and messages
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
#include "caretmovement.h"
#include "trace.h"
#include "winfo.h"
#include "edierror.h"
#include "winutil.h"

#define MAXCOL		2*MAXLINELEN

// Maximum number of lines in a file for which the max line length is correctly calculated to update
// horizontal scrollbars.
#define MAXLINES_TO_CALCULATE_COLUMNS 1024

/*! MODIFY HERE !*/
long		_multiplier = 1;

#if defined(WIN32)
static void SetWin32ScrollInfo(WINFO * wp, int nSlider, 
		int lValue, int nVisible, int lMaximum)
{	SCROLLINFO	scrollInfo;

	scrollInfo.cbSize = sizeof scrollInfo;
	GetScrollInfo(wp->ww_handle, nSlider, &scrollInfo);
	if (lMaximum != scrollInfo.nMax || 
			nVisible != (INT)scrollInfo.nPage ||
			lValue != scrollInfo.nPos) {
		scrollInfo.fMask = SIF_ALL|SIF_DISABLENOSCROLL;
		scrollInfo.nMin = 0;
		scrollInfo.nMax = lMaximum;
		scrollInfo.nPage = nVisible;
		scrollInfo.nPos = lValue;
		SetScrollInfo(wp->ww_handle, nSlider, &scrollInfo, TRUE);
		ShowScrollBar(wp->ww_handle, nSlider, nVisible < lMaximum);
	}
}
#endif

/**
 * Calculate the length of the longest line on the fly. 
 * Dumb version, which does not yet handle tabs / formatting etc.
 * Should cache the result and be notified by the model, when the model
 * changes to remove the cache.
 */
static int calculateLongestLine(WINFO *wp) {
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

/*------------------------------------------------------------
 * sl_size()
 * Update the internal sizeds of the WINFO structure (maxrows etc...).
 * Also update slider thumbs for the window.
 */
int sl_size(WINFO *wp) {
	long  	n;
	long	screenTop;
	FTABLE	*fp;

	if (!wp->ww_handle) {
		return 0;
	}

	fp = FTPOI(wp);
	if (wp->dispmode & SHOWHIDEVSLIDER) {
		n = screenTop = 0;
	} else {
		screenTop = wp->minln;
		n = fp->nlines+1L;
	}
	SetWin32ScrollInfo(wp, SB_VERT, screenTop, wp->maxln - wp->minln + 1, n);

	if (wp->dispmode & SHOWHIDEHSLIDER) {
		n = 0;
	} else {
		if (wp->maxVisibleLineLen < 0) {
			wp->maxVisibleLineLen = calculateLongestLine(wp);
		}
		n = wp->mincol;
		if (n > wp->maxVisibleLineLen) {
			n = wp->maxVisibleLineLen;
		}
	}
	int visibleColumns = wp->maxcol - wp->mincol + 1;
	SetWin32ScrollInfo(wp, SB_HORZ, n, visibleColumns, (wp->dispmode & SHOWHIDEHSLIDER) ? 0 : wp->maxVisibleLineLen);
	return 1;
}

/*------------------------------------------------------------
 * sl_thumb2deltapos()
 */
long sl_thumb2deltapos(WINFO *wp, int horizontal, WORD thumb)
{	int bar = (horizontal) ? SB_HORZ : SB_VERT;
	int  bar_range,bar_pos,dummy;
	long max;
	
	GetScrollRange(wp->ww_handle, bar, &dummy, &bar_range);
	bar_pos = GetScrollPos(wp->ww_handle, bar);

	max = (horizontal) ? MAXCOL : (FTPOI(wp))->nlines;
	while(max > 32000) {
		max >>= 1;
		bar_pos >>= 1;
	}
	return MulDiv((int)max,thumb,bar_range) - bar_pos;
}

/*------------------------------------------------------------
 * sl_calcnewmin()
 * check valid range for ln or col
 */
static long sl_calcnewmin(long da,long max,long val)
{	register long b;

	EdTRACE(log_errorArgs(DEBUG_FUNCS,"sl_calcnewmin(%ld,%ld)",da,max));
	max--;
	if (da < 0L) {
		if ((b = val + da) < 0L) {
			val = 0L;
		}
		else {
			val = b;
		}
	}
	else {
		if ((b = val + da) >= max) {
			val = max;
		}
		else {
			val = b;
		}
	}
	return val;
}

/*-----------------------------------------------------------
 * sl_scrollwinrange()
 */
int sl_scrollwinrange(WINFO *wp, long *pDeltaY, long *pDeltaX)
{ 	long   dy,dx,val;
	FTABLE *fp = FTPOI(wp);

	dy = *pDeltaY, dx = *pDeltaX;
	EdTRACE(log_errorArgs(DEBUG_FUNCS,"sl_scrollwinrange(%ld,%ld) fp = %lx",
				 dy,dx,(long)(intptr_t)fp));
	if (dx) {
		val = sl_calcnewmin(dx,MAXCOL,wp->mincol);
		*pDeltaX = val-wp->mincol;
		wp->mincol = val;
	} 

	if (dy) {
		val = sl_calcnewmin(dy,fp->nlines,wp->minln);
		*pDeltaY = val-wp->minln;
		wp->minln = val;
	} 

	if (*pDeltaY || *pDeltaX) {
		ww_setScrollCheckBounds(wp);
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------
 * sl_winchanged()
 */
void sl_winchanged(WINFO *wp,long dy, long dx) { 	
	int  my = ((int)(wp->maxln - wp->minln) >> 1);
	int  mx = ((int)(wp->maxcol - wp->mincol) >> 1);

	if ((dy < my && dy > -my) && 
	    (dx < mx && dx > -mx)) {
		wt_scrollxy(wp,(int)dy,(int)dx);
	} else {
		if (wp->lineNumbers_handle && dy != 0) {
			win_sendRedrawToWindow(wp->lineNumbers_handle);
		}
		win_sendRedrawToWindow(wp->ww_handle);
	}
	if (dx && wp->ru_handle) {
		win_sendRedrawToWindow(wp->ru_handle);
	}
	sl_size(wp);
}

/*------------------------------------------------------------
 * sl_moved
 * Invoked, when a scrollbar had been dragged - update the WINFO 
 * data structure.
 */
int sl_moved(WINFO *wp, long dy, long dx, int cursor_adjust) 
{
	long col = wp->caret.col;
	long	ln  = wp->caret.ln;

	if (!sl_scrollwinrange(wp,&dy,&dx))
		return 0;
	sl_winchanged(wp,dy,dx);

	if (cursor_adjust) {
		if (ln < wp->mincursln) {
			ln = wp->mincursln;
		} else if (ln > wp->maxcursln) {
			ln = wp->maxcursln;
		}

		if (col < wp->mincurscol) {
			col = wp->mincurscol;
		} else if (col > wp->maxcurscol) {
			col = wp->maxcurscol;
		}
		if (col != wp->caret.col || ln != wp->caret.ln) {
			if (!caret_updateLineColumn(wp, &ln, &col, 0)) {
				ln = wp->caret.ln;
			}
			wp->caret.ln  = ln;
			wp->caret.col = col;
			render_updateCaret(wp);
		}
	} else {
		render_updateCaret(wp);
	}

	return 1;
}

/*------------------------------------------------------------
 * EdScrollCursor()
 * Scroll - cursor sticks to text
 */
void EdScrollCursor(int mtype)
{
	long 	delta = 0;
	extern long _multiplier;
	WINFO *	wp = ww_getCurrentEditorWindow();
	
	if (wp == NULL) {
		return;
	}
	switch(mtype) {
		case MOT_SINGLE:
			delta = _multiplier; break;
		case -MOT_SINGLE:
			delta = -_multiplier; break;
		case MOT_CENTER:
			delta = wp->caret.ln-(wp->maxln+wp->minln) / 2;
			break;
	}
	sl_moved(wp, delta, 0, (wp->scrollflags & SC_CURSORCATCH));
}

/*--------------------------------------------------------------------------
 * EdScrollScreen()
 * Scroll - cursor sticks to screen
 */
int EdScrollScreen(int mtype)
{	long	dln,ln,col;
	int		ret;
	WINFO	*wp = ww_getCurrentEditorWindow();

	if (!wp)
		return 0;
	FTABLE* fp = wp->fp;

	switch(mtype) {
		case MOT_SINGLE:
			dln = 1;
			break;
		case -MOT_SINGLE:
			dln = -1;
			break;
		case MOT_PAGE:
			dln = wp->maxln-wp->minln;
			break;
		case -MOT_PAGE:
			dln = wp->minln-wp->maxln;
			break;
		default:
			return 0;
	}

	dln *= _multiplier;

	if ((ln = wp->caret.ln+dln) < 0 ||
		ln >= fp->nlines)
		return 0;

	col = wp->caret.offset;
	sl_moved(wp,dln,0,0);
	ret = wp->renderer->r_placeCaret(wp,&ln,&col,0);
	wt_curpos(wp,ln,col);
	return ret;
}

