/*
 * PaintEditorWindow.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: paint edit (work-)windos
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"

#pragma hdrstop

extern char *writeattrline();

static DWORD _ROPcodes[] = {
	BLACKNESS,          /* D = 0 */
	0x00500A9UL,        /* ~(P|D) */
	0x00A0329UL,        /* ~P & D */
	0x00F0001UL,        /* ~P */
	0x00500325UL,       /* P&~D */
	DSTINVERT,          /* ~D */
	PATINVERT,          /* P^D */
	0x005F00E9UL,       /* ~(P&D) */
	0x00A000C9UL,		/* P&D */
	0x00A50065UL, 		/* ~(P^D) */
	0x00AA0029UL, 		/* D */
	0x00AF0029UL, 		/* ~P|D */
	PATCOPY, 			/* P */
	0x00F50225UL,       /* P|~D */
	0x00FA0089UL,       /* P|D */
	WHITENESS			/* D = 1 */
};

/*--------------------------------------------------------------------------
 * markline()
 * marked one line
 */
static void markline(HDC hdc, WINFO *wp,LINE *lp,int y,int lastcol)
{
	RECT 	r;
	FTABLE	*fp = FTPOI(wp);

	r.top = y;
	r.bottom = y+wp->cheight;
	r.left = 0;
	if (lp->lflg & LNXMARKED) {
		r.right = wp->workarea.g_w;
/*
		SelectObject(hdc,GetStockObject(wp->markstyles[FS_XMARKED].style));
		PatBlt(hdc,r.left,r.top,
				 r.right-r.left,r.bottom-r.top,
				 _ROPcodes[wp->markstyles[FS_XMARKED].mode]);
*/
		SelectObject(hdc,GetStockObject(GRAY_BRUSH));
		PatBlt(hdc,r.left,r.top,
			r.right-r.left,r.bottom-r.top,
			PATINVERT);	 
	}

	if (lp->lflg & LNCPMARKED) {
		if (ww_blkcolomn(wp) != 0) {
			r.left = fp->blcol1;
			r.right = fp->blcol2;
		} else {
			if (!fp->blstart || !fp->blend) {
				alert("bad marked line");
				return;
			}
			if (P_EQ(lp,fp->blstart->lm))
				r.left = _cphys2scr(fp,lp->lbuf,fp->blstart->lc);
			else r.left = wp->mincol;
	
			if (P_EQ(lp,fp->blend->lm))
				r.right = _cphys2scr(fp,lp->lbuf,fp->blend->lc);
			else r.right = lastcol;
		}
		r.left -= wp->mincol; if (r.left < 0) r.left = 0;
	
		if (r.right > wp->maxcol+1)
			r.right = wp->maxcol+1;
		r.right -= wp->mincol;
	
		r.left = r.left * wp->cwidth; 
		r.right = r.right * wp->cwidth;
		if (r.right > r.left) {
			SelectObject(hdc,GetStockObject(wp->markstyles[FS_BMARKED].style));
			PatBlt(hdc,r.left,r.top,
					 r.right-r.left,r.bottom-r.top,
					 _ROPcodes[wp->markstyles[FS_BMARKED].mode]);
		}
	}
}

/*--------------------------------------------------------------------------
 * write_line()
 */
int write_line(HDC hdc, int x, int y, WINFO *wp, LINE *lp)
{	register int 			start,i,end,ind,textlen;
	register unsigned char 	*d,*s,*send;
	LINEAL 				*linp = ((FTABLE*)FTPOI(wp))->lin;
	/* limited linelength ! */
	char 				buf[512];
	int					flags;

	start = wp->mincol;
	end = wp->maxcol+1;
	flags = wp->dispmode;

	if (flags & SHOWATTR) {
		d = writeattrline(hdc,x,y,buf,lp,start,end,wp);
		goto _domark;
	}

	textlen = end-start;
	if (textlen >= sizeof buf) {
		textlen = sizeof buf-1;
		end = textlen+start;
	}
#if 1
	blfill(buf,textlen,' ');
# endif

	d = buf;

	if (PLAINCONTROL(flags)) {
		if (lp->len < start) {
			goto _output;
		}
	
		s = &lp->lbuf[start];
		if (end > lp->len)
			send = &lp->lbuf[lp->len];
		else send = &lp->lbuf[end];
		while (s < send) {
			if ((*d++ = *s++) == 0) d[-1] = 'O';
		}

termline:
		if (end > lp->len && !(lp->lflg & LNNOTERM)) {
			*d++ = (lp->lflg & LNNOCR) ? '¬' : '¶';
		}

		goto _output;
	} else {
		i 	= 0;
		s 	= lp->lbuf;
		send	= &lp->lbuf[lp->len];
		while (i < start) {
			if (s >= send) 
				goto _output;

			if (*s++ == '\t') {
				if ((i = TabStop(i,linp)) > start) {
					if (i > end)
						i = end;
					d  += (i-start);
					break;
				}
			} else i++;
		}
		while (i < end && s < send) {
			if (*s >= ' ') {
				*d++ = *s++;
				i++;
			} else {
				if (*s != '\t') {
					*d++ = ' ';
					s++; i++;
					continue;
				}
				ind = TabStop(i,linp);
				if (ind > end) 
					ind = end;
				d = blfill(d, ind-i, linp->t1);
				if (flags & SHOWCONTROL) {
					d[-1] = '»';
				}
				i = ind;
				s++;
			}
		}
		if (flags & SHOWCONTROL) {
			goto termline;
		}
	}
_output:
/* optimize leading blanks */
	for (s = buf; s < d; ) {
		if (*s != ' ') {
			break;
		}
		s++;
		x += wp->cwidth;
	}
	if ((textlen = d-s) > 0) {
		TextOut(hdc,x,y,s,textlen);
	}
_domark:
	return start+d-buf;
}

/*------------------------------------------------------------
 * calcy()
 */
static int calcy(WINFO *wp,long ln)
{	int y = ln-wp->minln;

	return y * wp->cheight;
}

/*--------------------------------------------------------------------------
 * redraw_indirect()
 */
static void redraw_indirect(HDC hdc, WINFO *wp, int y, LINE *lp)
{
	int		x;
	int		nWidth;
	DWORD	dwExtent;
	char 	buf[100];

	wsprintf(buf," skipped %ld... ", (long)LpIndNTotal(lp));

	TextOut(hdc, wp->cwidth, y, buf, lstrlen(buf));
	dwExtent = GetTextExtent(hdc, buf, lstrlen(buf));
	nWidth = LOWORD(dwExtent) + 4;
	y = y + wp->cheight/2-1;
	x = wp->workarea.g_w;
	MoveTo(hdc, nWidth, y);
	LineTo(hdc, x, y);

	y += 2;
	MoveTo(hdc, nWidth, y);
	LineTo(hdc, x, y);
}

/*--------------------------------------------------------------------------
 * P_redraw()
 */
static void P_redraw(WINFO *wp,long min,long max,int flg)
{
	HBRUSH		hBrush;
	HDC 			hdc;
	HWND			hwnd;
	HFONT 		saveFont;
	RECT 		r;
	PAINTSTRUCT 	ps;
	int  		y,newy,visLen;
	long			ln;
	FTABLE *		fp = FTPOI(wp);
	LINE *		lp;

	hwnd = wp->ww_handle;

	hdc = BeginPaint(hwnd, &ps);
	saveFont = EdSelectFont(wp,hdc);
	SetTextColor(hdc,wp->fnt_fgcolor);
	/* SetBkColor(hdc,wp->fnt_bgcolor); */
	SetBkMode(hdc, TRANSPARENT);
	hBrush = CreateSolidBrush(wp->fnt_bgcolor);

	y = calcy(wp,min);
	lp = ln_relgo(fp,min-wp->ln);
	for (ln = min; lp && ln <= max && y < ps.rcPaint.bottom;
		lp = lp->next, ln++,  y = newy) {
		newy = y + wp->cheight;
		if (newy > ps.rcPaint.top && 			/* if in redraw area */
		    (flg || (lp->lflg & LNREPLACED))) {	/* if Line is modified || we redraw all */
	
			r.left = 0; r.right = wp->workarea.g_w;
			r.top = y;
			r.bottom = min(ps.rcPaint.bottom,y+wp->cheight);
			FillRect(hdc,&r,hBrush);
	
			if (lp->lflg & LNINDIRECT) {
				redraw_indirect(hdc, wp, y, lp);
				visLen = 1;
			} else {
				visLen = write_line(hdc,0,y,wp,lp);
			}
			if (lp ->lflg & (LNCPMARKED|LNXMARKED)) 
				markline(hdc,wp,lp,y,visLen);
			if (ln == wp->ln)
				UpdateOwnCaret(wp,hdc);
		}
	}

	if (!lp) {
		r.left = 0; r.right = wp->workarea.g_w;
		r.top = y;
		r.bottom = min(ps.rcPaint.bottom,y+(max-ln)*wp->cheight);
		FillRect(hdc,&r,hBrush);
	}

	DeleteObject(hBrush);
	EdUnselectFont(hdc);
	EndPaint(hwnd,&ps);

}

/*--------------------------------------------------------------------------
 * SendRedraw()
 */
EXPORT void SendRedraw(HWND hwnd)
{
	if (hwnd > 0) {
		UpdateWindow(hwnd);
		InvalidateRect(hwnd,(LPRECT)0,1);
	}
}

/*------------------------------------------------------------
 * RedrawWmPaint()
 */
EXPORT void RedrawWmPaint(WINFO *wp)
{
	P_redraw(wp,wp->minln,wp->maxln,1);
}

/*--------------------------------------------------------------------------
 * EdRedrawWindow()
 */
EXPORT void EdRedrawWindow(WINFO *wp)
{
	if (wp) {
		SendRedraw(wp->ww_handle);
	}
}

/*--------------------------------------------------------------------------
 * p_redraw()
 */
EXPORT void p_redraw(void)
{	FTABLE *fp;
	WINFO  *wp;

	if ((fp = _currfile) != 0L) {
		wp = WIPOI(fp);
		SendRedraw(wp->ww_handle);
		P_redraw(wp,wp->minln,wp->maxln,0);
	}
}

/*--------------------------------------------------------------------------
 * RedrawFromTo()
 */
EXPORT void RedrawFromTo(WINFO *wp,long min,long max)
{
	SendRedraw(wp->ww_handle);
	P_redraw(wp,min,max,1);
}

/*--------------------------------------------------------------------------
 * RedrawTotalWindow()
 */
EXPORT void RedrawTotalWindow(FTABLE *fp)
{
	EdRedrawWindow(WIPOI(fp));
	curpos(fp->ln, (long)fp->lnoffset);
}

/*--------------------------------------------------------------------------
 * redrawlinepart()
 */
EXPORT void redrawlinepart(FTABLE *fp, long ln, int col1, int col2)
{
	WINFO *	wp;
	RECT  	r;

	wp = WIPOI(fp);
	if (wp->ww_handle > 0 && ln >= wp->minln && ln <= wp->maxln) {
		if (col1 < wp->mincol) {
			col1 = wp->mincol;
		}
		if (col2 > wp->maxcol) {
			col2 = wp->maxcol;
		}
		r.left = wp->workarea.g_x + (col1 - wp->mincol) * wp->cwidth;
		r.right = r.left + (col2 - col1) * wp->cwidth;
		r.top = wp->cheight * (ln - wp->minln);
		r.bottom = r.top + wp->cheight;
		InvalidateRect(wp->ww_handle, &r, 0);
	}
}

/*--------------------------------------------------------------------------
 * redrawline()
 */
EXPORT void redrawline(void)
{	
	WINFO *	wp;
	FTABLE *	fp;

	if ((fp = _currfile) != 0L) {
		wp = WIPOI(fp);
		redrawlinepart(fp, wp->ln, wp->mincol, wp->maxcol);
	}
}

/*--------------------------------------------------------------------------
 * redrawspecificline()
 */
EXPORT void redrawspecificline(FTABLE *fp, LINE *lpWhich)
{
	LINE *		lp;
	long			ln;
     WINFO *		wp;

	wp = WIPOI(fp);
	lp = ln_relgo(fp, wp->minln - wp->ln);
	for (ln = wp->minln; lp && ln <= wp->maxln; lp = lp->next, ln++) {
		if (lp == lpWhich) {
			redrawlinepart(fp, ln, wp->mincol, wp->maxcol);
			break;
		}
	}
}

/*--------------------------------------------------------------------------
 * OwnTextCursor()
 */
EXPORT void OwnTextCursor(int on)
{
	FTABLE 	*fp;
	WINFO 	*wp;

	if ((fp = _currfile) == 0 ||
	    (wp = WIPOI(fp)) == 0)
		return;
	if (on != wp->owncursor) {
		wp->owncursor = on;
		redrawline();
	}
}

