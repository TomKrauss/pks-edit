/*
 * CaretMovement.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: caret positioning
 *
 * 										created      : 15.03.91
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"
#include "editorconfiguration.h"

#include "caretmovement.h"
#include "textblocks.h"
#include "winfo.h"
#include "winterf.h"

#pragma hdrstop

#include "pksedit.h"

/*-----------------------*/
/* EXTERNALS			*/
/*-----------------------*/
extern LINE *		ln_relgo(FTABLE *fp, long l);
extern LINE *		ln_crelgo(FTABLE *fp, long l);
extern int		isword(unsigned char c);
extern int		isnospace(unsigned char c);
extern int 		ft_checkSelection(FTABLE *fp);
extern int 		bl_syncSelectionWithCaret(FTABLE *fp, CARET *caret, int flags);
extern void 	graf_mkstate(int *x, int *y, int *but, int *shift);

extern long		_multiplier;

/*--------------------------------------------------------------------------
 * caret_lineOffset2screen()
 * the following stuff is calculating the cursor screen position, out of
 * the internal offset to the line buffer
 */
EXPORT int caret_lineOffset2screen(FTABLE *fp, CARET *cp)
{	register int  col = 0,flags;
	register char *p = cp->linePointer->lbuf;
	register char* lbuf = p;
	int lnoffset = cp->offset;

	if (fp == NULL) {
		if (!ft_CurrentDocument()) {
			return 0;
		}
		fp = ft_CurrentDocument();
	}
	lbuf += cp->offset;
	flags = fp->documentDescriptor->dispmode;

	if (flags & SHOWATTR) {
		while (p < lbuf) {
			if (*p++ == '\t') {
				if ((flags & SHOWCONTROL) == 0) {
					col = TabStop(col, fp->documentDescriptor);
				} else {
					col++;
				}
				continue;
			}
			if (p[-1] == '\033') {
				p++;
			} else {
				col++;
			}
		}
		return col;
	}

	if (PLAINCONTROL(flags)) {
		col = lnoffset;
	} else {
		while (p < lbuf) {
			if (*p++ == '\t') 
				col = TabStop(col,fp->documentDescriptor);
			else col++;
		}
	}
	return col;
}

/*--------------------------------------------------------------------------
 * caret_screen2lineOffset()
 * Calculate the internal offset in the line buffer
 * for a given cursor screen position
 */
EXPORT int caret_screen2lineOffset(FTABLE *fp, CARET *pCaret)
{
	int  	i=0;
	int		flags;
	int col = pCaret->offset;
	LINE* lp = pCaret->linePointer;
	char *	p = lp->lbuf;
	char *	pend = &lp->lbuf[lp->len];

	if (fp == NULL) {
		if (!ft_CurrentDocument()) {
			return 0;
		}
		fp = ft_CurrentDocument();
	}
	flags = fp->documentDescriptor->dispmode;

	if (flags & SHOWATTR) {
		while (i < col && p < pend) {
			if (*p++ == '\t') {
				if ((flags & SHOWCONTROL) == 0) {
					i = TabStop(i, fp->documentDescriptor);
				} else {
					i++;
				}
				continue;
			}
			if (p[-1] == '\033') {
				p++;
			} else {
				i++;
			}
		}
		return (int)(p - lp->lbuf);
	}
	
	if (PLAINCONTROL(flags)) {
		if (col < 0) {
			return 0;
		}
		return col > lp->len ? lp->len : col;
	} else {
		while (i < col && p < pend) {
			if (*p++ == '\t')
				i = TabStop(i,fp->documentDescriptor);
			else i++;
		}
	}
	return (int)(p-lp->lbuf);
}

/*--------------------------------------------------------------------------
 * XtndBlock()
 */
static int _xtndMark;
static int nSentinel;
void XtndBlock(FTABLE *fp)
{
	if (nSentinel) {
		return;
	}
	nSentinel = 1;
	if (_xtndMark) {
		if (_xtndMark == MARK_START && 
			(fp->documentDescriptor->workmode & BLK_LINES)) {
			if (fp->blend && 
				ln_cnt(fp->caret.linePointer, fp->blend->lm) < 2) {
				_xtndMark = MARK_END;
			}
		}
		EdSyncSelectionWithCaret(_xtndMark | MARK_RECALCULATE);
	} else {
		EdSyncSelectionWithCaret(MARK_END | MARK_RECALCULATE);
	}
	nSentinel = 0;
}

/*--------------------------------------------------------------------------
 * BegXtndBlock()
 */
static void BegXtndBlock(FTABLE *fp)
{
	MARK	*		pMark;
	int			bMarkLines;

	if (nSentinel) {
		return;
	}
	bMarkLines = fp->documentDescriptor->workmode & BLK_LINES;
	nSentinel++;
	if ((pMark = fp->blstart) != 0 && 
		pMark->lm == fp->caret.linePointer &&
		(pMark->lc == fp->caret.offset ||
		 bMarkLines)) {
		_xtndMark = MARK_START;
	} else if ((pMark = fp->blend) != 0 && 
		((pMark->lm == fp->caret.linePointer &&
		  pMark->lc == fp->caret.offset) ||
		 (bMarkLines && fp->caret.linePointer->next == pMark->lm))) {
		_xtndMark = MARK_END;
	} else {
		_xtndMark = 0;
		if (fp->blstart || fp->blend) {
			EdBlockHide();
		}
		bl_syncSelectionWithCaret(fp, &fp->caret, MARK_RECALCULATE|MARK_START);
		bl_syncSelectionWithCaret(fp, &fp->caret, MARK_RECALCULATE|MARK_END);
	}
	nSentinel--;
}

/*--------------------------------------------------------------------------
 * HideWindowsBlocks()
 */
static void HideWindowsBlocks(FTABLE *fp)
{
	long col;

	if (!WIPOI(fp)->bXtndBlock &&
		(GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) &&
		ft_checkSelection(fp)) {
		col = fp->col;
		bl_hideSelection(1);
		fp->col = col;		/* save fix column position */
	}
}

/*--------------------------------------------------------------------------
 * cphypos()
 * cursoraddressierung with slider and mouse
 */
EXPORT int cphyspos(FTABLE *fp, long *ln,long *col,int newcol)
{
	LINE	 *		lp;
	int			i;
	BOOL			bXtnd;

	if ((lp = ln_crelgo(fp, *ln - fp->ln)) == 0) {
		return 0;
	}

	i = *col;
	if (!newcol && i < fp->col) {
		i = fp->col;
	}
	i = caret_screen2lineOffset(fp, &(CARET){lp, i});
	if (lp->len < i) {
		i = lp->len;
	}

	*col = caret_lineOffset2screen(fp, &(CARET) { lp, i});

	bXtnd = WIPOI(fp)->bXtndBlock;
	if (bXtnd) {
		BegXtndBlock(fp);
	}

	fp->ln		= *ln;
	fp->caret.linePointer 	= lp;
	fp->caret.offset	= i;
	if (newcol) 
		fp->col = *col;

	return 1;
}

/*--------------------------------------------------------------------------
 * cposvalid()
 */
EXPORT int cposvalid(FTABLE *fp, long *ln,long *col,int newcol)
{
	LINE *		lp;
	int 			i;
	int			o;
	BOOL			bXtnd;

	if ((lp = ln_crelgo(fp,*ln-fp->ln)) == 0L)
		return 0;

	o = *col;
	if (o < 0) {
		*col = 0;
		o    = 0;
	}

	if (fp->documentDescriptor->dispmode & SHOWATTR) {
		while(1) {
			if (o > 0 && lp->lbuf[o-1] == '\033')
				o++;
			else if (lp->lbuf[o] == '\033')
				o += 2;
			else
				break;
			if (o >= lp->len) {
				break;
			}
		}
	}

	if (o > lp->len) {
		o = lp->len;
	}

	i = caret_lineOffset2screen(fp, &(CARET) { lp, o});
	if (!newcol && i != fp->col)
		o = caret_screen2lineOffset(fp, &(CARET) {
		lp, fp->col
	});
	if (lp->len < o) o = lp->len;
	if (o != *col) i = caret_lineOffset2screen(fp, &(CARET) { lp, o});

	bXtnd = WIPOI(fp)->bXtndBlock;
	if (bXtnd) {
		BegXtndBlock(fp);
	}

	if (newcol) fp->col = i;
	fp->ln = *ln;
	fp->caret.linePointer = lp;
	fp->caret.offset = o;
	*col = i;

	return 1;
}

/**
 * Calculate the byte offset of the current caret in a file. 
 */
long wi_getCaretByteOffset(WINFO* view) {
	long offset = 0;
	FTABLE* fp = view->fp;
	LINE* lp = fp->firstl;

	if (fp->caret.linePointer == NULL) {
		return 0;
	}
	while (lp != NULL && lp != fp->caret.linePointer) {
		offset += lp->len;
		if (LINE_HAS_LINE_END(lp)) {
			offset += LINE_HAS_CR(lp) ? 2 : 1;
		}
		lp = lp->next;
	}
	return offset + fp->caret.offset;
}

/*--------------------------------------------------------------------------
 * curpos()
 * cursor absolut positioning
 */
EXPORT int _curpos(FTABLE *fp, long ln,long col)
{
	if (!cposvalid(fp,&ln,&col,1)) return 0;
	wt_curpos(WIPOI(fp),ln,col);
	return 1;
}

/*--------------------------------------------------------------------------
 * curpos()
 * cursor absolut positioning
 */
EXPORT int curpos(long ln,long col)
{
	return (ft_CurrentDocument()) ? _curpos(ft_CurrentDocument(),ln,col) : 0;
}

/*--------------------------------------------------------------------------
 * wi_adjust()
 * justify to middle of screen
 */
EXPORT static int wi_adjust(WINFO *wp, long ln,int adjustflag)
{	long pos,dy,dx;

	if (ln < wp->mincursln || ln > wp->maxcursln || (adjustflag & 2)) {
		if (adjustflag & 1) savecpos();
		if (wp->cursaftersearch == CP_POSTOP) pos = wp->mincursln;
		else if (wp->cursaftersearch == CP_POSLOW) pos = wp->maxcursln;
		else pos = ((wp->maxcursln+wp->mincursln)>>1);
		dy = ln-pos;
		dx = 0L;
		if (sl_scrollwinrange(wp,&dy,&dx))
			sl_winchanged(wp,dy,dx);
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * centernewpos()
 */
EXPORT int centernewpos(long ln,long col)
{	WINFO *wp;

	if (!ft_CurrentDocument())
		return 0;
	wp = WIPOI(ft_CurrentDocument());

	return wi_adjust(wp,ln,1) ? 
			curpos(ln,col) 
			: 
			newcpos(ln,col);
}

/*--------------------------------------------------------------------------
 * centercpos()
 */
EXPORT int centercpos(long ln,long col)
{	WINFO *wp;

	if (!ft_CurrentDocument())
		return 0;
	wp = WIPOI(ft_CurrentDocument());

	wi_adjust(wp,ln,0);
	return curpos(ln,col);
}

/*--------------------------------------------------------------------------
 * center2cpos()
 */
EXPORT int center2cpos(FTABLE *fp, long ln,long col)
{
	wi_adjust(WIPOI(fp),ln,3);
	return _curpos(fp,ln,col);
}

/*--------------------------------------------------------------------------
 * cadv_page()
 * cursor advance one screen 
 */
EXPORT int cadv_page(long *ln,int dir)
{	register ds;
	register FTABLE *fp;
	WINFO    *wp;

	fp = ft_CurrentDocument();
	wp = WIPOI(fp);
	ds = wp->maxcursln-wp->mincursln;
	*ln = fp->ln + (dir * ds * _multiplier);
	if (*ln < 0L) *ln = 0L;
	else if (*ln >= fp->nlines) *ln = fp->nlines-1L;
	return 1;
}

/*--------------------------------------------------------------------------
 * ln_leadspce(l)
 */
EXPORT int ln_leadspce(LINE *lp)
{	
	return CntSpaces(lp->lbuf,lp->len);
}

/*--------------------------------------------------------------------------
 * isempty()
 */
EXPORT int isempty(LINE *lp)
{
	return (ln_leadspce(lp) == lp->len);
}

/*--------------------------------------------------------------------------
 * counttabs()
 * calculate nr of tabs in line
 * depending on shiftwidth
 */
EXPORT static int counttabs(LINE *lp)
{
	return caret_lineOffset2screen(NULL, &(CARET){ lp,ln_leadspce(lp) });
}

/*--------------------------------------------------------------------------
 * tabedstart()
 */
EXPORT int tabedstart(LINE *lp) 
{	 return (counttabs(lp) != counttabs(lp->prev)); }

/*--------------------------------------------------------------------------
 * tabedend()
 */
EXPORT int tabedend(LINE *lp)
{	return (!lp->next || (counttabs(lp->next) != counttabs(lp))); }

/*--------------------------------------------------------------------------
 * pgrstart()
 */
EXPORT int pgrend(LINE *lp)
{	return (isempty(lp) && !isempty(lp->prev)); }

/*--------------------------------------------------------------------------
 * pgrstart()
 */
EXPORT int pgrstart(LINE *lp)
{	return (isempty(lp->prev) && !isempty(lp)); }

/*--------------------------------------------------------------------------
 * cadv_section()
 * advance one section (depending on func)
 */
static void cadv_section(long *ln,int dir,int start,
					int (*func1)(),int (*func2)())
					/* pgrstart,pgrend for advancing paragraphs */
					/* equaltabed(line) for advancing sections */
{	register LINE *lp;
	register FTABLE *fp;
	register long lnr,mul;

	fp  = ft_CurrentDocument();
	lnr = *ln;
	if ((lp = ln_relgo(fp,lnr-fp->ln)) == 0)
		return;

	if (dir > 0 && !start) {
		lp=lp->next; lnr++;
	}
	for (mul = 0; mul < _multiplier; mul++) {
		while ((dir < 0 && (lp = lp->prev) != 0L) || 
			  (dir > 0 && (lp = lp->next) != 0 && lp->next != 0)) {
			lnr += dir;
			if (start) {
				if ((dir < 0 && !lp->prev)) break;
				if ((*func1)(lp)) goto nextblk;
			} else 
				if ((*func2)(lp)) goto nextblk;
		}
		break;
nextblk:	;
	}
	*ln = lnr;
}

/*--------------------------------------------------------------------------
 * cparagrph()
 */
EXPORT long cparagrph(long ln,int dir,int start)
{
	cadv_section(&ln,dir,start,pgrstart,pgrend);
	if (!start) ln--;
	return ln;
}

/*--------------------------------------------------------------------------
 * curspgrph()
 */
EXPORT int curspgrph(int dir,int start)
{	long ln;

	ln = cparagrph(ft_CurrentDocument()->ln,dir,start);
	return newcpos(ln,0L);
}

/*--------------------------------------------------------------------------
 * cursabsatz()
 */
EXPORT int cursabsatz(int dir,int start)
{	long ln;
	FTABLE *fp = ft_CurrentDocument();

	savecpos();
	ln = fp->ln;
	cadv_section(&ln,dir,start,tabedstart,tabedend);
	curpos(ln,0L);

	return curpos(ln,(long)ln_leadspce(fp->caret.linePointer));
}

/*--------------------------------------------------------------------------
 * ComparePosition()
 * check for position 2 > position 1
 */
# if 0
static int ComparePosition(FTABLE *fp, LINE *lp1, int col1, LINE *lp2, int col2)
{
	if (lp1 == lp2) {
		return col2 - col1;
	}
	while((lp1 = lp1->prev) != 0) {
		if (lp1 == lp2) {
			return -1;
		}
	}
	return 1;
}
# endif

/*--------------------------------------------------------------------------
 * cursupdown()
 * general cursor advancing in
 * vertical direction
 */
EXPORT int cursupdown(int dir, int mtype)
{
	BOOL		bXtnd;
	int		nRet;
	long 	col;
	long 	ln;
	FTABLE *	fp = ft_CurrentDocument(); 
	WINFO *	wp = WIPOI(fp);

	bXtnd = wp->bXtndBlock;
	nRet = 0;
	if (mtype & MOT_XTNDBLOCK) {
		wp->bXtndBlock = TRUE;
	}
	HideWindowsBlocks(fp);

	col = fp->caret.offset, ln = fp->ln;
	switch(mtype & (~MOT_XTNDBLOCK)) {
		case MOT_SINGLE:
			ln += (dir * _multiplier);
			break;
		case MOT_TOEND:
			ln = (dir > 0L) ? wp->maxcursln : wp->mincursln;
			break;
		case MOT_PAGE:
			if (!cadv_page(&ln,dir)) {
				goto err;
			}
			break;
		case MOT_CENTER:
			ln = (wp->maxln+wp->minln) / 2;
			break;
		case MOT_FILE:
			if (!newcpos((dir > 0) ? fp->nlines-1L : 0L,0L)) {
				goto err;
			}
			nRet = 1;
			goto err;	
	}

	if (cposvalid(fp,&ln,&col,0)) {
		nRet = 1;
		wt_curpos(wp,ln,col);
	}
err:
	wp->bXtndBlock = bXtnd;
	return nRet;
}

/*--------------------------------------------------------------------------
 * cadv_word()
 * cursor advance one word
 */
static LINE *nextw(LINE *lp,long *ln,long *col,
	int (*iswfunc)(unsigned char c),int dir, int bNo)
{	register unsigned char *p;
	register long l;
	register int c,len;

	l   = *ln;
	c   = *col;
	p   = &lp->lbuf[c];
	len =  lp->len;

	if (dir < 0) {
		if (c == 0) {
			if (l-- <= 0) return 0;
			lp = lp->prev;
			c  = lp->len;
		} else {
			p--;
			if (bNo) {
				while (c && !(*iswfunc)(*p)) { p-- , c--; }
			}
			while (c &&  (*iswfunc)(*p)) { p-- , c--; }
		}
	} else {
		if (c == len) {
			if (++l >= ft_CurrentDocument()->nlines) return 0;
			lp = lp->next;
			c  = 0;
		} else {
			while ( (*iswfunc)(*p) && c < len) { p++ , c++; }
			if (bNo) {
				while (!(*iswfunc)(*p) && c < len) { p++ , c++; }
			}
		}
	}
	*ln = l, *col = c;
	return lp;
}

/*--------------------------------------------------------------------------
 * cadv_wordonly()
 */
EXPORT LINE *cadv_wordonly(LINE *lp,long *ln,long *col,int dir)
{
	return nextw(lp,ln,col,isnospace,dir,0);
}

/*--------------------------------------------------------------------------
 * cadv_space()
 */
EXPORT LINE *cadv_space(LINE *lp,long *ln,long *col,int dir)
{
	return nextw(lp,ln,col,isnospace,dir,1);
}

/*--------------------------------------------------------------------------
 * cadv_word()
 */
EXPORT LINE *cadv_word(LINE *lp,long *ln,long *col,int dir)
{
	return nextw(lp,ln,col,isword,dir,1);
}

/*--------------------------------------------------------------------------
 * cadv_c()
 * cursor advance to char
 */
EXPORT LINE *cadv_c(LINE *lp,long *ln,long *col,int dir,unsigned char match)
{	register long l;
	register int c;

	l = *ln, c = *col;
	if (dir < 0) {
		for (;;) {
			if (--c < 0) {
				if ((lp = lp->prev) == 0)
					return 0;
				l--;
				c  = lp->len;
			} else
				if (lp->lbuf[c] == match)
					break;;
		}
	} else {
		for (;;) {
			if (++c >= lp->len) {
				lp = lp->next;
				if (lp->next == 0)
					return 0;
				l++;
				c = 0;
			}
			if (lp->lbuf[c] == match)
				break;
		}
	}
	*ln = l, *col = c;
	return lp;
}

/*--------------------------------------------------------------------------
 * l_advance()
 */
EXPORT int l_advance(LINE *lp, int col)
{
	col--;
	if (ft_CurrentDocument()->documentDescriptor->dispmode & SHOWATTR) {
		while (col > 1) {
			if (lp->lbuf[col-1] == '\033')
				col--;
			if (lp->lbuf[col] == '\033')
				col--;
			else break;
		}
	}
	return col;
}

/*--------------------------------------------------------------------------
 * setmatchfunc()
 */
LINE * (*advmatchfunc)();
EXPORT void setmatchfunc(int mtype, int ids_name, int *c)
{
	advmatchfunc = cadv_word;
	switch (abs(mtype)) {
		case MOT_UNTILC:
			*c = getuntilc(ids_name);
			advmatchfunc = cadv_c;
			break;
		case 5:
			advmatchfunc = cadv_space;
			break;
	}
}

/*--------------------------------------------------------------------------
 * caret_moveLeftRight()
 * Move the caret to the left or right. If motionFlags contains MOT_XTNDBLOCK
 * the selection is extended.
 */
EXPORT int caret_moveLeftRight(int direction, int motionFlags)
{
	FTABLE *	fp;
	WINFO *	wp;
	long  	ln;
	long 	col;
	LINE *	lp;
	int   	matchc;
	int		moving;
	int		nRet;
	BOOL		bXtnd;

	fp = ft_CurrentDocument();
	wp = WIPOI(fp);
	col = fp->caret.offset;
	lp = fp->caret.linePointer;
	ln = fp->ln;
	bXtnd = wp->bXtndBlock;
	nRet = 0;

	if (motionFlags & MOT_XTNDBLOCK) {
		wp->bXtndBlock = TRUE;
	}

	HideWindowsBlocks(fp);

	moving = direction * (motionFlags & (~MOT_XTNDBLOCK));
	setmatchfunc(moving , IDS_CURSUNTILC, &matchc);

	switch(moving) {
		case  MOT_SINGLE:
			if (col >= lp->len) {
				lp = lp->next;
				if (lp->next == 0) {
					goto err;
				}
				ln++;
				col = 0;
			} else col++;
			break;
		case  MOT_TOEND:
			col = lp->len;
			break;
		case  MOT_WORD:	case -MOT_WORD:
		case  MOT_UNTILC:	case -MOT_UNTILC:
		case	 MOT_SPACE:	case -MOT_SPACE:
			if ((lp =	(*advmatchfunc)(lp,&ln,&col,
				(moving > 0) ? 1 : -1,matchc)) == 0) {
				goto err;
			}
			break;
		case -MOT_SINGLE:
			if (!col) {
				if ((lp = lp->prev) == 0)
					goto err;
				ln--;
				col = lp->len;
			} else {
				col = l_advance(lp,col);
			}
			break;
		case -MOT_TOEND:
			col = 0L;
			break;
	}
	nRet = _curpos(fp, ln, col);

err:
	wp->bXtndBlock = bXtnd;

	return nRet;
}

/*---------------------------------*
 * caret_calculateOffsetFromScreen()
 * Calculate the offset in the file (line and column) given the X and Y coordinates
 * on which a mouse was clicked. No validation regarding the validity of the column
 * is performed at that point in time.
 */
EXPORT void caret_calculateOffsetFromScreen(WINFO *wp, int x, int y, long *line, long *column)
{	FTABLE *fp = FTPOI(wp);

	*column = x / wp->cwidth + wp->mincol;
	*line	= y / wp->cheight + wp->minln;
	if (*line < 0) {
		*line = 0;
	} else if (*line >= fp->nlines) {
		*line = fp->nlines - 1;
	}
}

/*--------------------------------------------------------------------------
 * caret_moveToXY()
 * move the caret to follow the mouse pressed on the screen coordinates x and y.
 */
EXPORT int caret_moveToXY(WINFO *wp, int x,int y)
{	long col,ln;
	int b=1,dummy;
	UINT_PTR id;

	SetCapture(wp->ww_handle);
	id = SetTimer(0,0,100,0);
	for (;;) {
		caret_calculateOffsetFromScreen(wp,x,y,&ln,&col);
		if (cphyspos(FTPOI(wp),&ln,&col,1)) {
			wt_curpos(wp,ln,col);
		}
		graf_mkstate(&x,&y,&b,&dummy);
		if (!b) break;
	}
	if (id)
		KillTimer(0,id);
	ReleaseCapture();
	return 1;
}

/*--------------------------------------------------------------------------
 * EdMousePosition()
 */
EXPORT int MousePosition(FTABLE *fp, long bAsk)
{	
	int 		x;
	int		y;
	int		b = 0;
	int		ret;
	WINFO *	wp;

	wp = WIPOI(fp);
	MouseGetXYPos(wp->ww_handle,&x,&y);

	if (bAsk && _playing) {
		forceredraw();
		ed_error(IDS_MSGPOSITIONMOUSECURS);
		do {
			graf_mkstate(&x,&y,&b,&ret);
		} while(!b);
		CloseErrorWin();
	}

	ret = caret_moveToXY(wp,x,y);

	if (bAsk && _playing) {
		forceredraw();
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * EdMousePosition()
 */
EXPORT int EdMousePosition(long bAsk)
{	
	FTABLE *	fp;

	if ((fp = ft_CurrentDocument()) == 0) {
		return 0;
	}

	HideWindowsBlocks(fp);
	MousePosition(fp, bAsk);
	return 1;
}

/*--------------------------------------------------------------------------
 * EdMousePositionUngrabbed()
 */
int EdMousePositionUngrabbed(long bGrab)
{
	FTABLE *	fp;
	WINFO *	wp;
	int		x;
	int		y;
	long		ln;
	long		col;

	if (bGrab) {
		return EdMousePosition(1);
	}

	if ((fp = ft_CurrentDocument()) == 0) {
		return 0;
	}

	wp = WIPOI(fp);
	HideWindowsBlocks(fp);
	MouseGetXYPos(wp->ww_handle,&x,&y);
	caret_calculateOffsetFromScreen(wp,x,y,&ln,&col);
	if (cphyspos(FTPOI(wp),&ln,&col,1)) {
		wt_curpos(wp,ln,col);
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdBlockXtndMode()
 */
int EdBlockXtndMode(long bOn)
{
	FTABLE *	fp;

	if ((fp = ft_CurrentDocument()) == 0) {
		return 0;
	}

	WIPOI(fp)->bXtndBlock = bOn ? TRUE : FALSE;

	return 1;
}

