/*
 * CaretMovement.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: caret positioning
 *
 * 										created: 15.03.91
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"

#include "caretmovement.h"
#include "textblocks.h"
#include "winfo.h"
#include "winterf.h"
#include "mouseutil.h"
#include "stringutil.h"
#include "xdialog.h"
#include "pksedit.h"

extern long		_multiplier;

/*--------------------------------------------------------------------------
 * caret_lineOffset2screen()
 * the following stuff is calculating the cursor screen position, out of
 * the internal offset to the line buffer
 */
EXPORT int caret_lineOffset2screen(WINFO *wp, CARET *cp)
{	register int  col = 0;
	register char *p = cp->linePointer->lbuf;
	register char* lbuf = p;
	int lnoffset = cp->offset;

	if (wp == NULL) {
		wp = ww_getCurrentEditorWindow();
		if (!wp) {
			return 0;
		}
	}
	lbuf += cp->offset;
	FTABLE* fp = wp->fp;

	while (p < lbuf) {
		if (*p++ == '\t') 
			col = doctypes_calculateNextTabStop(col,fp->documentDescriptor);
		else col++;
	}
	return col;
}

/*--------------------------------------------------------------------------
 * caret_screen2lineOffset()
 * Calculate the internal offset in the line buffer
 * for a given cursor screen position
 */
EXPORT int caret_screen2lineOffset(WINFO *wp, CARET *pCaret)
{
	int  	i=0;
	int col = pCaret->offset;
	LINE* lp = pCaret->linePointer;
	char *	p = lp->lbuf;
	char *	pend = &lp->lbuf[lp->len];

	if (wp == NULL) {
		wp = ww_getCurrentEditorWindow();
		if (wp == NULL) {
			return 0;
		}
	}
	FTABLE* fp = wp->fp;

	while (i < col && p < pend) {
		if (*p++ == '\t')
			i = doctypes_calculateNextTabStop(i,fp->documentDescriptor);
		else i++;
	}
	return (int)(p-lp->lbuf);
}

/*--------------------------------------------------------------------------
 * caret_extendSelection()
 */
static int _xtndMark;
static int nSentinel;
void caret_extendSelection(WINFO *wp)
{
	if (nSentinel) {
		return;
	}
	nSentinel = 1;
	if (_xtndMark) {
		if (_xtndMark == MARK_START && 
			(wp->workmode & BLK_LINES)) {
			if (wp->blend && 
				ln_cnt(wp->caret.linePointer, wp->blend->lm) < 2) {
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
static void BegXtndBlock(WINFO *wp)
{
	MARK	*		pMark;
	int			bMarkLines;

	if (nSentinel) {
		return;
	}
	bMarkLines = wp->workmode & BLK_LINES;
	nSentinel++;
	if ((pMark = wp->blstart) != 0 && 
		pMark->lm == wp->caret.linePointer &&
		(pMark->lc == wp->caret.offset ||
		 bMarkLines)) {
		_xtndMark = MARK_START;
	} else if ((pMark = wp->blend) != 0 && 
		((pMark->lm == wp->caret.linePointer &&
		  pMark->lc == wp->caret.offset) ||
		 (bMarkLines && wp->caret.linePointer->next == pMark->lm))) {
		_xtndMark = MARK_END;
	} else {
		_xtndMark = 0;
		if (wp->blstart || wp->blend) {
			EdBlockHide();
		}
		bl_syncSelectionWithCaret(wp, &wp->caret, MARK_RECALCULATE|MARK_START, NULL);
		bl_syncSelectionWithCaret(wp, &wp->caret, MARK_RECALCULATE|MARK_END, NULL);
	}
	nSentinel--;
}

/*--------------------------------------------------------------------------
 * HideWindowsBlocks()
 */
static void HideWindowsBlocks(WINFO *wp)
{
	long col;

	if (!wp->bXtndBlock &&
		(GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) &&
		ft_checkSelection(wp)) {
		col = wp->caret.virtualOffset;
		bl_hideSelection(wp, 1);
		wp->caret.virtualOffset = col;		/* restore virtual column position */
	}
}

/*
 * Move the caret to the given line. 
 */
void caret_moveToLine(WINFO* wp, long ln) {
	if (wp->caret.ln == ln) {
		return;
	}
	long oldln = wp->caret.ln;
	wp->caret.ln = ln;
	if (wp->dispmode & SHOWCARET_LINE_HIGHLIGHT) {
		int ln1;
		int ln2;
		if (oldln < wp->caret.ln) {
			ln1 = oldln;
			ln2 = wp->caret.ln;
		}
		else {
			ln1 = wp->caret.ln;
			ln2 = oldln;
		}
		render_repaintFromLineTo(wp->fp, ln1, ln2);
	}
}

/*--------------------------------------------------------------------------
 * caret_updateLineColumn()
 * Invoked, when the cursor is positioned using slider or mouse to update the
 * caret position. 
 */
EXPORT int caret_updateLineColumn(WINFO *wp, long *ln, long *col, int updateVirtualColumn) {
	LINE *	lp;
	int		i;
	BOOL	bXtnd;
	FTABLE* fp = wp->fp;

	if ((lp = ln_goto(fp, *ln)) == 0) {
		return 0;
	}

	i = *col;
	if (!updateVirtualColumn && i < wp->caret.virtualOffset) {
		i = wp->caret.virtualOffset;
	}
	i = caret_screen2lineOffset(wp, &(CARET) {lp, i});
	if (lp->len < i) {
		i = lp->len;
	}

	*col = caret_lineOffset2screen(wp, &(CARET) { lp, i});

	bXtnd = wp->bXtndBlock;
	if (bXtnd) {
		BegXtndBlock(wp);
	}

	wp->caret.linePointer = lp;
	wp->caret.offset = i;
	caret_moveToLine(wp, *ln);
	if (updateVirtualColumn) {
		wp->caret.virtualOffset = *col;
	}

	return 1;
}


/*--------------------------------------------------------------------------
 * caret_placeCursorAndValidate()
 */
EXPORT int caret_placeCursorAndValidate(WINFO *wp, long *ln,long *col,int updateVirtualOffset)
{
	LINE *		lp;
	int 		i;
	int			o;
	BOOL		bXtnd;
	FTABLE* fp = wp->fp;

	if ((lp = ln_goto(fp,*ln)) == 0L)
		return 0;

	o = *col;
	if (o < 0) {
		*col = 0;
		o    = 0;
	}

	if (o > lp->len) {
		o = lp->len;
	}

	i = caret_lineOffset2screen(wp, &(CARET) { lp, o});
	if (!updateVirtualOffset && i != wp->caret.virtualOffset)
		o = caret_screen2lineOffset(wp, &(CARET) {
		lp, wp->caret.virtualOffset
	});
	if (lp->len < o) o = lp->len;
	if (o != *col) i = caret_lineOffset2screen(wp, &(CARET) { lp, o});

	bXtnd = wp->bXtndBlock;
	if (bXtnd) {
		BegXtndBlock(wp);
	}
	if (updateVirtualOffset) {
		wp->caret.virtualOffset = i;
	}
	wp->caret.linePointer = lp;
	wp->caret.offset = o;
	caret_moveToLine(wp, *ln);
	*col = i;

	return 1;
}

/**
 * Calculate the byte offset of the current caret in a file. 
 */
long wi_getCaretByteOffset(WINFO* wp) {
	long offset = 0;
	FTABLE* fp = wp->fp;
	LINE* lp = fp->firstl;

	if (wp->caret.linePointer == NULL) {
		return 0;
	}
	while (lp != NULL && lp != wp->caret.linePointer) {
		offset += lp->len;
		if (LINE_HAS_LINE_END(lp)) {
			offset += LINE_HAS_CR(lp) ? 2 : 1;
		}
		lp = lp->next;
	}
	return offset + wp->caret.offset;
}

/*--------------------------------------------------------------------------
 * caret_placeCursorForFile()
 * cursor absolut positioning for the given file.
 */
EXPORT int caret_placeCursorForFile(WINFO *wp, long ln,long col)
{
	if (!caret_placeCursorAndValidate(wp,&ln,&col,1)) return 0;
	wt_curpos(wp,ln,col);
	return 1;
}

/*--------------------------------------------------------------------------
 * caret_placeCursorInCurrentFile()
 * cursor absolut positioning for the current file.
 */
EXPORT int caret_placeCursorInCurrentFile(WINFO* wp, long ln,long col)
{
	return wp ? caret_placeCursorForFile(wp,ln,col) : 0;
}

/*--------------------------------------------------------------------------
 * caret_saveLastPosition()
 * Store the last cursor position so one can navigate "back" for the current
 * editor window.
 */
int caret_saveLastPosition(void) {
	WINFO* wp = ww_getCurrentEditorWindow();
	register FTABLE* fp;

	if (wp != 0) {
		fp = wp->fp;
		fp->lastln = wp->caret.ln,
		fp->lastcol = wp->caret.offset;
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * wi_adjust()
 * justify to middle of screen
 */
EXPORT static int wi_adjust(WINFO *wp, long ln,int adjustflag)
{	long pos,dy,dx;

	if (ln < wp->mincursln || ln > wp->maxcursln || (adjustflag & 2)) {
		if (adjustflag & 1) caret_saveLastPosition();
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
 * caret_placeCursorMakeVisibleAndSaveLocation()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible - save the last cursor position, if we needed to scroll.
 */
EXPORT int caret_placeCursorMakeVisibleAndSaveLocation(WINFO* wp, long ln,long col)
{
	if (wp == NULL) {
		return 0;
	}

	return wi_adjust(wp,ln,1) ? 
			caret_placeCursorInCurrentFile(wp, ln,col) 
			: 
			caret_placeCursorAndSavePosition(wp, ln,col);
}

/*--------------------------------------------------------------------------
 * caret_placeCursorAndSavePosition()
 * Places the cursor in the current document in a given row and column and save the current
 * position.
 */
int caret_placeCursorAndSavePosition(WINFO* wp, long ln, long col)
{
	FTABLE* fp;

	if (wp == NULL)
		return 0;
	fp = wp->fp;
	if (ln == wp->caret.ln && col == wp->caret.offset)
		return 1;

	caret_saveLastPosition();
	return caret_placeCursorInCurrentFile(wp, ln, col);
}

/*--------------------------------------------------------------------------
 * caret_placeCursorMakeVisible()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible.
 */
EXPORT int caret_placeCursorMakeVisible(WINFO* wp, long ln, long col)
{
	if (wp == NULL) {
		return 0;
	}
	wi_adjust(wp,ln,0);
	return caret_placeCursorInCurrentFile(wp, ln,col);
}

/*--------------------------------------------------------------------------
 * caret_placeCursorAndMakevisibleWithSpace()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible. Leave some rows space at top / bottom of the screen.
 */
EXPORT int caret_placeCursorAndMakevisibleWithSpace(WINFO *wp, long ln,long col)
{
	wi_adjust(wp,ln,3);
	return caret_placeCursorForFile(wp,ln,col);
}

/*--------------------------------------------------------------------------
 * caret_advancePage()
 * cursor advance one screen 
 */
EXPORT int caret_advancePage(WINFO* wp, long *ln,int dir)
{	register ds;
	register FTABLE *fp;

	if (wp == NULL) {
		return 0;
	}
	fp = wp->fp;
	ds = wp->maxcursln-wp->mincursln;
	*ln = wp->caret.ln + (dir * ds * _multiplier);
	if (*ln < 0L) *ln = 0L;
	else if (*ln >= fp->nlines) *ln = fp->nlines-1L;
	return 1;
}

/*--------------------------------------------------------------------------
 * ln_countLeadingSpaces(l)
 */
EXPORT int ln_countLeadingSpaces(LINE *lp)
{	
	return string_countSpacesIn(lp->lbuf,lp->len);
}

/*--------------------------------------------------------------------------
 * ln_lineIsEmpty()
 */
EXPORT int ln_lineIsEmpty(LINE *lp)
{
	return (ln_countLeadingSpaces(lp) == lp->len);
}

/*--------------------------------------------------------------------------
 * counttabs()
 * calculate nr of tabs in line
 * depending on shiftwidth
 */
static int counttabs(LINE *lp)
{
	return caret_lineOffset2screen(NULL, &(CARET){ lp,ln_countLeadingSpaces(lp) });
}

/*--------------------------------------------------------------------------
 * tabedstart()
 */
static int tabedstart(LINE *lp) 
{	 return (counttabs(lp) != counttabs(lp->prev)); }

/*--------------------------------------------------------------------------
 * tabedend()
 */
static int tabedend(LINE *lp)
{	return (!lp->next || (counttabs(lp->next) != counttabs(lp))); }

/*--------------------------------------------------------------------------
 * pgrstart()
 */
static int pgrend(LINE *lp)
{	return (ln_lineIsEmpty(lp) && !ln_lineIsEmpty(lp->prev)); }

/*--------------------------------------------------------------------------
 * pgrstart()
 */
static int pgrstart(LINE *lp)
{	return (ln_lineIsEmpty(lp->prev) && !ln_lineIsEmpty(lp)); }

/*--------------------------------------------------------------------------
 * cadv_section()
 * advance one section (depending on func)
 */
static void cadv_section(WINFO* wp, long *ln,int dir,int start,
					int (*func1)(),int (*func2)())
					/* pgrstart,pgrend for advancing paragraphs */
					/* equaltabed(line) for advancing sections */
{	register LINE *lp;
	register FTABLE *fp;
	register long lnr,mul;

	fp  = wp->fp;
	lnr = *ln;
	if ((lp = ln_goto(fp,lnr)) == 0)
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
 * caret_advanceParagraph()
 * Advance the cursor starting from a line by one paragraph.
 */
EXPORT long caret_advanceParagraph(WINFO* wp, long ln,int dir,int start)
{
	cadv_section(wp, &ln,dir,start,pgrstart,pgrend);
	if (!start) ln--;
	return ln;
}

/*--------------------------------------------------------------------------
 * caret_advanceParagraphFromCurrentLine()
 */
EXPORT int caret_advanceParagraphFromCurrentLine(WINFO* wp, int dir,int start)
{	long ln;
	FTABLE* fp = wp->fp;

	ln = caret_advanceParagraph(wp, wp->caret.ln,dir,start);
	return caret_placeCursorAndSavePosition(wp, ln,0L);
}

/*--------------------------------------------------------------------------
 * caret_advanceSection()
 * Advances the cursor by one "section".
 */
EXPORT int caret_advanceSection(WINFO* wp, int dir,int start)
{	long ln;
	FTABLE *fp = wp->fp;

	caret_saveLastPosition();
	ln = wp->caret.ln;
	cadv_section(wp, &ln,dir,start,tabedstart,tabedend);
	caret_placeCursorInCurrentFile(wp, ln,0L);

	return caret_placeCursorInCurrentFile(wp, ln,(long)ln_countLeadingSpaces(wp->caret.linePointer));
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
 * caret_moveUpOrDown()
 * general cursor advancing in
 * vertical direction
 */
EXPORT int caret_moveUpOrDown(WINFO* wp, int dir, int mtype)
{
	BOOL		bXtnd;
	int		nRet;
	long 	col;
	long 	ln;
	FTABLE* fp = wp->fp;

	bXtnd = wp->bXtndBlock;
	nRet = 0;
	if (mtype & MOT_XTNDBLOCK) {
		wp->bXtndBlock = TRUE;
	}
	HideWindowsBlocks(wp);

	col = wp->caret.offset, ln = wp->caret.ln;
	switch(mtype & (~MOT_XTNDBLOCK)) {
		case MOT_SINGLE:
			ln += (dir * _multiplier);
			break;
		case MOT_TOEND:
			ln = (dir > 0L) ? wp->maxcursln : wp->mincursln;
			break;
		case MOT_PAGE:
			if (!caret_advancePage(wp, &ln,dir)) {
				goto err;
			}
			break;
		case MOT_CENTER:
			ln = (wp->maxln+wp->minln) / 2;
			break;
		case MOT_FILE:
			if (!caret_placeCursorAndSavePosition(wp, (dir > 0) ? fp->nlines-1L : 0L,0L)) {
				goto err;
			}
			nRet = 1;
			goto err;	
	}

	if (caret_placeCursorAndValidate(wp,&ln,&col,0)) {
		nRet = 1;
		wt_curpos(wp,ln,col);
	}
err:
	wp->bXtndBlock = bXtnd;
	return nRet;
}

/*--------------------------------------------------------------------------
 * cadv_gotoIdentifierEnd()
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
			if (++l >= ft_getCurrentDocument()->nlines) return 0;
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
	return nextw(lp,ln,col,char_isNospace,dir,0);
}

/*--------------------------------------------------------------------------
 * cadv_gotoIdentifierSkipSpace()
 */
EXPORT LINE *cadv_gotoIdentifierSkipSpace(LINE *lp,long *ln,long *col,int dir)
{
	return nextw(lp,ln,col, char_isIdentifier,dir,1);
}

/*--------------------------------------------------------------------------
 * cadv_gotoIdentifierEnd()
 */
EXPORT LINE *cadv_gotoIdentifierEnd(LINE *lp,long *ln,long *col,int dir)
{
	return nextw(lp,ln,col,char_isIdentifier,dir,0);
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
					break;
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
 * caret_getPreviousColumnInLine()
 * Got back one screen column in a line and return the new column considering internal 
 * structure of the data and return the new offset into the line buffer.
 */
EXPORT int caret_getPreviousColumnInLine(WINFO* wp, LINE *lp, int col) {
	return col-1;
}

/*--------------------------------------------------------------------------
 * caret_setMatchFunction()
 */
LINE * (*advmatchfunc)();
EXPORT void caret_setMatchFunction(int mtype, int ids_name, int *c)
{
	advmatchfunc = cadv_gotoIdentifierEnd;
	switch (abs(mtype)) {
		case MOT_UNTILC:
			*c = EdPromptForCharacter(ids_name);
			advmatchfunc = cadv_c;
			break;
		case MOT_SPACE:
			advmatchfunc = cadv_gotoIdentifierSkipSpace;
			break;
	}
}

/*--------------------------------------------------------------------------
 * caret_moveLeftRight()
 * Move the caret to the left or right. If motionFlags contains MOT_XTNDBLOCK
 * the selection is extended.
 */
EXPORT int caret_moveLeftRight(WINFO* wp, int direction, int motionFlags)
{
	FTABLE *fp;
	long  	ln;
	long 	col;
	LINE *	lp;
	int   	matchc;
	int		moving;
	int		nRet;
	BOOL		bXtnd;

	fp = wp->fp;
	col = wp->caret.offset;
	lp = wp->caret.linePointer;
	ln = wp->caret.ln;
	bXtnd = wp->bXtndBlock;
	nRet = 0;

	if (motionFlags & MOT_XTNDBLOCK) {
		wp->bXtndBlock = TRUE;
	}

	HideWindowsBlocks(wp);

	moving = direction * (motionFlags & (~MOT_XTNDBLOCK));
	caret_setMatchFunction(moving , IDS_CURSUNTILC, &matchc);

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
		case MOT_WORD:    case -MOT_WORD:
		case MOT_UNTILC:  case -MOT_UNTILC:
		case MOT_SPACE:   case -MOT_SPACE:
			if ((lp = (*advmatchfunc)(lp,&ln,&col,
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
				col = caret_getPreviousColumnInLine(wp, lp,col);
			}
			break;
		case -MOT_TOEND:
			col = 0L;
			break;
	}
	nRet = caret_placeCursorForFile(wp, ln, col);

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

	*column = (x + wp->cwidth/2) / wp->cwidth + wp->mincol;
	*line	= y / wp->cheight + wp->minln;
	if (*line < 0) {
		*line = 0;
	} else if (*line >= fp->nlines) {
		*line = fp->nlines - 1;
	}
}

/*--------------------------------------------------------------------------
 * caret_moveToCurrentMousePosition()
 */
EXPORT int caret_moveToCurrentMousePosition(WINFO *wp, long bAsk)
{	
	int 		x;
	int		y;
	int		ret;
	int		b = 0;

	mouse_getXYPos(wp->ww_handle,&x,&y);

	if (bAsk && _playing) {
		error_showErrorById(IDS_MSGPOSITIONMOUSECURS);
		do {
			mouse_dispatchUntilButtonRelease(&x,&y,&b,&ret);
		} while(!b);
		error_closeErrorWindow();
	}

	return caret_moveToXY(wp,x,y);
}

/*--------------------------------------------------------------------------
 * caret_positionCloseToMouseWithConfirmation()
 */
EXPORT int caret_positionCloseToMouseWithConfirmation(long bAsk)
{	
	WINFO *	wp;

	if ((wp = ww_getCurrentEditorWindow()) == 0) {
		return 0;
	}

	caret_moveToCurrentMousePosition(wp, bAsk);
	return 1;
}

/*--------------------------------------------------------------------------
 * EdMousePositionUngrabbed()
 */
int EdMousePositionUngrabbed(long bGrab) {
	return caret_positionCloseToMouseWithConfirmation(1);
}

/*--------------------------------------------------------------------------
 * EdBlockXtndMode()
 */
int EdBlockXtndMode(long bOn)
{
	WINFO *	wp;

	if ((wp = ww_getCurrentEditorWindow()) == 0) {
		return 0;
	}

	wp->bXtndBlock = bOn ? TRUE : FALSE;

	return 1;
}

