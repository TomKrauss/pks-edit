/*
 * CutAndPaste.c
 *
 * cut & paste, mark blocks
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 01.02.1991
 *									
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
#include "actions.h"

 /*-----------------------*/
/* EXTERNALS			*/
/*-----------------------*/
extern MARK	*mark_find(FTABLE *fp, int id);
extern MARK	*mark_set(FTABLE *fp, LINE *lp,int offs,int c);
extern unsigned char *bl_convertPasteBufferToText(unsigned char *b, unsigned char *end, 
				PASTE *pp);
extern void 	bl_validateTrashCanName(char *pszValid);
extern void 	bl_collectClipboardIds(char *pszValid);

extern LINE	*find_expandTabsInFormattedLines(FTABLE *fp, LINE *lp);
extern int 	dlg_displayDialogTemplate(unsigned char c, 
				char *(*fpTextForTmplate)(char *s), char *s);

extern LINE	*cadv_gotoIdentifierEnd(LINE *lp,long *ln,long *col,int dir);
extern LINE	*cadv_gotoIdentifierSkipSpace(LINE *lp,long *ln,long *col,int dir);
extern LINE	*cadv_wordonly(LINE *lp,long *ln,long *col,int dir);

extern long	_multiplier;

/*
 * Sets a block mark - if the "has current selection" property has changed, fire an action change.
 */
void bl_setBlockMark(FTABLE* fp, MARK* pMark, BOOL bStart) {
	BOOL bHasSelection = fp->blstart && fp->blend;
	if (bStart) {
		fp->blstart = pMark;
	}
	else {
		fp->blend = pMark;
	}
	if (bHasSelection != (fp->blstart && fp->blend)) {
		action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
	}
}

/*----------------------------
 * bl_hideSelection() 		
 * Hide the current selection and optionally
 * remove the selection marker from "marked lines"
 *----------------------------*/
EXPORT int bl_hideSelection(int removeLineSelectionFlag) {
	FTABLE *		fp;
	WINFO *		wp;
	LINE *		lp;
	LINE *		lpFirst;
	LINE *		lpLast;
	MARK	 *		mps;
	MARK *		mpe;
	long			ln;

	fp = ft_getCurrentDocument();
	mps = fp->blstart;
	mpe = fp->blend;
	wp = WIPOI(fp);

	if (removeLineSelectionFlag) {
		caret_placeCursorInCurrentFile(fp->ln,(long)fp->caret.offset);
	}
	lp = lpFirst = ln_relgo(fp, wp->minln - wp->ln);
	for (ln = wp->minln; 
		lp && ln <= wp->maxln; 
		lp = lp->next, ln++) {
		if (lp->lflg & (LNCPMARKED|LNXMARKED|LNDIFFMARK)) {
			lp->lflg |= LNREPLACED;
			lpLast = lp;
		}
	}
	if (mps && mpe) {
		ln_removeFlag(mps->lm,mpe->lm,LNCPMARKED);
		if (removeLineSelectionFlag) {
			if (mps->lm == mpe->lm) {
				if (mps->lc != mpe->lc) {
					render_repaintLine(fp, mps->lm);
				}
			} else {
				render_repaintLineRange(fp, mps->lm, mpe->lm);
			}
		}
	} else {
		if (removeLineSelectionFlag && (mps || mpe)) {
			LINE* lp = mps ? mps->lm : mpe->lm;
			ln_removeFlag(lp, lp, (LNCPMARKED|LNXMARKED|LNDIFFMARK));
			render_repaintLine(fp, lp);
		}
	}
	ln_removeFlag(lpFirst, lpLast, LNREPLACED);
	mark_killSelection(fp);
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
	WINFO *	wp;

	fp = ft_getCurrentDocument();
	wp = WIPOI(fp);

	if ((ret = bl_paste(buf,fp,fp->caret.linePointer, offset,colflg)) == 0 ||
		colflg ||
		(fp->documentDescriptor->workmode & BLK_LINES)) {
		if (ret == 0 || !move) {
			render_repaintAllForFile(fp);
		}
		return ret;
	}

	delta = buf->nlines-1L;
	ln = buf->nlines+fp->ln-1L;
	lp = buf->pln;
	while(lp->next) lp=lp->next;
	col = lp->len;
	oln = fp->ln;
	if (buf->nlines == 1) 
		col += offset;
	if (!move) {
		if (delta > 0 && (oln+delta) <= wp->maxln) {
			caret_placeCursorInCurrentFile(fp->ln+1L,0L);
			wt_insline(wp,(int ) delta);
		}
		if (ln > wp->maxln) delta = wp->maxln;
		else delta = ln;
		render_repaintFromLineTo(wp,oln,delta);
		caret_placeCursorInCurrentFile(ln,col);		
	} else {
		fp->blstart->lc++; /* get block marked afterwards (s. cpy_mv) */
		caret_placeCursorInCurrentFile(ln,col);
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
	return bl_pasteBlock(buf,ww_hasColumnSelection(WIPOI(ft_getCurrentDocument())),
				ft_getCurrentDocument()->caret.offset, move);
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
			bl_validateTrashCanName : bl_collectClipboardIds; 
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
	FTABLE *	fp;

     fp = ft_getCurrentDocument();
	if ((pp = bl_getPasteBuffer(which)) != 0) {
		if ((GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) && ft_checkSelection(fp)) {
			EdBlockDelete(0);
		}
		return paste(pp,0);
	}

	return 0;
}

/*----------------------------
 * bl_insertTextBlockFromFile()
 * Read a text block from a file with the given name
 * and insert it into the current document.
 *----------------------------*/
EXPORT int bl_insertTextBlockFromFile(char *fn)
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
	if ((ret = bl_read(fn,&pbuf,0)) != 0)
		paste(&pbuf,0);
	ln_listfree(pbuf.pln);
	return ret;
}

/*
 * PKS Edit command to read a block from a default file name 
 */
EXPORT int EdBlockRead(void ) { 
	return bl_insertTextBlockFromFile((char *)0);	
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
	int	 	colflg = ww_hasColumnSelection(WIPOI(ft_getCurrentDocument()));

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

	if (bl_cut(&_p,ms->lm,me->lm,ms->lc,me->lc,0,colflg)) {

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
	if (!ft_checkSelectionWithError(ft_getCurrentDocument()))
		return 0;
	return CutBlock(ft_getCurrentDocument()->blstart,ft_getCurrentDocument()->blend,flg,pp);
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
			ret = bl_write(fn,&pbuf,ret);
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
		if (bl_read(fn,bp,-1))
			return 1;
		return 0;
	} else
		return bl_write(fn,bp,F_NORMOPEN);
}

/*---------------------------------*/
/* upd_lines() 				*/
/*---------------------------------*/
static void upd_lines()
{	register	FTABLE *fp = ft_getCurrentDocument();
	register	LINE *lp;
	register	long ln = -1L;

	lp = fp->caret.linePointer;
	while(lp) {		    /* make the whole thing faster: test (lp) ! */ 
		ln++, lp = lp->prev;
	}
	fp->ln = ln;
	WIPOI(ft_getCurrentDocument())->ln = ln;
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
	MARK   *bstart,*bend;
	int	  move_nocolblk,ret,colflg;

	fp = ft_getCurrentDocument();
	colflg = ww_hasColumnSelection(WIPOI(ft_getCurrentDocument()));

	if (!ft_checkSelectionWithError(fp))
		return 0;

	/*
	 * due to positioning problems afterwards: expand destination line
	 */
	if (colflg) {
		offs = caret_lineOffset2screen(fp, &fp->caret);
		if (find_expandTabsInFormattedLines(fp,fp->caret.linePointer) == 0)
			return 0;
	} else {
		offs = fp->caret.offset;
	}

	delta = 0;
	dln   = 0;
	ret   = 0;

	move_nocolblk = (move && !colflg);
	bstart = fp->blstart;
	bend   = fp->blend;

	ls = bstart->lm, cs = bstart->lc;
	le = bend->lm,   ce = bend->lc;
	if (move_nocolblk) {			/* valid move ??	*/
		lp = ls;
		while (!P_EQ(lp,le)) {
			if (P_EQ(lp,fp->caret.linePointer)) {	/* makes no sense	*/
				if (!P_EQ(lp,ls) || offs >= cs) {
nomove:				error_showErrorById(IDS_MSGBADBLOCKMOVE);
					return 0;
				}
			}
			lp = lp->next;
		}
		if (P_EQ(fp->caret.linePointer,le)) {
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
			caret_placeCursorInCurrentFile((long)(fp->ln+dln),(long)(offs + delta));
			bl_hideSelection(0);
			EdSyncSelectionWithCaret(MARK_START);
			fp->blstart->lc--;
			upd_lines();
		}
		if (colflg) {
			delta = fp->blcol2 - fp->blcol1;
			dln	 = ln_cnt(bstart->lm,bend->lm) - 1;
			bl_hideSelection(0);
		} else {
			offs = fp->caret.offset;
		}

		ret = bl_pasteBlock(&pbuf,colflg,offs,move);
		if (colflg) {
			caret_placeCursorInCurrentFile(fp->ln,(long)offs);
			fp->blcol1 = offs;
			fp->blcol2 = offs + delta;
			bl_setSelection(fp,fp->caret.linePointer,offs,
				ln_relative(fp->caret.linePointer,dln),fp->blcol2);
		}
		render_repaintAllForFile(fp);
	}
	bl_free(&pbuf);

	return ret;
}

/*---------------------------------*/
/* blfin()					*/
/*---------------------------------*/
static int blfin(MARK *mp)
{	long newln;

	if (mp != 0) {
		newln = ln_indexOf(ft_getCurrentDocument(),mp->lm);
		caret_placeCursorAndSavePosition(newln,(long)mp->lc);
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
	if (ft_getCurrentDocument()) blfin(ft_getCurrentDocument()->blend);
}

/*---------------------------------*/
/* EdBlockFindStart()				*/
/*---------------------------------*/
EXPORT int EdBlockFindStart()
{
	if (ft_getCurrentDocument()) return blfin(ft_getCurrentDocument()->blstart);
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
	FTABLE 	*	fp;
	int 		ret;
	
	fp = ft_getCurrentDocument();
	if (!ft_checkSelectionWithError(fp))
		return 0;
	ms = *fp->blstart;
	me = *fp->blend;
	EdBlockFindStart();
	bl_hideSelection(0);
	if (bSaveTrash) {
		ret = bl_undoIntoUnqBuffer(ms.lm,me.lm,ms.lc,me.lc,1);
	} else {
		ret = bl_delete(fp, ms.lm, me.lm, ms.lc, me.lc, 1, 0);
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

	if (bp == 0 ||ft_getCurrentDocument() == 0L) return 0;
	ls = ft_getCurrentDocument()->caret.linePointer;
	if ((le = ln_relgo(ft_getCurrentDocument(),n)) == 0) return 0;
	
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

	mlm	= mark->lm;
	if (P_EQ(fpcl,mlm)) {
		return((markend) ? (col <= mark->lc) : (col >= mark->lc));
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
/* marklines() 			*/
/*----------------------------*/
static void marklines(int changed,int colflg,
	LINE *lpolds, LINE *lpolde, int markc)
{
	LINE *		lp1;
	LINE *		lp2;
	LINE *		lp;
	LINE *		lpFirst;
	LINE *		lpLast;
	FTABLE *		fp;
	WINFO  *		wp;
	unsigned char 	flags[512];
	long   		ln;

	fp  = ft_getCurrentDocument();
	wp  = WIPOI(fp);
	lp1 = fp->blstart->lm;
	lp2 = fp->blend  ->lm;

	lp = lpFirst = ln_relgo(fp,wp->minln-wp->ln);
	for (ln = wp->minln; lp && ln <= wp->maxln; lp = lp->next, ln++) {
		lpLast = lp;
		flags[ln-wp->minln] = lp->lflg;
	}

	if (changed) {
		ln_removeFlag(fp->firstl,lp1,LNCPMARKED);
	}
	ln_addFlag (lp1,lp2,LNCPMARKED);
	if (changed) {
		ln_removeFlag(lp2->next,fp->lastl,LNCPMARKED);
	}

	lp = lpFirst;
	if (colflg) {
		ln = ln_cnt(fp->firstl, lp1);
		if (ln_cnt(fp->firstl, lpolds) < ln) {
			lpFirst = lpolds;
		} else {
			lpFirst = lp1;
		}
		ln = ln_cnt(lpFirst, lp2);
		if (ln_cnt(lpFirst, lpolde) < ln) {
			lpLast = lp2;
		} else {
			lpLast = lpolde;
		}
		while(lp) {
			if (!(lp->lflg & LNREPLACED)) {
				lp->lflg |= LNREPLACED;
				render_repaintLine(fp, lp);
			}
			if (lp == lpLast) {
				break;
			}
			lp = lp->next;
		}
	} else for (ln = wp->minln; lp && ln <= wp->maxln; lp = lp->next, ln++) {
		if ((P_EQ(lp, lpolds) && !P_EQ(lp, lp1)) ||
			(P_EQ(lp, lpolde) && !P_EQ(lp, lp2)) ||
			(P_EQ(lp, lp1) && markc == MARKSTART) ||
			(P_EQ(lp, lp2) && markc == MARKEND) ||
			flags[ln - wp->minln] != lp->lflg) {
			lp->lflg |= LNREPLACED;
			render_repaintLine(fp, lp);
		}
	}

	ln_removeFlag(lpFirst, lpLast, LNREPLACED);
}

/*---------------------------------*/
/* bl_setColumnSelection()				*/
/*---------------------------------*/
static void bl_setColumnSelectionNoRepaint(FTABLE *fp)
{	MARK	 *mp;

	if ((mp = fp->blstart) != 0L) {
		fp->blcol1 = caret_lineOffset2screen(fp, &(CARET) { mp->lm, mp->lc });
	}
	if ((mp = fp->blend) != 0L) {
		fp->blcol2 = caret_lineOffset2screen(fp, &(CARET) {
			mp->lm, mp->lc
		});
		if (fp->blcol2 <= fp->blcol1)
			fp->blcol2 = fp->blcol1	+ 1;
	}
}

/*------------------------------------------------------------
 * bl_setColumnSelection()
 */
EXPORT int bl_setColumnSelection(FTABLE *fp)
{
	bl_setColumnSelectionNoRepaint(fp);
	render_repaintAllForFile(fp);
	return 1;
}

/*---------------------------------
 * bl_setSelection()
 * Set the current selection on a given file.
 *---------------------------------*/
EXPORT int bl_setSelection(FTABLE *fp, LINE *lps, int cs, LINE *lpe, int ce)
{
	if (lps) 
		bl_setBlockMark(fp, mark_set(fp,lps,cs,MARKSTART), TRUE);
	if (lpe) 
		bl_setBlockMark(fp, mark_set(fp,lpe,ce,MARKEND), FALSE);
	if (lps && lpe) {
		ln_addFlag(lps,lpe,LNCPMARKED);
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
int bl_syncSelectionWithCaret(FTABLE *fp, CARET *lpCaret, int flags, int *pMarkSet)
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

	type = (flags & (~(MARK_COLUMN|MARK_RECALCULATE|MARK_NO_HIDE)));
	workmode = fp->documentDescriptor->workmode;

	if (type == MARK_ALL) {
		lp2 = fp->lastl->prev;
		bl_setSelection(fp,fp->firstl,0,lp2,lp2->len);
		fp->blcol1 = 0;
		fp->blcol2 = MAXLINELEN;
		render_repaintAllForFile(fp);
		return 1;
	}

	colflg = ww_hasColumnSelection(WIPOI(fp));

	if (fp->blstart) {
		lp1	  = fp->blstart->lm;
		startx = fp->blstart->lc;
	}
	if (fp->blend) {
		lp2	= fp->blend  ->lm;
		endx = fp->blend->lc;
	}
	wasmarked = (lp1 && lp2);

	if (type == MARK_END) {
		markc = MARKEND;
		marks = fp->blstart;
	} else {
		markc = MARKSTART;
		marks = fp->blend;
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
			EdBlockHide();
		}
	}

	mark_set(fp, lpMark, nMarkOffset, markc);
	marks = mark_find(fp, MARKSTART);
	marke = mark_find(fp, MARKEND);

	if (bSwap) {
		if (marks) {
			marks->mchar = MARKEND;
		}
		if (marke) {
			marke->mchar = MARKSTART;
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
	bl_setBlockMark(fp, marks, TRUE);
	bl_setBlockMark(fp, marke, FALSE);

	if (colflg && !(flags & MARK_COLUMN)) {
		bl_setColumnSelectionNoRepaint(fp);
	}

	if (marks && marke) {		/* mark in a line */
		if ( colflg == 0	  && 
			P_EQ(lp1,marks->lm) && 
			P_EQ(lp2,marke->lm)) {
			if (marke->lc != endx) {
				if (marke->lc < endx)
					startx = marke->lc;
				else {
					startx = endx;
					endx   = marke->lc;
				}
			} else {
				if (marks->lc < startx) {
					endx   = startx;
					startx = marks->lc;
				} else {
					endx = marks->lc;
				}
			}
			render_repaintLinePart(fp, fp->ln, caret_lineOffset2screen(fp, &(CARET) {
				fp->caret.linePointer, startx
			}),
				caret_lineOffset2screen(fp, &(CARET) {
					fp->caret.linePointer, endx
				}));
		}
		else {
			marklines(wasmarked, colflg, lp1, lp2, markc);
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
	FTABLE *	fp;
	int    	colflg;

	fp = ft_getCurrentDocument();
	colflg = ww_hasColumnSelection(WIPOI(fp));

	caret_positionCloseToMouseWithConfirmation(0L);

	ln  = fp->ln;
	col = fp->col;

	if ((lp = ln_gotouserel(fp,ln)) == 0)
		return 0;

	EdBlockHide();

	o1 = 0;
	o2 = 0;
	if (type == MOT_SPACE || type == MOT_WORD) {
		LINE *(*func)() = (type == MOT_SPACE) ? 
			cadv_wordonly : cadv_gotoIdentifierEnd;

		o2 = caret_screen2lineOffset(fp, &(CARET) { lp, col });
		lp2 = (*func)(lp,&ln,&o2,1);
		o1  = o2;
		lp  = (*func)(lp,&ln,&o1,-1);
	} else if (type == MOT_PGRPH) {
		col = caret_advanceParagraph(ln,1,1);
		ln  = caret_advanceParagraph(col,-1,1);
		lp  = ln_gotouserel(fp,ln);
		lp2 = ln_gotouserel(fp,col);
	} else {	/* MOT_TOEND */
		lp2 = lp->next;
		if (lp2->next == 0 || colflg) {
			lp2 = lp;
			o2  = lp->len;
		}
	}
	CARET c1 = { lp, o1 };
	CARET c2 = { lp2, o2 };
	bl_syncSelectionWithCaret(fp, &c1, MARK_START, NULL);
	return bl_syncSelectionWithCaret(fp, &c2, MARK_END, NULL);
}

/*---------------------------------
 * EdSyncSelectionWithCaret()
 * PKS Edit command to set the end of the current selection to the caret position.
 *---------------------------------*/
EXPORT int EdSyncSelectionWithCaret(int flags)
{	
	FTABLE *		fp;

	fp = ft_getCurrentDocument();
	// for now: always consider to swap the block marks, if the end mark is placed
	// before the start mark. In fact we should introduce the concept of a selection head instead
	// of swapping marks.
	return bl_syncSelectionWithCaret(fp, &fp->caret, flags | MARK_RECALCULATE, NULL);
}

