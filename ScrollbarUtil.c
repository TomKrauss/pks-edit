/*
 * ScrollbarUtil.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: handle slider update and messages
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
#include "caretmovement.h"
#include "trace.h"
#include "winfo.h"
#include "edierror.h"

#define MAXCOL		2*MAXLINELEN

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
	}
}
#endif

/*------------------------------------------------------------
 * sl_size()
 */
int sl_size(WINFO *wp)
{
	long  	n,top;
	int   	oldrange,oldstart;
	FTABLE	*fp;

	if (!wp->ww_handle)
		return 0;

	fp = FTPOI(wp);
	if (wp->dispmode & SHOWHIDEVSLIDER) {
		n = top = 0;
	} else {
		for (n = fp->nlines+1L, top = wp->minln; 
		    n > 32000L; 
		    n >>= 1, top >>= 1)
		;
	}

#if defined(WIN32)
	SetWin32ScrollInfo(wp, SB_VERT, top, wp->maxln - wp->minln + 1, n);
#else
	GetScrollRange(wp->ww_handle, SB_VERT, &oldstart, &oldrange);
	oldstart = GetScrollPos(wp->ww_handle, SB_VERT);
	SetScrollRange(wp->ww_handle, SB_VERT, 0, (int)n, FALSE);
	SetScrollPos(wp->ww_handle, SB_VERT, (int)top, 
			  (oldstart != top || oldrange != n));
#endif
	if (wp->dispmode & SHOWHIDEHSLIDER) {
		n = top = 0;
	} else {
		top = MAXCOL;
		n = wp->mincol;
		if (n > top)
			n = top;
	}

#if 0
/* PKS-EDIT does not know about the longest line in a file currently */
	SetWin32ScrollInfo(wp, SB_HORZ, n, wp->maxcol - wp->mincol + 1, top);
#else
	GetScrollRange(wp->ww_handle, SB_HORZ, &oldstart, &oldrange);
	oldstart = GetScrollPos(wp->ww_handle, SB_HORZ);
	SetScrollRange(wp->ww_handle,SB_HORZ, 0, top, FALSE);
	SetScrollPos(wp->ww_handle,SB_HORZ, (int)n, 
			  (oldstart != n || oldrange != top));
#endif
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

	EdTRACE(Debug(DEBUG_FUNCS,"sl_calcnewmin(%ld,%ld)",da,max));
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
int sl_scrollwinrange(WINFO *wp,long *DY, long *DX)
{ 	long   dy,dx,val;
	FTABLE *fp = FTPOI(wp);

	dy = *DY, dx = *DX;
	EdTRACE(Debug(DEBUG_FUNCS,"sl_scrollwinrange(%ld,%ld) fp = %lx",
				 dy,dx,(long)fp));
	if (dx) {
		val = sl_calcnewmin(dx,MAXCOL,wp->mincol);
		*DX = val-wp->mincol;
		wp->mincol = val;
	} 

	if (dy) {
		val = sl_calcnewmin(dy,fp->nlines,wp->minln);
		*DY = val-wp->minln;
		wp->minln = val;
	} 

	if (*DY || *DX) {
		SetScrollCheckBounds(wp);
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
		SendRedraw(wp->ww_handle);
	}
	if (dx) {
		SendRedraw(wp->ru_handle);
	}
	sl_size(wp);
}

/*------------------------------------------------------------
 * sl_moved
 */
int sl_moved(WINFO *wp, long dy, long dx, int cursor_adjust) 
{
	long col = wp->col;
	long	ln  = wp->ln;

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
		if (col != wp->col || ln != wp->ln) {
			if (!cphyspos(FTPOI(wp),&ln,&col,0)) {
				ln = wp->ln;
			}
			wp->ln  = ln;
			wp->col = col;
			updatecursor(wp);
		}
	} else {
		updatecursor(wp);
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
	WINFO *	wp = WIPOI(_currfile);
	
	switch(mtype) {
		case MOT_SINGLE:
			delta = _multiplier; break;
		case -MOT_SINGLE:
			delta = -_multiplier; break;
		case MOT_CENTER:
			delta = wp->ln-(wp->maxln+wp->minln) / 2;
			break;
	}
	sl_moved(wp, delta, 0, (wp->scrollflags & SC_CURSORCATCH));
}

/*--------------------------------------------------------------------------
 * EdScrollScreen()
 * Scroll - cursor sticks to screen
 */
int EdScrollScreen(int mtype)
{	long		dln,ln,col;
	int		ret;
	WINFO	*wp;
	FTABLE	*fp = _currfile;

	if (!fp)
		return 0;

	wp = WIPOI(fp);

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

	if ((ln = fp->ln+dln) < 0 ||
		ln >= fp->nlines)
		return 0;

	col = fp->lnoffset;
	sl_moved(wp,dln,0,0);
	ret = cposvalid(fp,&ln,&col,0);
	wt_curpos(wp,ln,col);
	return ret;
}

