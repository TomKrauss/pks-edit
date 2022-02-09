/*
 * CaretMovement.c
 *
 * PROJEKT: PKS-EDIT for Windows
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
#include "documentmodel.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"
#include "crossreferencelinks.h"

#include "caretmovement.h"
#include "linkedlist.h"
#include "textblocks.h"
#include "winfo.h"
#include "winterf.h"
#include "mouseutil.h"
#include "stringutil.h"
#include "xdialog.h"
#include "pksedit.h"
#include "codecompletion.h"
#include "markpositions.h"

extern long		_multiplier;

/*
 * Scroll the top left of the visible area by a given delta in x (columns) and y (lines)
 */
static void wi_scrollTop(WINFO* wp, long dy) {
	long dx = 0;
	if (sl_scrollwinrange(wp, &dy, &dx)) {
		sl_winchanged(wp, dy, dx);
	}
}

/*
 * Calculate an offset of character buffer positions to the corresponding number
 * of columns as displayed on the screen.
 */
int caret_bufferOffset2screen(WINFO* wp, const char* lbuf, int lnoffset) {
	register int  col = 0;
	register const char* p = lbuf;

	lbuf += lnoffset;
	while (p < lbuf) {
		if (*p++ == '\t')
			col = indent_calculateNextTabStop(col, &wp->indentation);
		else col++;
	}
	return col;
}

/*--------------------------------------------------------------------------
 * caret_lineOffset2screen()
 * the following stuff is calculating the cursor screen position, out of
 * the internal offset to the line buffer
 */
EXPORT int caret_lineOffset2screen(WINFO *wp, CARET *cp) {	
	LINE* lp = cp->linePointer;
	int lnoffset = cp->offset;

	if (wp == NULL) {
		wp = ww_getCurrentEditorWindow();
		if (!wp) {
			return 0;
		}
	}
	if (lnoffset > lp->len) {
		lnoffset = lp->len;
	}
	return caret_bufferOffset2screen(wp, lp->lbuf, lnoffset);
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
	while (i < col && p < pend) {
		if (*p++ == '\t')
			i = indent_calculateNextTabStop(i, &wp->indentation);
		else i++;
	}
	return (int)(p-lp->lbuf);
}


/*
 * Update the current selection in a window. It is assumed that the caret has moved with an "extend selection"
 * move (e.g. by mouse drag or by Shift+Arrow Key)
 */
void caret_updateSelection(WINFO* wp, CARET* c1, CARET* c2, BOOL bC1BeforeC2) {
	if ((c1->linePointer == c2->linePointer && c1->offset < c2->offset) ||
		(c1->linePointer != c2->linePointer && bC1BeforeC2)) {
		bl_syncSelectionWithCaret(wp, c1, MARK_START | MARK_NO_HIDE, NULL);
		bl_syncSelectionWithCaret(wp, c2, MARK_END | MARK_NO_HIDE, NULL);
	} else {
		bl_syncSelectionWithCaret(wp, c1, MARK_END | MARK_NO_HIDE, NULL);
		bl_syncSelectionWithCaret(wp, c2, MARK_START | MARK_NO_HIDE, NULL);
	}
}

/**
 * caret_isBeforeOther()
 * Check, whether one caret is located before another one. If bMarkEnd is true, the logic for determining the end is reversed.
 */
int caret_isBeforeOther(LINE* lpFirst, int col1, const LINE* fpcl, int col, int bMarkEnd) {
	LINE* mlm;

	mlm = lpFirst;
	if (fpcl == mlm) {
		return (col <= col1);
	}
	LINE* lpBack = mlm;
	LINE* lpForward = mlm;

	while (fpcl != lpBack && fpcl != lpForward && lpBack && lpForward) {
		lpBack = lpBack->prev;
		lpForward = lpForward->next;
	}
	return bMarkEnd ? (fpcl != lpForward) : fpcl == lpForward;
}

/*--------------------------------------------------------------------------
 * caret_extendSelection()
 */
static CARET _caretBeforeExtension;
void caret_extendSelection(WINFO *wp)
{
	CARET* pCurrent = &wp->caret;

	if (_caretBeforeExtension.linePointer && 
			(pCurrent->linePointer != _caretBeforeExtension.linePointer || pCurrent->offset != _caretBeforeExtension.offset)) {
		MARK* mpStart = wp->blstart;
		MARK* mpEnd = wp->blend;
		CARET caret1;
		CARET caret2;
		BOOL b1Before2 = TRUE;
		caret1.linePointer = NULL;
		if (mpStart && mpEnd && (mpStart->m_linePointer != mpEnd->m_linePointer || mpStart->m_column != mpEnd->m_column)) {
			if (mpStart->m_linePointer == _caretBeforeExtension.linePointer && mpStart->m_column == _caretBeforeExtension.offset) {
				if (caret_isBeforeOther(mpEnd->m_linePointer, 0, pCurrent->linePointer, 0, FALSE)) {
					caret1.offset = mpEnd->m_column;
					caret1.linePointer = mpEnd->m_linePointer;
					caret2 = *pCurrent;
				} else {
					caret2.offset = mpEnd->m_column;
					caret2.linePointer = mpEnd->m_linePointer;
					caret1 = *pCurrent;
				}
			}
			else if (mpEnd->m_linePointer == _caretBeforeExtension.linePointer && mpEnd->m_column >= _caretBeforeExtension.offset) {
				if (caret_isBeforeOther(pCurrent->linePointer, 0, mpStart->m_linePointer, 0, FALSE)) {
					caret1 = *pCurrent;
					caret2.linePointer = mpStart->m_linePointer;
					caret2.offset = mpStart->m_column;
				} else {
					caret1.offset = mpStart->m_column;
					caret1.linePointer = mpStart->m_linePointer;
					caret2 = *pCurrent;
				}
			}
		}
		if (caret1.linePointer == NULL) {
			caret1 = _caretBeforeExtension;
			caret2 = *pCurrent;
			b1Before2 = caret_isBeforeOther(caret1.linePointer, caret1.offset, caret2.linePointer, caret2.offset, FALSE);
		}
		caret_updateSelection(wp, &caret1, &caret2, b1Before2);
	}
	_caretBeforeExtension.linePointer = NULL;
}

/*--------------------------------------------------------------------------
 * caret_startExtendingSelection()
 * Start an operation where the selection is defined via cursor movement (e.g. when
 * the user presses shift+RIGHT).
 */
static void caret_startExtendingSelection(WINFO *wp) {
	if (!wp->bXtndBlock) {
		return;
	}
	_caretBeforeExtension = wp->caret;
}

/*--------------------------------------------------------------------------
 * caret_hideSelectionOnMove()
 */
static void caret_hideSelectionOnMove(WINFO *wp)
{
	long col;

	if (!wp->bXtndBlock &&
		(GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) &&
		ww_hasSelection(wp)) {
		col = wp->caret.virtualOffset;
		bl_hideSelection(wp, 1);
		wp->caret.virtualOffset = col;		/* restore virtual column position */
	}
}

/*
 * Calculate the "matching" line number in a file fp2 compared with file fp1.
 * nFlag1 is the flag marking lines to skip (e.g. lines inserted) in the source file nFlag2
 * the corresponding flag for lines to skip in the second file.
 */
static long caret_calculateSyncedLine(FTABLE* fp1, FTABLE* fp2, long ln, int nFlag1, int nFlag2) {
	LINE* lp = fp1->firstl;
	long lnLeft = 0;
	while (lp && ln > 0) {
		if (!(lp->lflg & nFlag1)) {
			lnLeft++;
		}
		lp = lp->next;
		ln--;
	}
	lp = fp2->firstl;
	ln = 0;
	while (lp && lnLeft > 0) {
		while (lp->lflg & nFlag2) {
			ln++;
			lp = lp->next;
			if (lp == NULL)  {
				return ln;
			}
		}
		ln++;
		lp = lp->next;
		lnLeft--;
	}
	while (lp && lp->lflg & nFlag2) {
		ln++;
		lp = lp->next;
	}
	return ln;
}

/*
 * Move the caret to the given line. 
 */
void caret_moveToLine(WINFO* wp, long ln) {
	if (wp->caret.ln == ln || ln < 0) {
		return;
	}
	caret_removeSecondaryCarets();
	codecomplete_hideSuggestionWindow(wp);
	long oldln = wp->caret.ln;
	wp->caret.ln = ln;
	if (wp->dispmode & SHOWCARET_LINE_HIGHLIGHT) {
		render_repaintWindowLine(wp, oldln);
		render_repaintWindowLine(wp, ln);
	}
	COMPARISON_LINK* pLink = wp->comparisonLink;
	if (pLink != NULL && pLink->cl_synchronizeCaret) {
		pLink->cl_synchronizeCaret = FALSE;
		int nRight = LN_COMPARE_ADDED;
		int nLeft = LN_COMPARE_DELETED;
		WINFO* wpOther = pLink->cl_wpLeft;
		if (wpOther == wp) {
			wpOther = pLink->cl_wpRight;
			nLeft = LN_COMPARE_ADDED;
			nRight = LN_COMPARE_DELETED;
		}
		// TODO: calculate matching line in other window.
		long ln1 = caret_calculateSyncedLine(wp->fp, wpOther->fp, ln, nLeft, nRight);
		long col = 0;
		wp->renderer->r_placeCaret(wpOther, &ln1, 0, &col, 0, 0);
		int nDelta = ln - wp->minln;
		int nNewMin = ln1 - nDelta;
		if (nNewMin < 0) {
			nNewMin = 0;
		}
		nNewMin -= wpOther->minln;
		if (nNewMin != 0 && wp == pLink->cl_wpLeft) {
			wi_scrollTop(wpOther, nNewMin);
		}
		pLink->cl_synchronizeCaret = TRUE;
	}
}

/*-------------------------------------
 * caret_updateDueToMouseClick()
 * Invoked, when the cursor is positioned using slider or mouse to update the
 * caret position. 
 */
EXPORT int caret_updateDueToMouseClick(WINFO *wp, long *ln, long *col, int updateVirtualColumn) {
	LINE *	lp;
	int		i;
	FTABLE* fp = wp->fp;

	if ((lp = ln_goto(fp, *ln)) == 0) {
		return 0;
	}

	i = *col;
	if (!updateVirtualColumn && i < wp->caret.virtualOffset) {
		i = wp->caret.virtualOffset;
	}
	i = caret_screen2lineOffset(wp, &(CARET) {.linePointer = lp, .offset = i});
	long maxcol = wp->renderer->r_calculateMaxColumn(wp, *ln, lp);
	if (maxcol < i) {
		i = maxcol;
	}

	*col = caret_lineOffset2screen(wp, &(CARET) {.linePointer = lp, .offset = i});

	caret_startExtendingSelection(wp);
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
 * Input parameters are a pointer to the line (in screen coordinates) to move to,
 * the line buffer offset, a pointer to the screen column. If 'updateVirtualOffset' is 1,
 * the virtual column on the screen should be updated. 'xDelta' is a hint defining, whether
 * the caret was moved horizontally to left or right or not.
 */
EXPORT int caret_placeCursorAndValidate(WINFO *wp, long *ln, long offset, long *col, int updateVirtualOffset, int xDelta) {
	LINE *		lp;
	int 		i;
	int			o;
	FTABLE* fp = wp->fp;

	if ((lp = ln_goto(fp,*ln)) == 0L)
		return 0;

	o = offset;
	if (o < 0) {
		*col = 0;
		o    = 0;
	}
	long maxcol = wp->renderer->r_calculateMaxColumn(wp, *ln, lp);
	if (o > maxcol) {
		o = maxcol;
	}

	i = caret_lineOffset2screen(wp, &(CARET) {.linePointer = lp, .offset = o});
	if (!updateVirtualOffset && i != wp->caret.virtualOffset && (wp->dispmode & SHOWCARET_PRESERVE_COLUMN))
		o = caret_screen2lineOffset(wp, &(CARET) {
		.linePointer = lp, .offset = wp->caret.virtualOffset
		});
	maxcol = wp->renderer->r_calculateMaxColumn(wp, *ln, lp);
	if (maxcol < o) o = maxcol;
	if (o != *col) i = caret_lineOffset2screen(wp, &(CARET) {.linePointer = lp, .offset = o});

	caret_startExtendingSelection(wp);
	if (updateVirtualOffset) {
		wp->caret.virtualOffset = i;
	}
	wp->caret.linePointer = lp;
	wp->caret.offset = o;
	caret_moveToLine(wp, *ln);
	*col = i;

	return 1;
}

/*--------------------------------------------------------------------------
 * caret_placeCursorForFile()
 * cursor absolut positioning for the given file.
 */
EXPORT int caret_placeCursorForFile(WINFO *wp, long ln, long col, long screenCol, int xDelta) {
	if (!wp->renderer->r_placeCaret(wp, &ln, col, &screenCol, 1, xDelta)) {
		return 0;
	}
	wt_curpos(wp, ln, screenCol);
	return 1;
}

/*
 * Adds a secondary caret to a window.
 */
CARET* caret_addSecondary(WINFO* wp, long ln, long nLineOffset) {
	LINE* lp = ln_goto(wp->fp, ln);
	if (!lp) {
		return 0;
	}
	CARET* pOld = wp->caret.next;
	while (pOld) {
		// do not add caret to existing secondary caret position.
		if (pOld->linePointer == lp && pOld->offset == nLineOffset) {
			return 0;
		}
		pOld = pOld->next;
	}
	CARET* pNew = (CARET*)ll_append((LINKED_LIST**) &wp->caret.next, sizeof(CARET));
	if (!pNew) {
		return 0;
	}
	pNew->offset = nLineOffset > lp->len ? lp->len : nLineOffset;
	pNew->ln = ln;
	pNew->linePointer = lp;
	render_repaintLineRangeWindow(wp, lp, lp);
	return pNew;
}

/*
 * Removes all secondary carets from the screen.
 */
int caret_removeSecondaryCarets() {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (wp == NULL) {
		return 0;
	}
	CARET* pCaret = wp->caret.next;
	while (pCaret) {
		LINE* lp = pCaret->linePointer;
		render_repaintLineRangeWindow(wp, lp, lp);
		pCaret = pCaret->next;
	}
	ll_destroy((LINKED_LIST**) & wp->caret.next, 0);
	return 1;
}

/*--------------------------------------------------------------------------
 * caret_placeCursorInCurrentFile()
 * cursor absolut positioning for the current file.
 */
EXPORT int caret_placeCursorInCurrentFile(WINFO* wp, long ln,long col) {
	long screenCol = col == wp->caret.offset ? wp->caret.col : col;
	return caret_placeCursorForFile(wp,ln,col,screenCol,0);
}

/*--------------------------------------------------------------------------
 * caret_saveLastPosition()
 * Store the last cursor position so one can navigate "back" for the current
 * editor window.
 */
int caret_saveLastPosition(void) {
	return fm_savepos(MTE_AUTO_LAST_SEARCH);
}

/*--------------------------------------------------------------------------
 * wi_adjust()
 * justify to middle of screen
 */
static int wi_adjust(WINFO *wp, long ln,int adjustflag)
{	long pos,dy;

	if (ln < wp->mincursln || ln > wp->maxcursln || (adjustflag & 2)) {
		if (adjustflag & 1) caret_saveLastPosition();
		if (wp->cursaftersearch == CP_POSTOP) pos = wp->mincursln;
		else if (wp->cursaftersearch == CP_POSLOW) pos = wp->maxcursln;
		else pos = ((wp->maxcursln+wp->mincursln)>>1);
		dy = ln-pos;
		wi_scrollTop(wp, dy);
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
	if (wp == NULL)
		return 0;
	if (ln == wp->caret.ln && col == wp->caret.offset)
		return 1;

	caret_saveLastPosition();
	return caret_placeCursorInCurrentFile(wp, ln, col);
}

/*--------------------------------------------------------------------------
 * caret_placeCursorAndMakevisibleWithSpace()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible. Leave some rows space at top / bottom of the screen.
 */
EXPORT int caret_placeCursorAndMakevisibleWithSpace(WINFO *wp, long ln,long col)
{
	wi_adjust(wp,ln,3);
	return caret_placeCursorForFile(wp,ln,col, col, 0);
}

/*--------------------------------------------------------------------------
 * caret_advancePage()
 * cursor advance one screen 
 */
EXPORT int caret_advancePage(WINFO* wp, long *ln,int dir)
{	register ds;

	if (wp == NULL) {
		return 0;
	}
	ds = wp->maxcursln-wp->mincursln;
	*ln = wp->caret.ln + (dir * ds * _multiplier);
	long nlines = wp->renderer->r_calculateMaxLine(wp);
	if (*ln < 0L) *ln = 0L;
	else if (*ln >= nlines) *ln = nlines-1L;
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
 * caret_countNumberOfTabs()
 * calculate nr of tabs in line
 * depending on shiftwidth
 */
static int caret_countNumberOfTabs(LINE *lp) {
	return caret_lineOffset2screen(NULL, &(CARET){.linePointer = lp, .offset = ln_countLeadingSpaces(lp) });
}

/*--------------------------------------------------------------------------
 * caret_findPreviousDifferentIndentation()
 */
static int caret_findPreviousDifferentIndentation(LINE *lp) 
{	 return (caret_countNumberOfTabs(lp) != caret_countNumberOfTabs(lp->prev)); }

/*--------------------------------------------------------------------------
 * caret_findNextDifferentIndentation()
 */
static int caret_findNextDifferentIndentation(LINE *lp)
{	return (!lp->next || (caret_countNumberOfTabs(lp->next) != caret_countNumberOfTabs(lp))); }

/*--------------------------------------------------------------------------
 * caret_gotoParagraphEnd()
 */
static int caret_gotoParagraphEnd(LINE *lp)
{	return (ln_lineIsEmpty(lp) && !ln_lineIsEmpty(lp->prev)); }

/*--------------------------------------------------------------------------
 * caret_gotoParagraphStart()
 */
static int caret_gotoParagraphStart(LINE *lp)
{	return (ln_lineIsEmpty(lp->prev) && !ln_lineIsEmpty(lp)); }

/*--------------------------------------------------------------------------
 * caret_advanceSectionUsing()
 * advance one section (depending on func)
 */
static void caret_advanceSectionUsing(WINFO* wp, long *ln,int dir,int start,
					int (*func1)(),int (*func2)())
					/* caret_gotoParagraphStart,caret_gotoParagraphEnd for advancing paragraphs */
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
	caret_advanceSectionUsing(wp, &ln,dir,start,caret_gotoParagraphStart,caret_gotoParagraphEnd);
	if (!start) ln--;
	return ln;
}

/*--------------------------------------------------------------------------
 * caret_advanceParagraphFromCurrentLine()
 */
EXPORT int caret_advanceParagraphFromCurrentLine(WINFO* wp, int dir,int start)
{	long ln;

	ln = caret_advanceParagraph(wp, wp->caret.ln,dir,start);
	return caret_placeCursorAndSavePosition(wp, ln,0L);
}

/*--------------------------------------------------------------------------
 * caret_advanceSection()
 * Advances the cursor by one "section".
 */
EXPORT int caret_advanceSection(WINFO* wp, int dir,int start)
{	long ln;

	caret_saveLastPosition();
	ln = wp->caret.ln;
	caret_advanceSectionUsing(wp, &ln,dir,start,caret_findPreviousDifferentIndentation,caret_findNextDifferentIndentation);
	caret_placeCursorInCurrentFile(wp, ln,0L);

	return caret_placeCursorInCurrentFile(wp, ln,(long)ln_countLeadingSpaces(wp->caret.linePointer));
}

/*
 * Find the caret with the highest / lowest line number. If dir < 0 find the one with the lowest line number (most top to the beginning of the file),
 * otherwise the one most down.
 */
static CARET* caret_findMostUpDown(CARET* pCaret, int dir) {
	int nMax = -1;
	CARET* pFound = pCaret;

	while (pCaret) {
		if (nMax < 0 || (dir > 0 && pCaret->ln > nMax) || (dir < 0 && pCaret->ln < nMax)) {
			nMax = pCaret->ln;
			pFound = pCaret;
		}
		pCaret = pCaret->next;
	}
	return pFound;
}

/*
 * Move the caret to add a secondary caret.
 */
int caret_moveAndAddSecondary(MOT_SECONDARY_MOVEMENT mType) {
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp == NULL) {
		return 0;
	}
	if (mType == MSM_WORDS) {
		char buf[256];

		if (!xref_getSelectedIdentifier(buf, sizeof buf)) {
			return 0;
		}
		long nLine = wp->minln;
		LINE* lp = ww_getMinLine(wp, nLine);
		int nDelta = 0;
		char* pMatch = wp->caret.linePointer->lbuf;
		while ((pMatch = strstr(pMatch, buf)) != NULL) {
			int nOffset = (int)(pMatch - wp->caret.linePointer->lbuf);
			if (nOffset <= wp->caret.offset && (nOffset + strlen(buf)) >= wp->caret.offset) {
				nDelta = wp->caret.offset - nOffset;
				break;
			}
			pMatch++;
		}
		while (lp && nLine <= wp->maxln) {
			pMatch = lp->lbuf;
			while ((pMatch = strstr(pMatch, buf)) != NULL) {
				int nOffset = (int)(pMatch - lp->lbuf) + nDelta;
				if (lp != wp->caret.linePointer || nOffset != wp->caret.offset) {
					caret_addSecondary(wp, nLine, nOffset);
				}
				pMatch++;
			}
			lp = lp->next;
			nLine++;
		}
		return 1;
	}
	int nDir = mType == MSM_DOWN ? 1 : -1;
	CARET* pCaret = caret_findMostUpDown(&wp->caret, nDir);
	if ((pCaret = caret_addSecondary(wp, pCaret->ln + nDir, pCaret->offset)) != NULL) {
		render_makeCaretVisible(wp, pCaret);
		return 1;
	}
	return 0;

}

/*--------------------------------------------------------------------------
 * caret_moveUpOrDown()
 * general cursor advancing in
 * vertical direction
 */
EXPORT int caret_moveUpOrDown(WINFO* wp, int dir, int mtype)
{
	BOOL	bXtnd;
	int		nRet;
	long 	col;
	long 	ln;

	bXtnd = wp->bXtndBlock;
	nRet = 0;
	if (mtype & MOT_XTNDBLOCK) {
		wp->bXtndBlock = TRUE;
	}
	caret_hideSelectionOnMove(wp);

	col = wp->caret.offset;
	ln = wp->caret.ln;
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
		case MOT_FILE: {
			long nlines = wp->renderer->r_calculateMaxLine(wp);
			if (!caret_placeCursorAndSavePosition(wp, (dir > 0) ? nlines - 1L : 0L, 0L)) {
				goto err;
			}
			nRet = 1;
			goto err;
		}
	}

	if (wp->renderer->r_placeCaret(wp,&ln,col,&col,0,0)) {
		nRet = 1;
		wt_curpos(wp,ln,col);
	}
err:
	wp->bXtndBlock = bXtnd;
	return nRet;
}

/*--------------------------------------------------------------------------
 * caret_gotoIdentifierEnd()
 * cursor advance one word
 */
static LINE *caret_nextWord(LINE *lp,long *ln,long *col,
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
			if (!lp) {
				return 0;
			}
			c  = lp->len;
		} else {
			BOOL bNavigated = FALSE;
			p--;
			if (bNo) {
				while (c && !(*iswfunc)(*p)) { p-- , c--; }
			} else if (c > 0 && !(*iswfunc)(*p)) {
				while (c && !(*iswfunc)(*p)) { p--, c--; }
				bNavigated = TRUE;
			}
			if (!bNavigated) {
				while (c && (*iswfunc)(*p)) { p--, c--; }
			}
		}
	} else {
		if (c == len) {
			WINFO* wp = ww_getCurrentEditorWindow();
			long nlines = wp->renderer->r_calculateMaxLine(wp);
			if (++l >= nlines) return 0;
			lp = lp->next;
			c  = 0;
		} else {
			if (!bNo && !(*iswfunc)(*p) && c < len) { 
				while (!(*iswfunc)(*p) && c < len) { p++, c++; }
			} else {
				while ((*iswfunc)(*p) && c < len) { p++, c++; }
				if (bNo) {
					while (!(*iswfunc)(*p) && c < len) { p++, c++; }
				}
			}
		}
	}
	*ln = l, *col = c;
	return lp;
}

/*--------------------------------------------------------------------------
 * caret_advanceWordOnly()
 */
EXPORT LINE *caret_advanceWordOnly(LINE *lp,long *ln,long *col,int dir)
{
	return caret_nextWord(lp,ln,col,char_isNospace,dir,0);
}

/*--------------------------------------------------------------------------
 * caret_gotoIdentifierSkipSpace()
 */
EXPORT LINE *caret_gotoIdentifierSkipSpace(LINE *lp,long *ln,long *col,int dir)
{
	return caret_nextWord(lp,ln,col, char_isIdentifier,dir,1);
}

/*--------------------------------------------------------------------------
 * caret_gotoIdentifierEnd()
 */
EXPORT LINE *caret_gotoIdentifierEnd(LINE *lp,long *ln,long *col,int dir)
{
	return caret_nextWord(lp,ln,col,char_isIdentifier,dir,0);
}

/*--------------------------------------------------------------------------
 * caret_advanceCharacter()
 * cursor advance to char
 */
static LINE *caret_advanceCharacter(LINE *lp,long *ln,long *col,int dir,unsigned char match)
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
 * Go back one screen column in a line and return the new column considering internal 
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
	advmatchfunc = caret_gotoIdentifierSkipSpace;
	switch (abs(mtype)) {
		case MOT_UNTILC:
			*c = EdPromptForCharacter(ids_name);
			advmatchfunc = caret_advanceCharacter;
			break;
		case MOT_SPACE:
			advmatchfunc = caret_gotoIdentifierEnd;
			break;
	}
}

/*--------------------------------------------------------------------------
 * caret_moveLeftRight()
 * Move the caret to the left or right. If motionFlags contains MOT_XTNDBLOCK
 * the selection is extended.
 */
EXPORT int caret_moveLeftRight(WINFO* wp, int direction, int motionFlags) {
	FTABLE *fp;
	long  	ln;
	long 	col;
	long    screencol;
	long maxcol;
	LINE *	lp;
	int   	matchc;
	int		moving;
	int		nRet;
	BOOL		bXtnd;

	fp = wp->fp;
	col = wp->caret.offset;
	screencol = wp->caret.col;
	lp = wp->caret.linePointer;
	ln = wp->caret.ln;
	bXtnd = wp->bXtndBlock;
	nRet = 0;
	if (motionFlags & MOT_XTNDBLOCK) {
		wp->bXtndBlock = TRUE;
	}

	caret_hideSelectionOnMove(wp);

	moving = direction * (motionFlags & (~MOT_XTNDBLOCK));

	switch(moving) {
		case MOT_WORD:    case -MOT_WORD:
		case MOT_UNTILC:  case -MOT_UNTILC:
		case MOT_SPACE:   case -MOT_SPACE:
			caret_setMatchFunction(moving, IDS_CURSUNTILC, &matchc);
			if ((lp = (*advmatchfunc)(lp, &ln, &col,
				(moving > 0) ? 1 : -1, matchc)) == 0) {
				goto err;
			}
			break;
		case  MOT_SINGLE:
			maxcol = wp->renderer->r_calculateMaxColumn(wp, ln, lp);
			if (col >= maxcol) {
				lp = lp->next;
				if (lp->next == 0) {
					goto err;
				}
				ln++;
				col = 0;
				screencol = 0;
			} else {
				col++;
				screencol++;
			}
			break;
		case  MOT_TOEND:
			col = wp->renderer->r_calculateMaxColumn(wp, ln, lp);
			break;
		case -MOT_SINGLE:
			if (!screencol) {
				if ((lp = lp->prev) == 0)
					goto err;
				maxcol = wp->renderer->r_calculateMaxColumn(wp, ln, lp);
				ln--;
				col = maxcol;
				screencol = maxcol;
			} else {
				col = caret_getPreviousColumnInLine(wp, lp,col);
				screencol--;
			}
			break;
		case -MOT_TOEND:
			col = 0L;
			screencol = 0;
			break;
	}
	nRet = caret_placeCursorForFile(wp, ln, col, screencol, moving);
	// The following code really makes sense only, if we are not considering the complete identifier under the cursor - should be possibly made configurable
	// codecomplete_updateCompletionList(wp, FALSE);

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
{	
	*column = (x + wp->cwidth/2) / wp->cwidth + wp->mincol;
	*line	= y / wp->cheight + wp->minln;
	if (*line < 0) {
		*line = 0;
	} else {
		long nlines = wp->renderer->r_calculateMaxLine(wp);
		if (*line >= nlines) {
			*line = nlines - 1;
		}
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

/*------------------------------------------------------------
 * EdMouseMoveText()
 * Synonym for mouse position.
 */
EXPORT int EdMouseMoveText(int move) {
	return EdMousePositionUngrabbed(TRUE);
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

