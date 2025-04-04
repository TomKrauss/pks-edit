/*
 * CutAndPaste.c
 *
 * cut & paste, mark blocks
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 01.02.1991
 */

#include <windows.h>
#include "caretmovement.h"
#include "linkedlist.h"
#include "textblocks.h"
#include "winfo.h"
#include "pksedit.h"
#include "edierror.h"
#include "iccall.h"
#include "editorconfiguration.h"
#include "errordialogs.h"
#include "publicapi.h"
#include "clipboard.h"
#include "fileselector.h"
#include "markpositions.h"
#include "funcdef.h"

 /*-----------------------*/
/* EXTERNALS			*/
/*-----------------------*/
extern LINE	*find_expandTabsInFormattedLines(WINFO *wp, LINE *lp);

extern long	_multiplier;

/*
 * Returns true, if the given ID represents the ID of the default (system) clipboard. 
 */
BOOL bl_isDefaultClipboard(const char* pszId) {
	return pszId == NULL || *pszId == 0;
}

/*----------------------------
 * bl_hideSelection() 		
 * Hide the current selection and optionally
 * remove the selection marker from "marked lines"
 *----------------------------*/
EXPORT int bl_hideSelection(WINFO* wp, int removeLineSelectionFlag) {
	FTABLE *fp;
	MARK*	mps;
	MARK *	mpe;

	fp = wp->fp;
	mps = wp->blstart;
	mpe = wp->blend;

	if (mps && mpe) {
		render_repaintLineRangeWindow(wp, mps->m_linePointer, mpe->m_linePointer);
		if (removeLineSelectionFlag) {
			ln_removeFlag(mps->m_linePointer, mpe->m_linePointer, (LNXMARKED | LN_COMPARE_DIFFERENT));
			if (mps->m_linePointer == mpe->m_linePointer) {
				if (mps->m_column != mpe->m_column) {
					render_repaintLine(fp, mps->m_linePointer);
				}
			}
		}
	} else {
		if (removeLineSelectionFlag) {
			LINE* lp = mps ? mps->m_linePointer : (mpe ? mpe->m_linePointer : wp->caret.linePointer);
			ln_removeFlag(lp, lp, (LNXMARKED|LN_COMPARE_DIFFERENT));
			render_repaintLine(fp, lp);
		}
	}
	mark_killSelection(wp);
	return 1;
}

/*---------------------------------*/
/* bl_pasteBlock()					*/
/*---------------------------------*/
EXPORT int bl_pasteBlock(WINFO* wp, PASTE *buf, int colflg, int offset, int move) {
	LINE *	lp;
	long 	col,ln;
	long 	delta,oln;
	int	    	ret;
	FTABLE *	fp;

	fp = wp->fp;
	if ((ret = bl_paste(buf,wp,wp->caret.linePointer, offset,colflg)) == 0 ||
		colflg ||
		(wp->workmode & WM_LINE_SELECTION)) {
		if (ret == 0 || !move) {
			render_repaintAllForFile(fp);
		}
		return ret;
	}

	delta = buf->nlines-1L;
	ln = buf->nlines+wp->caret.ln-1L;
	lp = buf->pln;
	while(lp->next) lp=lp->next;
	col = lp->len;
	oln = wp->caret.ln;
	if (buf->nlines == 1) 
		col += offset;
	if (!move) {
		if (delta > 0 && (oln+delta) <= wp->maxln) {
			caret_placeCursorInCurrentFile(wp, wp->caret.ln+1L,0L);
			wt_insline(fp, wp->caret.ln, (int ) delta);
		}
		if (ln > wp->maxln) delta = wp->maxln;
		else delta = ln;
		render_repaintFromLineTo(fp,oln,delta);
		caret_placeCursorInCurrentFile(wp, ln,col);		
	} else {
		wp->blstart->m_column++; /* get block marked afterwards (s. cpy_mv) */
		caret_placeCursorInCurrentFile(wp, ln,col);
		render_repaintAllForFile(fp);
		EdSyncSelectionWithCaret(wp, MARK_END);
	}
	return ret;
}

/*-----------------------*/
/* paste()			*/
/*-----------------------*/
EXPORT int paste(WINFO* wp, PASTE *buf,int move) {
	return bl_pasteBlock(wp, buf,ww_isColumnSelectionMode(wp), wp->caret.offset, move);
}

/*--------------------------------------------------------------------------
 * bl_getTextForClipboardNamed()
 */
char *bl_getTextForClipboardNamed(PASTE* pp, BOOL bDefaultClipboard) {
	if (bDefaultClipboard && !clp_ismine()) {
		clp_getdata();
	}
	char *pszRet = bl_convertPasteBufferToText(_linebuf, _linebuf + 512, pp);
	if (strlen(pszRet) >= 255) {
		strcat(pszRet, "...");
	}
	return pszRet;
}


/*----------------------------*/
/* bl_getNamedBuffer() 			*/
/*----------------------------*/
static char* bl_getNamedBuffer(BOOL bNewBuffer) {
	return bl_showClipboardList(bNewBuffer ? SNCO_CREATE : SNCO_SELECT);
}

/*---------------------------------*/
/* bl_getPasteBuffer()				*/
/*---------------------------------*/
EXPORT PASTE *bl_getPasteBuffer(int which) {
	PASTE *	pp;
	char* 	pszId;
	int		err=0;

	if (which & PASTE_QUERYID) {
		if ((pszId = bl_getNamedBuffer(FALSE)) == 0) {
			return 0;
		}
	} else {
		pszId = 0;
	}

	/* try to get data from windows clipboard */
	if (bl_isDefaultClipboard(pszId)) {
		clp_getdata();
	}
	pp = bl_addrbyid(pszId,0,PLT_NAMED_BUFFER);
	err = IDS_MSGNOCLIP;

	if (!pp || !pp->pln) {
		error_showErrorById(err);
		return 0;
	}

	return pp;
}

/*------------------------------------------------------------
 * EdBlockPaste()
 */
EXPORT int EdBlockPaste(WINFO* wp, int which)
{	PASTE *	pp;

	if ((pp = bl_getPasteBuffer(which)) != 0) {
		if ((GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) && ww_hasSelection(wp)) {
			EdBlockDelete(wp, 0);
		}
		return paste(wp, pp,0);
	}

	return 0;
}

/*----------------------------
 * bl_insertPasteBufFromFile()
 * Read a text block from a file with the given name
 * and insert it into the current document.
 *----------------------------*/
EXPORT int bl_insertPasteBufFromFile(WINFO* wp, char *fn, long codePage)
{	PASTE	pbuf;
	char 	fname[256];
	int		ret;

	if (!wp) 
		return 0;
	if (fn == 0) {
		FILE_SELECT_PARAMS pFSP;
		memset(&pFSP, 0, sizeof pFSP);
		pFSP.fsp_inputFile = fn;
		pFSP.fsp_optionsAvailable = FALSE;
		pFSP.fsp_saveAs = FALSE;
		if (!fsel_selectFileWithTitle(CMD_OPEN_FILE,fname, &pFSP))
			return 0;
		fn = fname;
	}
	FILE_READ_OPTIONS fro = {
		.fro_fileName = fn,
		.fro_useDefaultDocDescriptor = 0,
		.fro_codePage = codePage
	};
	if ((ret = bl_readFileIntoPasteBuf(&pbuf, &fro)) != 0) {
		paste(wp, &pbuf, 0);
	}
	ln_listfree(pbuf.pln);
	return ret;
}

/*
 * PKS Edit command to read a block from a default file name 
 */
EXPORT int EdBlockRead(WINFO* wp) { 
	return bl_insertPasteBufFromFile(wp, (char *)0, -1);	
}

/*
 * PKS Edit command to write a block to a default file name. 
 */
EXPORT int EdBlockWrite(WINFO* wp){ 	
	return bl_writeToFile(wp, (char *)0);	
}

/*---------------------------------*/
/* bl_cutOrCopyBlock()					*/
/*---------------------------------*/
int bl_cutOrCopyBlock(WINFO*wp, MARK *ms, MARK *me, int flg, PASTE *pp) {
	PASTE 	_p;
	PASTE_LIST_TYPE tType = PLT_CLIPBOARD;
	char*	 pszId;
	int	 	colflg = ww_isColumnSelectionMode(wp);

	memset(&_p, 0, sizeof _p);
	if (flg & CUT_QUERYID) {
		if ((pszId = bl_getNamedBuffer(TRUE)) == 0) {
			return 0;
		}
		tType = PLT_NAMED_BUFFER;
	} else {
		pszId = 0;
	}

	if (!(flg & CUT_USEPP)) {
		if ((pp = bl_addrbyid(pszId, (flg & CUT_APPND) ? 0 : 1, tType)) == 0) {
			return 0;
		}
	} else {
		pp->pln = 0;
	}

	if (bl_cut(wp, &_p,ms->m_linePointer,me->m_linePointer,ms->m_column,me->m_column,0,colflg)) {

		if (!(flg & CUT_APPND))
			bl_free(pp);

		if (bl_isDefaultClipboard(pszId))
			clp_setmine();

		bl_join(pp,&_p);		

		if (!(flg & CUT_USEPP)) {
			error_showMessageInStatusbar(IDS_MSGCUTBLOCK,pp->size,pp->nlines);
		}
		return 1;
	}

	return 0;
}

/*----------------------------*/
/* bl_cutOrCopy()				*/
/*----------------------------*/
EXPORT int bl_cutOrCopy(WINFO* wp, int flg,PASTE *pp)
{
	if (!ww_checkSelectionWithError(wp))
		return 0;
	int ret = bl_cutOrCopyBlock(wp, wp->blstart,wp->blend,flg,pp);
	if (ret && (flg & CUT_DELETE)) {
		EdBlockDelete(wp, 0);
	}
	return ret;
}

/*---------------------------------*
 * bl_writeToFile()
 * PKS Edit command to write a block to a file with the given name. 
 *---------------------------------*/
EXPORT int bl_writeToFile(WINFO* wp, char *fn)
{	PASTE  pbuf;
	char   fname[EDMAXPATHLEN];
	int	  ret;
	int   mode;

	if (bl_cutOrCopy(wp, CUT_USEPP,&pbuf)) {
		if (fn != 0) {
			ret = TRUE;
			mode = F_NORMOPEN;
		} else {
			mode = F_NEWFILE;
			FILE_SELECT_PARAMS fsp = {
				.fsp_optionsAvailable = FALSE,
				.fsp_saveAs = TRUE
			};
			ret = fsel_selectFileWithTitle(CMD_SAVE_FILE_AS,fname,&fsp);
			fn  = fname;
		}
		if (ret != 0) {
			ret = bl_writePasteBufToFile(&pbuf, fn, mode);
		}
		ln_listfree(pbuf.pln);
		return ret;
	}
	return 0;
}

/*---------------------------------
 * bl_resetCurrentLine()
 * reset the caret, so it is displayed in the "correct line".
 *---------------------------------*/
static void bl_resetCurrentLine(WINFO* wp) {
	LINE *lp;
	long ln = -1L;

	lp = wp->caret.linePointer;
	while(lp) {		    // make the whole thing faster: test (lp) ! 
		ln++, lp = lp->prev;
	}
	caret_moveToLine(wp, ln);
}

static BOOL blk_isBadMove(LINE* pSelectionStart, int nStartOffset, LINE* pSelectionEnd, int nEndOffset, CARET* pDestination) {
	if (pSelectionStart == pDestination->linePointer) {
		return pDestination->offset >= nStartOffset && (pSelectionEnd != pDestination->linePointer || pDestination->offset <= nEndOffset);
	}
	if (pSelectionEnd == pDestination->linePointer) {
		return pDestination->offset <= nEndOffset;
	}
	if (pSelectionStart == pSelectionEnd) {
		return FALSE;
	}
	for (LINE* lp = pSelectionStart->next; lp && lp != pSelectionEnd; lp = lp->next) {
		if (lp == pDestination->linePointer) {
			return TRUE;
		}
	}
	return FALSE;
}

/*------------------------------
 * EdBlockCopyOrMove()
 * PKS Edit command to copy or move a block.
 *----------------------------*/
EXPORT int EdBlockCopyOrMove(WINFO* wp, BOOL move) {	
	LINE   *ls,*le;
	int	  cs,ce,offs;
	long   delta,dln;
	FTABLE *fp;
	MARK   *bstart,*bend;
	int	  move_nocolblk,ret,colflg;

	fp = wp->fp;
	colflg = ww_isColumnSelectionMode(wp);

	if (!ww_checkSelectionWithError(wp))
		return 0;

	/*
	 * due to positioning problems afterwards: expand destination line
	 */
	if (colflg) {
		offs = wp->caret.col;
		if (find_expandTabsInFormattedLines(wp,wp->caret.linePointer) == 0)
			return 0;
	} else {
		offs = wp->caret.offset;
	}

	delta = 0;
	dln   = 0;
	ret   = 0;

	move_nocolblk = (move && !colflg);
	bstart = wp->blstart;
	bend   = wp->blend;

	ls = bstart->m_linePointer, cs = bstart->m_column;
	le = bend->m_linePointer,   ce = bend->m_column;
	if (move_nocolblk) {			/* valid move ??	*/
		if (blk_isBadMove(ls, cs, le, ce, &wp->caret)) {
			error_showErrorById(IDS_MSGBADBLOCKMOVE);
			return 0;
		}
		if (wp->caret.linePointer == ls && wp->caret.col > cs) {
			if (wp->caret.linePointer == le && wp->caret.col > ce) {
				delta = cs - ce;
			} else {
				delta = wp->caret.col - cs;
			}
		} else if (wp->caret.linePointer == le && wp->caret.col > ce) {
			delta = -ce;
		}
		long nLinesCopied = ll_indexOf((LINKED_LIST*)ls, (LINKED_LIST*)le);
		if (nLinesCopied > 1) {
			long nCaretLineOffset = ll_indexOf((LINKED_LIST*)ls, (LINKED_LIST*)wp->caret.linePointer);
			if (nLinesCopied < nCaretLineOffset) {
				dln = -nLinesCopied;
			}
		}
	}

	PASTE  pbuf = {
		.pln = 0
	};
	if ((ret = bl_cut(wp, &pbuf,ls,le,cs,ce,move,colflg)) != 0) {
		if (move_nocolblk) {
			caret_placeCursorInCurrentFile(wp, (long)(wp->caret.ln+dln),(long)(offs + delta));
			bl_hideSelection(wp, 0);
			EdSyncSelectionWithCaret(wp, MARK_START);
			wp->blstart->m_column--;
			bl_resetCurrentLine(wp);
		}
		if (colflg) {
			delta = wp->blcol2 - wp->blcol1;
			dln	 = ln_cnt(bstart->m_linePointer,bend->m_linePointer) - 1;
			bl_hideSelection(wp, 0);
		} else {
			offs = wp->caret.offset;
		}

		ret = bl_pasteBlock(wp, &pbuf,colflg,offs,move);
		if (colflg) {
			caret_placeCursorInCurrentFile(wp, wp->caret.ln,(long)offs);
			wp->blcol1 = offs;
			wp->blcol2 = offs + delta;
			bl_setSelection(wp,wp->caret.linePointer,offs,
				ln_relative(wp->caret.linePointer,dln),wp->blcol2);
		}
		render_repaintAllForFile(fp);
	}
	bl_free(&pbuf);

	return ret;
}

/*
 * Move the current selection one line up / down depending on 'delta'.
 * If no selection exists, the current line is selected and moved.
 */
int bl_moveSelectionUpDown(long delta) {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (!wp) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	if (!ww_hasSelection(wp)) {
		bl_setSelection(wp, wp->caret.linePointer, 0, wp->caret.linePointer->next, 0);
	} 
	long pos;
	long pos2;
	ww_getSelectionLines(wp, &pos, &pos2, FALSE);
	long newPos;
	newPos = pos + delta;
	if (newPos < 0 || newPos >= fp->nlines) {
		return 0;
	}
	caret_placeCursorInCurrentFile(wp, newPos, 0);
	int ret = EdBlockCopyOrMove(wp, TRUE);
	caret_placeCursorInCurrentFile(wp, newPos, 0);
	return ret;
}

/*---------------------------------*/
/* bl_placeCursorOnBlockMark()					*/
/*---------------------------------*/
static int bl_placeCursorOnBlockMark(WINFO* wp, MARK *mp) {	
	long newln;

	if (mp != 0) {
		newln = ln_indexOfWP(wp, mp->m_linePointer);
		caret_placeCursorAndSavePosition(wp, newln,(long)mp->m_column);
		return 1;
	} else {
		error_showErrorById(IDS_MSGNOBLOCKSELECTED);
		return 0;
	}
}

/*---------------------------------*/
/* EdBlockFindEnd() 				*/
/*---------------------------------*/
EXPORT void EdBlockFindEnd(WINFO* wp) {
	if (wp) bl_placeCursorOnBlockMark(wp, wp->blend);
}

/*---------------------------------*/
/* EdBlockFindStart()				*/
/*---------------------------------*/
EXPORT int EdBlockFindStart(WINFO* wp) {
	if (wp) return bl_placeCursorOnBlockMark(wp, wp->blstart);
	return 0;
}

/*----------------------------
 * EdBlockDelete()
 * PKS Edit macro command to delete the current selection and
 * optionally save the text in the trashcan clipboard of PKS Edit
 *----------------------------*/
EXPORT int EdBlockDelete(WINFO* wp, int bSaveOnClip) {
	MARK		ms;
	MARK		me;
	int 		ret;
	
	if (!ww_checkSelectionWithError(wp))
		return 0;
	ms = *wp->blstart;
	me = *wp->blend;
	EdBlockFindStart(wp);
	bl_hideSelection(wp, 0);
	ret = bl_delete(wp, ms.m_linePointer, me.m_linePointer, ms.m_column, me.m_column, 1, bSaveOnClip);
	render_repaintAllForFile(wp->fp);
	return ret;
}

/*---------------------------------*/
/* bl_cutLines() 				*/
/*---------------------------------*/
EXPORT int bl_cutLines(WINFO* wp)
{
	LINE *ls,*le;
	long n = _multiplier;
	PASTE *bp = bl_addrbyid(0,0, PLT_CLIPBOARD);

	if (bp == 0 || wp == 0L) return 0;
	ls = wp->caret.linePointer;
	if ((le = ln_relative(wp->caret.linePointer,n)) == 0) return 0;
	
	if (bl_cut(wp, bp,ls,le,0,(P_EQ(ls,le)) ? ls->len : 0, 0, 0)) {
		clp_setmine();
		return 1;
	}
	return 0;
}

/*----------------------------*/
/* bl_updateSelectionInLines() 			*/
/*----------------------------*/
static void bl_updateSelectionInLines(WINFO* wp, LINE *lpOldSelectionStart, LINE *lpOldSelectionEnd) {
	LINE *		lpFrom;
	LINE *		lpTo;

	lpFrom = wp->blstart->m_linePointer;
	if (lpOldSelectionStart == lpFrom) {
		lpFrom = lpOldSelectionEnd;
	}
	lpTo = wp->blend->m_linePointer;
	if (lpOldSelectionEnd == lpTo) {
		lpTo = lpOldSelectionStart;
	}
	render_repaintLineRangeWindow(wp, lpFrom, lpTo);

}

/*---------------------------------*/
/* bl_setColumnSelection()				*/
/*---------------------------------*/
static void bl_setColumnSelectionNoRepaint(WINFO *wp) {
	MARK	 *mp;

	if ((mp = wp->blstart) != 0L) {
		wp->blcol1 = caret_lineOffset2screen(wp, &(CARET) {.linePointer = mp->m_linePointer, .offset = mp->m_column });
	}
	if ((mp = wp->blend) != 0L) {
		wp->blcol2 = caret_lineOffset2screen(wp, &(CARET) {
			.linePointer = mp->m_linePointer, .offset = mp->m_column
		});
		if (wp->blcol2 <= wp->blcol1)
			wp->blcol2 = wp->blcol1	+ 1;
	}
}

/*------------------------------------------------------------
 * bl_setColumnSelection()
 */
EXPORT int bl_setColumnSelection(WINFO *wp)
{
	bl_setColumnSelectionNoRepaint(wp);
	render_repaintAllForFile(wp->fp);
	return 1;
}

/*---------------------------------
 * bl_setSelection()
 * Set the current selection on a given file.
 *---------------------------------*/
EXPORT int bl_setSelection(WINFO *wp, LINE *lps, int cs, LINE *lpe, int ce)
{
	if (lps) 
		bl_setBlockMark(wp, mark_set(wp,lps,cs,MARKSTART), TRUE);
	if (lpe) 
		bl_setBlockMark(wp, mark_set(wp,lpe,ce,MARKEND), FALSE);
	if (lps && lpe) {
		return 1;
	}
	return 0;
}

/*---------------------------------
 * bl_syncSelectionWithCaret()
 * Set the selection end to the passed caret. Use one of the MARK_XXXX flags
 * to modify the request to e.g. change to block selection type etc...
 * If pMarkSet != NULL, returns either "MARK_START" or "MARK_END" in pMarkSet depending
 * on whether the start or the end mark had been set.
 *---------------------------------*/
int bl_syncSelectionWithCaret(WINFO *wp, CARET *lpCaret, int flags, int *pMarkSet)
{
	MARK *		marks;
	MARK *		marke;
	MARK *		mark;
	LINE *		lp1 = 0;
	LINE *		lp2 = 0;
	int  		startx=0,endx=10000,markc,colflg;
	int  		type;
	int			bSwap;
	int			workmode;
	int			nMarkOffset = lpCaret->offset;
	LINE*		lpMark = lpCaret->linePointer;
	FTABLE*		fp = wp->fp;

	type = (flags & (~(MARK_COLUMN|MARK_RECALCULATE|MARK_NO_HIDE)));
	workmode = wp->workmode;

	if (type == MARK_ALL) {
		lp2 = fp->lastl->prev;
		bl_setSelection(wp,fp->firstl,0,lp2,lp2->len);
		wp->blcol1 = 0;
		wp->blcol2 = MAXLINELEN;
		render_repaintAllForFile(fp);
		return 1;
	}

	colflg = ww_isColumnSelectionMode(wp);

	if (wp->blstart) {
		lp1	  = wp->blstart->m_linePointer;
		startx = wp->blstart->m_column;
	}
	if (wp->blend) {
		lp2	= wp->blend  ->m_linePointer;
		endx = wp->blend->m_column;
	}

	if (type == MARK_END) {
		markc = MARKEND;
		marks = wp->blstart;
	} else {
		markc = MARKSTART;
		marks = wp->blend;
	}

	bSwap = 0;
	if (workmode & WM_LINE_SELECTION) {
		nMarkOffset = 0;
		if (type == MARK_END) {
			if (lpMark->next == fp->lastl) {
				nMarkOffset = lpMark->len;
			} else {
				lpMark = lpMark->next;
			}
		}
	}

	if (marks && 
		caret_isBeforeOther(marks->m_linePointer, marks->m_column, lpMark, nMarkOffset, type == MARK_END)) {
		if (flags & MARK_RECALCULATE) {
			bSwap = 1;
		} else if (!(flags & MARK_NO_HIDE)) {
			bl_hideSelectionInCurrentWindow(wp);
		}
	}

	mark_set(wp, lpMark, nMarkOffset, markc);
	marks = mark_find(wp, MARKSTART);
	marke = mark_find(wp, MARKEND);

	if (bSwap) {
		if (marks) {
			marks->m_identifier = MARKEND;
		}
		if (marke) {
			marke->m_identifier = MARKSTART;
		}
		
		mark = marks;
		marks = marke;
		marke = mark;
		markc = (markc == MARKSTART) ? MARKEND : MARKSTART;
		type = markc;
	}

	if (pMarkSet != NULL) {
		*pMarkSet = type;
	}
	bl_setBlockMark(wp, marks, TRUE);
	bl_setBlockMark(wp, marke, FALSE);

	if (colflg && !(flags & MARK_COLUMN)) {
		bl_setColumnSelectionNoRepaint(wp);
	}

	if (marks && marke) {		/* mark in a line */
		if ( colflg == 0	  && 
			P_EQ(lp1,marks->m_linePointer) && 
			P_EQ(lp2,marke->m_linePointer)) {
			if (marke->m_column != endx) {
				if (marke->m_column < endx)
					startx = marke->m_column;
				else {
					startx = endx;
					endx   = marke->m_column;
				}
			} else {
				if (marks->m_column < startx) {
					endx   = startx;
					startx = marks->m_column;
				} else {
					endx = marks->m_column;
				}
			}
			render_repaintLinePart(fp, wp->caret.ln, caret_lineOffset2screen(wp, &(CARET) {
				.linePointer = wp->caret.linePointer, .offset = startx
			}),
				caret_lineOffset2screen(wp, &(CARET) {
					.linePointer = wp->caret.linePointer, .offset = endx
				}));
		}
		else {
			bl_updateSelectionInLines(wp, lp1, lp2);
		}
	}

	return 1;
}

/*---------------------------------*/
/* EdMouseMarkParts()				*/
/*---------------------------------*/
EXPORT int EdMouseMarkParts(WINFO* wp, int type)
{
	long   	ln,col;
	LINE *	lp;
	LINE *	lp2;
	long   	o1;
	long 	o2;
	FTABLE *	fp;
	int    	colflg;

	fp = wp->fp;
	colflg = ww_isColumnSelectionMode(wp);

	caret_positionCloseToMouseWithConfirmation(wp,0L);

	ln  = wp->caret.ln;
	col = wp->caret.offset;

	if ((lp = ln_gotoWP(wp,ln)) == 0)
		return 0;

	bl_hideSelectionInCurrentWindow(wp);

	o1 = 0;
	o2 = 0;
	if (type == MOT_SPACE || type == MOT_WORD) {
		LINE *(*func)(LINE * lp, long* ln, long* col, DIRECTION_OPTION dir, unsigned char unused) = (type == MOT_SPACE) ?
			caret_advanceWordOnly : caret_gotoIdentifierEnd;
		o2 = col;
		lp2 = (*func)(lp,&ln,&o2,1,0);
		o1  = o2;
		lp  = (*func)(lp,&ln,&o1,-1,0);
	} else if (type == MOT_PGRPH) {
		col = caret_advanceParagraph(wp, ln,1,1);
		ln  = caret_advanceParagraph(wp, col,-1,1);
		lp  = ln_gotoWP(wp,ln);
		lp2 = ln_gotoWP(wp,col);
	} else {	/* MOT_TOEND */
		lp2 = lp->next;
		if (lp2->next == 0 || colflg) {
			lp2 = lp;
			o2  = lp->len;
		}
	}
	CARET c1 = { .linePointer = lp, .offset = o1 };
	CARET c2 = { .linePointer = lp2, .offset = o2 };
	bl_syncSelectionWithCaret(wp, &c1, MARK_START, NULL);
	return bl_syncSelectionWithCaret(wp, &c2, MARK_END, NULL);
}

/*---------------------------------
 * EdSyncSelectionWithCaret()
 * PKS Edit command to set the end of the current selection to the caret position.
 *---------------------------------*/
EXPORT int EdSyncSelectionWithCaret(WINFO* wp, int flags)
{	
	// for now: always consider to swap the block marks, if the end mark is placed
	// before the start mark. In fact we should introduce the concept of a selection head instead
	// of swapping marks.
	return bl_syncSelectionWithCaret(wp, &wp->caret, flags | MARK_RECALCULATE, NULL);
}

