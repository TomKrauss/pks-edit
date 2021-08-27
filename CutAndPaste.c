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

#include <tos.h>
#include "caretmovement.h"
#include "textblocks.h"
#include "winfo.h"
#include "pksedit.h"
#include "edierror.h"
#include "iccall.h"
#include "editorconfiguration.h"
#include "errordialogs.h"
#include "clipboard.h"
#include "fileselector.h"
#include "markpositions.h"

 /*-----------------------*/
/* EXTERNALS			*/
/*-----------------------*/
extern unsigned char *bl_convertPasteBufferToText(unsigned char *b, unsigned char *end, 
				PASTE *pp);
extern void 	bl_validateTrashcanName(char *pszValid);
extern void 	bl_collectClipboardIds(char *pszValid);

extern LINE	*find_expandTabsInFormattedLines(WINFO *wp, LINE *lp);
extern int 	dlg_displayDialogTemplate(unsigned char c, 
				char *(*fpTextForTmplate)(char *s), char *s);

extern LINE	*cadv_gotoIdentifierEnd(LINE *lp,long *ln,long *col,int dir);
extern LINE	*cadv_gotoIdentifierSkipSpace(LINE *lp,long *ln,long *col,int dir);
extern LINE	*cadv_wordonly(LINE *lp,long *ln,long *col,int dir);

extern long	_multiplier;

/*----------------------------
 * bl_hideSelection() 		
 * Hide the current selection and optionally
 * remove the selection marker from "marked lines"
 *----------------------------*/
EXPORT int bl_hideSelection(WINFO* wp, int removeLineSelectionFlag) {
	FTABLE *	fp;
	LINE *		lp;
	LINE *		lpFirst;
	LINE *		lpLast;
	MARK	 *		mps;
	MARK *		mpe;
	long			ln;

	fp = wp->fp;
	mps = wp->blstart;
	mpe = wp->blend;

	if (removeLineSelectionFlag) {
		caret_placeCursorInCurrentFile(wp, wp->caret.ln,(long)wp->caret.offset);
	}
	lp = lpFirst = ln_goto(fp, wp->minln);
	for (ln = wp->minln; 
		lp && ln <= wp->maxln; 
		lp = lp->next, ln++) {
		if (lp->lflg & (LNXMARKED|LNDIFFMARK)) {
			lpLast = lp;
		}
	}
	if (mps && mpe) {
		if (removeLineSelectionFlag) {
			ln_removeFlag(mps->m_linePointer, mpe->m_linePointer, (LNXMARKED | LNDIFFMARK));
			if (mps->m_linePointer == mpe->m_linePointer) {
				if (mps->m_column != mpe->m_column) {
					render_repaintLine(fp, mps->m_linePointer);
				}
			} else {
				render_repaintLineRange(fp, mps->m_linePointer, mpe->m_linePointer);
			}
		}
	} else {
		if (removeLineSelectionFlag) {
			LINE* lp = mps ? mps->m_linePointer : (mpe ? mpe->m_linePointer : wp->caret.linePointer);
			ln_removeFlag(lp, lp, (LNXMARKED|LNDIFFMARK));
			render_repaintLine(fp, lp);
		}
	}
	mark_killSelection(wp);
	return 1;
}

/*---------------------------------*/
/* bl_pasteBlock()					*/
/*---------------------------------*/
EXPORT int bl_pasteBlock(PASTE *buf, int colflg, int offset, int move) {
	LINE *	lp;
	long 	col,ln;
	long 	delta,oln;
	int	    	ret;
	FTABLE *	fp;
	WINFO *	wp = ww_getCurrentEditorWindow();

	fp = wp->fp;
	if ((ret = bl_paste(buf,wp,wp->caret.linePointer, offset,colflg)) == 0 ||
		colflg ||
		(wp->workmode & BLK_LINES)) {
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
		EdSyncSelectionWithCaret(MARK_END);
	}
	return ret;
}

/*-----------------------*/
/* paste()			*/
/*-----------------------*/
EXPORT int paste(PASTE *buf,int move)
{
	WINFO* wp = ww_getCurrentEditorWindow();

	return bl_pasteBlock(buf,ww_isColumnSelectionMode(wp), wp->caret.offset, move);
}

/*--------------------------------------------------------------------------
 * TextForTrash()
 */
static char *TextForTrash(char *s)
{
	PASTE *	pp;

	pp = bl_getBlockFromUndoBuffer(*s - '0');
	return bl_convertPasteBufferToText(_linebuf, _linebuf + 256, pp);
}

/*--------------------------------------------------------------------------
 * TextForClip()
 */
static char *TextForClip(char *s)
{
	PASTE *	pp;

	pp = bl_addrbyid(*s, 0);
	return bl_convertPasteBufferToText(_linebuf, _linebuf + 256, pp);
}


/*----------------------------*/
/* GetTmplateDlg() 			*/
/*----------------------------*/
static int GetTmplateDlg(char * (*fpTextForTmplate)(char *s), void (*fpGetValid)(char *s2))
{	static unsigned char c;
	char cIdentChars[256];

     (*fpGetValid)(cIdentChars);
	c = dlg_displayDialogTemplate(c, fpTextForTmplate, cIdentChars);
	return c;
}

/*---------------------------------*/
/* bl_getPasteBuffer()				*/
/*---------------------------------*/
EXPORT PASTE *bl_getPasteBuffer(int which) {
	PASTE *	pp;
	char 	id;
	int		err=0;
	int		icon;
	char *	(*getTextFunc)(char *s);
	void		(*validateNameFunction)(char *s);

	if (which & PASTE_QUERYID) {
		getTextFunc = (which & PASTE_XUNDO) ? 
			TextForTrash : TextForClip; 
		validateNameFunction = (which & PASTE_XUNDO) ? 
			bl_validateTrashcanName : bl_collectClipboardIds; 
		if ((id = GetTmplateDlg(getTextFunc, validateNameFunction)) == 0) {
			return 0;
		}
		if (which & PASTE_XUNDO) {
			id -= '0';
		}
	} else {
		id = (char )which;
	}

	switch (which & (PASTE_UNDO|PASTE_XUNDO|PASTE_CLIP)) {
		case PASTE_UNDO:
			icon = ICID_TRASH;
			pp = _undobuf;
			err = IDS_MSGNOTRASH;
			break;
		case PASTE_XUNDO:
			icon = ICID_TRASH;
			pp = bl_getBlockFromUndoBuffer(id);
			err = IDS_MSGNOUNDOBLOCK;
			break;
		default:
			icon = ICID_CLIP;
			/* try to get data from windows clipboard */
			if (!id)
				clp_getdata();
			pp = bl_addrbyid(id,0);
			err = IDS_MSGNOCLIP;
			break;
	}

	if (!pp || !pp->pln) {
		error_showErrorById(err);
		return 0;
	}

	return pp;
}

/*------------------------------------------------------------
 * EdBlockPaste()
 */
EXPORT int EdBlockPaste(int which)
{	PASTE *	pp;
	WINFO *	wp;

     wp = ww_getCurrentEditorWindow();
	if ((pp = bl_getPasteBuffer(which)) != 0) {
		if ((GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) && ww_hasSelection(wp)) {
			EdBlockDelete(0);
		}
		return paste(pp,0);
	}

	return 0;
}

/*----------------------------
 * bl_insertPasteBufFromFile()
 * Read a text block from a file with the given name
 * and insert it into the current document.
 *----------------------------*/
EXPORT int bl_insertPasteBufFromFile(char *fn)
{	PASTE	pbuf;
	char 	fname[256];
	int		ret;

	if (!ft_getCurrentDocument()) 
		return 0;
	if (fn == 0) {
		if (!fsel_selectFileWithTitle(MREADF,fname, FALSE))
			return 0;
		fn = fname;
	}
	if ((ret = bl_readFileIntoPasteBuf(&pbuf, fn, 0)) != 0)
		paste(&pbuf,0);
	ln_listfree(pbuf.pln);
	return ret;
}

/*
 * PKS Edit command to read a block from a default file name 
 */
EXPORT int EdBlockRead(void ) { 
	return bl_insertPasteBufFromFile((char *)0);	
}

/*
 * PKS Edit command to write a block to a default file name. 
 */
EXPORT int EdBlockWrite(void ){ 	
	return EdBlockWriteToFile((char *)0);	
}

/*---------------------------------*/
/* CutBlock()					*/
/*---------------------------------*/
EXPORT int CutBlock(MARK *ms, MARK *me, int flg, PASTE *pp)
{
	PASTE 	_p;
	int	 	id;
	int	 	colflg = ww_isColumnSelectionMode(ww_getCurrentEditorWindow());

	_p.pln = 0;
	bl_free(&_p);
	if (flg & CUT_QUERYID) {
		if ((id = GetTmplateDlg(TextForClip, bl_collectClipboardIds)) == 0) {
			return 0;
		}
	} else {
		id = flg & 0xFF;
	}

	if (!(flg & CUT_USEPP)) {
		if ((pp = bl_addrbyid(id,(flg & CUT_APPND) ? 0 : 1)) == 0)
			return 0;
	} else {
		pp->pln = 0;
	}

	if (bl_cut(&_p,ms->m_linePointer,me->m_linePointer,ms->m_column,me->m_column,0,colflg)) {

		if (!(flg & CUT_APPND))
			bl_free(pp);

		if (id == 0)
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
/* EdBlockCut()				*/
/*----------------------------*/
EXPORT int EdBlockCut(int flg,PASTE *pp)
{
	WINFO* wp = ww_getCurrentEditorWindow();

	if (!ww_checkSelectionWithError(wp))
		return 0;
	return CutBlock(wp->blstart,wp->blend,flg,pp);
}

/*---------------------------------*
 * EdBlockWriteToFile()
 * PKS Edit command to write a block to a file with the given name. 
 *---------------------------------*/
EXPORT int EdBlockWriteToFile(char *fn)
{	PASTE  pbuf;
	char   fname[256];
	int	  ret;

	if (EdBlockCut(CUT_USEPP,&pbuf)) {
		if (fn != 0) {
			ret = F_NORMOPEN;
		} else {
			ret = fsel_selectFileWithTitle(MWRITEF,fname,TRUE);
			fn  = fname;
		}
		if (ret != 0)
			ret = bl_writePasteBufToFile(&pbuf, fn, ret);
		ln_listfree(pbuf.pln);
		return ret;
	}
	return 0;
}

/*---------------------------------*/
/* block_rw()					*/
/*---------------------------------*/
EXPORT int block_rw(char *fn,int doread)
{
	PASTE *bp = bl_addrbyid(0,0);

	if (!bp)
		return 0;

	if (doread) {
		if (bl_readFileIntoPasteBuf(bp, fn, -1))
			return 1;
		return 0;
	} else
		return bl_writePasteBufToFile(bp, fn, F_NORMOPEN);
}

/*---------------------------------*/
/* upd_lines() 				*/
/*---------------------------------*/
static void upd_lines()
{	register	WINFO *wp = ww_getCurrentEditorWindow();
	register	LINE *lp;
	register	long ln = -1L;
	FTABLE* fp = wp->fp;

	lp = wp->caret.linePointer;
	while(lp) {		    // make the whole thing faster: test (lp) ! 
		ln++, lp = lp->prev;
	}
	caret_moveToLine(wp, ln);
}

/*------------------------------
 * EdBlockCopyOrMove()
 * PKS Edit command to copy or move a block.
 *----------------------------*/
EXPORT int EdBlockCopyOrMove(BOOL move) {	
	LINE   *lp,*ls,*le;
	int	  cs,ce,offs;
	long   delta,dln;
	PASTE  pbuf;
	FTABLE *fp;
	WINFO* wp;
	MARK   *bstart,*bend;
	int	  move_nocolblk,ret,colflg;

	wp = ww_getCurrentEditorWindow();
	fp = wp->fp;
	colflg = ww_isColumnSelectionMode(wp);

	if (!ww_checkSelectionWithError(wp))
		return 0;

	/*
	 * due to positioning problems afterwards: expand destination line
	 */
	if (colflg) {
		offs = caret_lineOffset2screen(wp, &wp->caret);
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
		lp = ls;
		while (!P_EQ(lp,le)) {
			if (P_EQ(lp,wp->caret.linePointer)) {	/* makes no sense	*/
				if (lp == NULL || !P_EQ(lp,ls) || offs >= cs) {
nomove:				error_showErrorById(IDS_MSGBADBLOCKMOVE);
					return 0;
				}
			}
			lp = lp->next;
		}
		if (P_EQ(wp->caret.linePointer,le)) {
			if (offs <= ce) {
				if (P_EQ(ls,le)) {
					if (offs >= cs) goto nomove;
					goto nodelta;
				}
				goto nomove;
			}
			if (!P_EQ(ls,le)) dln = -1L;
			delta = -(ce-cs);
nodelta:		;
		}
	}

	pbuf.pln = 0;
	if ((ret = bl_cut(&pbuf,ls,le,cs,ce,move,colflg)) != 0) {
		if (move_nocolblk) {
			caret_placeCursorInCurrentFile(wp, (long)(wp->caret.ln+dln),(long)(offs + delta));
			bl_hideSelection(wp, 0);
			EdSyncSelectionWithCaret(MARK_START);
			wp->blstart->m_column--;
			upd_lines();
		}
		if (colflg) {
			delta = wp->blcol2 - wp->blcol1;
			dln	 = ln_cnt(bstart->m_linePointer,bend->m_linePointer) - 1;
			bl_hideSelection(wp, 0);
		} else {
			offs = wp->caret.offset;
		}

		ret = bl_pasteBlock(&pbuf,colflg,offs,move);
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

/*---------------------------------*/
/* bl_placeCursorOnBlockMark()					*/
/*---------------------------------*/
static int bl_placeCursorOnBlockMark(MARK *mp) {	
	long newln;
	WINFO* wp = ww_getCurrentEditorWindow();

	if (mp != 0) {
		newln = ln_indexOf(wp->fp,mp->m_linePointer);
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
EXPORT void EdBlockFindEnd(void)
{
	if (ww_getCurrentEditorWindow()) bl_placeCursorOnBlockMark(ww_getCurrentEditorWindow()->blend);
}

/*---------------------------------*/
/* EdBlockFindStart()				*/
/*---------------------------------*/
EXPORT int EdBlockFindStart()
{
	if (ww_getCurrentEditorWindow()) return bl_placeCursorOnBlockMark(ww_getCurrentEditorWindow()->blstart);
	return 0;
}

/*----------------------------
 * EdBlockDelete()
 * PKS Edit macro command to delete the current selection and
 * optionally save the text in the trashcan clipboard of PKS Edit
 *----------------------------*/
EXPORT int EdBlockDelete(int bSaveTrash)
{
	MARK		ms;
	MARK		me;
	WINFO*		wp;
	FTABLE 	*	fp;
	int 		ret;
	
	wp = ww_getCurrentEditorWindow();
	fp = wp->fp;
	if (!ww_checkSelectionWithError(wp))
		return 0;
	ms = *wp->blstart;
	me = *wp->blend;
	EdBlockFindStart();
	bl_hideSelection(wp, 0);
	if (bSaveTrash) {
		ret = bl_undoIntoUnqBuffer(wp, ms.m_linePointer,me.m_linePointer,ms.m_column,me.m_column,1);
	} else {
		ret = bl_delete(wp, ms.m_linePointer, me.m_linePointer, ms.m_column, me.m_column, 1, 0);
	}
	render_repaintAllForFile(fp);
	return ret;
}

/*---------------------------------*/
/* EdLinesYank() 				*/
/*---------------------------------*/
EXPORT int EdLinesYank()
{
	LINE *ls,*le;
	long n = _multiplier;
	PASTE *bp = bl_addrbyid(0,0);
	WINFO* wp = ww_getCurrentEditorWindow();

	if (bp == 0 || wp == 0L) return 0;
	ls = wp->caret.linePointer;
	if ((le = ln_relative(wp->caret.linePointer,n)) == 0) return 0;
	
	if (bl_cut(bp,ls,le,0,(P_EQ(ls,le)) ? ls->len : 0, 0, 0)) {
		clp_setmine();
		return 1;
	}
	return 0;
}

/*----------------------------*/
/* isbefore()				*/
/*----------------------------*/
static int isbefore(FTABLE *fp, MARK *mark, LINE *fpcl, int col, int markend)
{
	LINE *	lp = fp->firstl;
	LINE *	mlm;

	mlm	= mark->m_linePointer;
	if (P_EQ(fpcl,mlm)) {
		return((markend) ? (col <= mark->m_column) : (col >= mark->m_column));
	}
	while(!P_EQ(fpcl,lp)) {
		if (P_EQ(mlm,lp)) {
			return (! markend);
		}
		lp = lp->next;
	}
	return markend;
}

/*----------------------------*/
/* bl_updateSelectionInLines() 			*/
/*----------------------------*/
static void bl_updateSelectionInLines(WINFO* wp, LINE *lpOldSelectionStart, LINE *lpOldSelectionEnd) {
	LINE *		lp1;
	LINE *		lp2;
	LINE *		lp;
	LINE *		lpFirst;
	LINE *		lpLast;
	FTABLE *	fp;
	long   		ln;

	fp  = wp->fp;
	lp1 = wp->blstart->m_linePointer;
	lp2 = wp->blend  ->m_linePointer;
	lp = lpFirst = ln_goto(fp,wp->minln);
	ln = ln_cnt(fp->firstl, lp1);
	if (ln_cnt(fp->firstl, lpOldSelectionStart) < ln) {
		lpFirst = lpOldSelectionStart;
	} else {
		lpFirst = lp1;
	}
	ln = ln_cnt(lpFirst, lp2);
	if (ln_cnt(lpFirst, lpOldSelectionEnd) < ln) {
		lpLast = lp2;
	} else {
		lpLast = lpOldSelectionEnd;
	}
	lp = lpFirst;
	while(lp) {
		render_repaintLine(fp, lp);
		if (lp == lpLast) {
			break;
		}
		lp = lp->next;
	}

}

/*---------------------------------*/
/* bl_setColumnSelection()				*/
/*---------------------------------*/
static void bl_setColumnSelectionNoRepaint(WINFO *wp) {
	MARK	 *mp;

	if ((mp = wp->blstart) != 0L) {
		wp->blcol1 = caret_lineOffset2screen(wp, &(CARET) { mp->m_linePointer, mp->m_column });
	}
	if ((mp = wp->blend) != 0L) {
		wp->blcol2 = caret_lineOffset2screen(wp, &(CARET) {
			mp->m_linePointer, mp->m_column
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
	int  		wasmarked,startx=0,endx=10000,markc,colflg;
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
	wasmarked = (lp1 && lp2);

	if (type == MARK_END) {
		markc = MARKEND;
		marks = wp->blstart;
	} else {
		markc = MARKSTART;
		marks = wp->blend;
	}

	bSwap = 0;
	if (workmode & BLK_LINES) {
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
		isbefore(fp, marks, lpMark, nMarkOffset, type == MARK_END)) {
		if (flags & MARK_RECALCULATE) {
			bSwap = 1;
		} else if (!(flags & MARK_NO_HIDE)) {
			bl_hideSelectionInCurrentWindow();
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
				wp->caret.linePointer, startx
			}),
				caret_lineOffset2screen(wp, &(CARET) {
					wp->caret.linePointer, endx
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
EXPORT int EdMouseMarkParts(int type)
{
	long   	ln,col;
	LINE *	lp;
	LINE *	lp2;
	long   	o1;
	long 	o2;
	WINFO* wp;
	FTABLE *	fp;
	int    	colflg;

	wp = ww_getCurrentEditorWindow();
	fp = wp->fp;
	colflg = ww_isColumnSelectionMode(wp);

	caret_positionCloseToMouseWithConfirmation(0L);

	ln  = wp->caret.ln;
	col = wp->caret.offset;

	if ((lp = ln_goto(fp,ln)) == 0)
		return 0;

	bl_hideSelectionInCurrentWindow();

	o1 = 0;
	o2 = 0;
	if (type == MOT_SPACE || type == MOT_WORD) {
		LINE *(*func)() = (type == MOT_SPACE) ? 
			cadv_wordonly : cadv_gotoIdentifierEnd;

		o2 = caret_screen2lineOffset(wp, &(CARET) { lp, col });
		lp2 = (*func)(lp,&ln,&o2,1);
		o1  = o2;
		lp  = (*func)(lp,&ln,&o1,-1);
	} else if (type == MOT_PGRPH) {
		col = caret_advanceParagraph(wp, ln,1,1);
		ln  = caret_advanceParagraph(wp, col,-1,1);
		lp  = ln_goto(fp,ln);
		lp2 = ln_goto(fp,col);
	} else {	/* MOT_TOEND */
		lp2 = lp->next;
		if (lp2->next == 0 || colflg) {
			lp2 = lp;
			o2  = lp->len;
		}
	}
	CARET c1 = { lp, o1 };
	CARET c2 = { lp2, o2 };
	bl_syncSelectionWithCaret(wp, &c1, MARK_START, NULL);
	return bl_syncSelectionWithCaret(wp, &c2, MARK_END, NULL);
}

/*---------------------------------
 * EdSyncSelectionWithCaret()
 * PKS Edit command to set the end of the current selection to the caret position.
 *---------------------------------*/
EXPORT int EdSyncSelectionWithCaret(int flags)
{	
	WINFO *		wp;

	wp = ww_getCurrentEditorWindow();
	// for now: always consider to swap the block marks, if the end mark is placed
	// before the start mark. In fact we should introduce the concept of a selection head instead
	// of swapping marks.
	return bl_syncSelectionWithCaret(wp, &wp->caret, flags | MARK_RECALCULATE, NULL);
}

