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
#include "winfo.h"
#include "pksedit.h"
#include "edierror.h"
#include "iccall.h"
#include "editorconfiguration.h"
#include "errordialogs.h"

/*-----------------------*/
/* EXTERNALS			*/
/*-----------------------*/
extern int 	bl_delete(FTABLE *fp, LINE *lnfirst, LINE *lnlast, int cfirst,
				int clast, int blkflg, int saveintrash);
extern MARK	*mark_find(FTABLE *fp, int id);
extern MARK	*mark_set(FTABLE *fp, LINE *lp,int offs,int c);
extern PASTE	*bl_addrbyid(int id,int insert);
extern PASTE	*blgetundo(int num);
extern void 	redrawlinepart(FTABLE *fp, long ln, int col1, int col2);
extern void 	redrawspecificline(FTABLE *fp, LINE *lpWhich);
extern unsigned char *BlockAsBuffer(unsigned char *b, unsigned char *end, 
				PASTE *pp);
extern void 	ValidTrashCans(char *pszValid);
extern void 	ValidClipboards(char *pszValid);

extern LINE	*condexpline(FTABLE *fp, LINE *lp);
extern int 	DialogTemplate(unsigned char c, 
				char *(*fpTextForTmplate)(char *s), char *s);

extern int	p_redraw(void );
extern LINE	*cadv_word(LINE *lp,long *ln,long *col,int dir);
extern LINE	*cadv_space(LINE *lp,long *ln,long *col,int dir);
extern LINE	*cadv_wordonly(LINE *lp,long *ln,long *col,int dir);
extern long	cparagrph(long ln, int dir, int start);

extern PASTE	*_undobuf;
extern long	_multiplier;

/*----------------------------*/
/* EdBlockHide() 			*/
/*----------------------------*/
EXPORT int Pastehide(flg)
{
	FTABLE *		fp;
	WINFO *		wp;
	LINE *		lp;
	LINE *		lpFirst;
	LINE *		lpLast;
	MARK	 *		mps;
	MARK *		mpe;
	long			ln;

	fp = ft_CurrentDocument();
	mps = fp->blstart;
	mpe = fp->blend;
	wp = WIPOI(fp);

	if (flg) {
		curpos(fp->ln,(long)fp->caret.offset);
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
		if (flg) {
			if (mps->lm == mpe->lm) {
				if (mps->lc != mpe->lc) {
					redrawspecificline(fp, mps->lm);
				}
			} else {
				p_redraw();
			}
		}
	} else {
		if (flg) {
			ln_removeFlag(fp->firstl,fp->lastl,(LNCPMARKED|LNXMARKED|LNDIFFMARK));
			p_redraw();
		}
	}
	ln_removeFlag(lpFirst, lpLast, LNREPLACED);
	mcp_release(fp);
	return 1;
}

/*---------------------------------*/
/* pasteblk()					*/
/*---------------------------------*/
EXPORT int pasteblk(PASTE *buf, int colflg, int offset, int move)
{
	LINE *	lp;
	long 	col,ln;
	long 	delta,oln;
	int	    	ret;
	FTABLE *	fp;
	WINFO *	wp;

	fp = ft_CurrentDocument();
	wp = WIPOI(fp);

	if ((ret = bl_paste(buf,fp,fp->caret.linePointer, offset,colflg)) == 0 ||
		colflg ||
		(fp->documentDescriptor->workmode & BLK_LINES)) {
		if (ret == 0 || !move) {
			RedrawTotalWindow(fp);
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
			curpos(fp->ln+1L,0L);
			wt_insline(wp,(int ) delta);
		}
		if (ln > wp->maxln) delta = wp->maxln;
		else delta = ln;
		RedrawFromTo(wp,oln,delta);
		curpos(ln,col);		
	} else {
		fp->blstart->lc++; /* get block marked afterwards (s. cpy_mv) */
		curpos(ln,col);
		RedrawTotalWindow(fp);
		EdBlockMark(MARK_END);
	}
	return ret;
}

/*-----------------------*/
/* paste()			*/
/*-----------------------*/
EXPORT int paste(PASTE *buf,int move)
{
	return pasteblk(buf,ww_blkcolomn(WIPOI(ft_CurrentDocument())),
				ft_CurrentDocument()->caret.offset, move);
}

/*--------------------------------------------------------------------------
 * TextForTrash()
 */
static char *TextForTrash(char *s)
{
	PASTE *	pp;

	pp = blgetundo(*s - '0');
	return BlockAsBuffer(_linebuf, _linebuf + 256, pp);
}

/*--------------------------------------------------------------------------
 * TextForClip()
 */
static char *TextForClip(char *s)
{
	PASTE *	pp;

	pp = bl_addrbyid(*s, 0);
	return BlockAsBuffer(_linebuf, _linebuf + 256, pp);
}


/*----------------------------*/
/* GetTmplateDlg() 			*/
/*----------------------------*/
static int GetTmplateDlg(char * (*fpTextForTmplate)(char *s), void (*fpGetValid)(char *s2))
{	static unsigned char c;
	char cIdentChars[256];

     (*fpGetValid)(cIdentChars);
	c = DialogTemplate(c, fpTextForTmplate, cIdentChars);
	return c;
}

/*---------------------------------*/
/* bl_getpaste()				*/
/*---------------------------------*/
EXPORT PASTE *bl_getpaste(int which)
{
	PASTE *	pp;
	char 	id;
	int		err=0;
	int		icon;
	char *	(*getTextFunc)(char *s);
	void		(*validCharFunc)(char *s);

	if (which & PASTE_QUERYID) {
		getTextFunc = (which & PASTE_XUNDO) ? 
			TextForTrash : TextForClip; 
		validCharFunc = (which & PASTE_XUNDO) ? 
			ValidTrashCans : ValidClipboards; 
		if ((id = GetTmplateDlg(getTextFunc, validCharFunc)) == 0) {
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
			pp = blgetundo(id);
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
		ed_error(err);
		return 0;
	}

	if (which & PASTE_ICON)
		iflash(icon,id);
	return pp;
}

/*------------------------------------------------------------
 * EdBlockPaste()
 */
EXPORT int EdBlockPaste(int which)
{	PASTE *	pp;
	FTABLE *	fp;

     fp = ft_CurrentDocument();
	if ((pp = bl_getpaste(which)) != 0) {
		if ((GetConfiguration()->options & O_HIDE_BLOCK_ON_CARET_MOVE) && _chkblk(fp)) {
			EdBlockDelete(0);
		}
		return paste(pp,0);
	}

	return 0;
}

/*----------------------------*/
/* EdBlockRead()			*/
/*----------------------------*/
EXPORT int _EdBlockRead(char *fn)
{	PASTE	pbuf;
	char 	fname[256];
	int		ret;

	if (!ft_CurrentDocument()) 
		return 0;
	if (fn == 0) {
		if (!txtfile_select(MREADF,fname))
			return 0;
		fn = fname;
	}
	if ((ret = bl_read(fn,&pbuf,0)) != 0)
		paste(&pbuf,0);
	ln_listfree(pbuf.pln);
	return ret;
}

EXPORT int EdBlockRead(void ) { 	return _EdBlockRead((char *)0);	}
EXPORT int EdBlockWrite(void ){ 	return _EdBlockWrite((char *)0);	}

/*---------------------------------*/
/* _chkblk()					*/
/*---------------------------------*/
EXPORT int _chkblk(FTABLE *fp)
{
	if (fp == 0 || fp->blstart == 0L || fp->blend == 0L)
		return 0;
	return 1;
}

/*---------------------------------*/
/* chkblk()					*/
/*---------------------------------*/
EXPORT int chkblk(FTABLE *fp)
{
	if (_chkblk(fp) == 0) {
		ed_error(IDS_MSGNOBLOCKSELECTED);
		return 0;
	}
	return 1;
}

/*---------------------------------*/
/* CutBlock()					*/
/*---------------------------------*/
EXPORT int CutBlock(MARK *ms, MARK *me, int flg, PASTE *pp)
{
	PASTE 	_p;
	int	 	id;
	int	 	colflg = ww_blkcolomn(WIPOI(ft_CurrentDocument()));

	_p.pln = 0;
	bl_free(&_p);
	if (flg & CUT_QUERYID) {
		if ((id = GetTmplateDlg(TextForClip, ValidClipboards)) == 0) {
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
			ShowMessage(IDS_MSGCUTBLOCK,pp->size,pp->nlines);
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
	if (!chkblk(ft_CurrentDocument()))
		return 0;
	return CutBlock(ft_CurrentDocument()->blstart,ft_CurrentDocument()->blend,flg,pp);
}

/*---------------------------------*/
/* _EdBlockWrite()				*/
/*---------------------------------*/
EXPORT int _EdBlockWrite(char *fn)
{	PASTE  pbuf;
	char   fname[256];
	int	  ret;

	if (EdBlockCut(CUT_USEPP,&pbuf)) {
		if (fn != 0) {
			ret = F_NORMOPEN;
		} else {
			ret = txtfile_select(MWRITEF,fname);
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
{	register	FTABLE *fp = ft_CurrentDocument();
	register	LINE *lp;
	register	long ln = -1L;

	lp = fp->caret.linePointer;
	while(lp) {		    /* make the whole thing faster: test (lp) ! */ 
		ln++, lp = lp->prev;
	}
	fp->ln = ln;
	WIPOI(ft_CurrentDocument())->ln = ln;
}

/*------------------------------
 * EdBlockCopy_mv()
 * Copy or move a block.
 *----------------------------*/
EXPORT int EdBlockCopy_mv(BOOL move) {	
	LINE   *lp,*ls,*le;
	int	  cs,ce,offs;
	long   delta,dln;
	PASTE  pbuf;
	FTABLE *fp;
	MARK   *bstart,*bend;
	int	  move_nocolblk,ret,colflg;

	fp = ft_CurrentDocument();
	colflg = ww_blkcolomn(WIPOI(ft_CurrentDocument()));

	if (!chkblk(fp))
		return 0;

	/*
	 * due to positioning problems afterwards: expand destination line
	 */
	if (colflg) {
		offs = caret_lineOffset2screen(fp, &fp->caret);
		if (condexpline(fp,fp->caret.linePointer) == 0)
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
nomove:				ed_error(IDS_MSGBADBLOCKMOVE);
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
			curpos((long)(fp->ln+dln),(long)(offs + delta));
			Pastehide(0);
			EdBlockMark(MARK_START);
			fp->blstart->lc--;
			upd_lines();
		}
		if (colflg) {
			delta = fp->blcol2 - fp->blcol1;
			dln	 = ln_cnt(bstart->lm,bend->lm) - 1;
			Pastehide(0);
		} else {
			offs = fp->caret.offset;
		}

		ret = pasteblk(&pbuf,colflg,offs,move);
		if (colflg) {
			curpos(fp->ln,(long)offs);
			fp->blcol1 = offs;
			fp->blcol2 = offs + delta;
			markblk(fp,fp->caret.linePointer,offs,
				ln_relative(fp->caret.linePointer,dln),fp->blcol2);
		}
		RedrawTotalWindow(fp);
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
		newln = ln_indexOf(ft_CurrentDocument(),mp->lm);
		newcpos(newln,(long)mp->lc);
		return 1;
	} else {
		ed_error(IDS_MSGNOBLOCKSELECTED);
		return 0;
	}
}

/*---------------------------------*/
/* EdBlockFindEnd() 				*/
/*---------------------------------*/
EXPORT void EdBlockFindEnd(void)
{
	if (ft_CurrentDocument()) blfin(ft_CurrentDocument()->blend);
}

/*---------------------------------*/
/* EdBlockFindStart()				*/
/*---------------------------------*/
EXPORT int EdBlockFindStart()
{
	if (ft_CurrentDocument()) return blfin(ft_CurrentDocument()->blstart);
	return 0;
}

/*----------------------------*/
/* EdBlockDelete()			*/
/*----------------------------*/
EXPORT int EdBlockDelete(int bSaveTrash)
{
	MARK		ms;
	MARK		me;
	FTABLE 	*	fp;
	int 		ret;
	
	fp = ft_CurrentDocument();
	if (!chkblk(fp))
		return 0;
	ms = *fp->blstart;
	me = *fp->blend;
	EdBlockFindStart();
	Pastehide(0);
	if (bSaveTrash) {
		ret = undoenq(ms.lm,me.lm,ms.lc,me.lc,1);
	} else {
		ret = bl_delete(fp, ms.lm, me.lm, ms.lc, me.lc, 1, 0);
	}
	RedrawTotalWindow(fp);
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

	if (bp == 0 ||ft_CurrentDocument() == 0L) return 0;
	ls = ft_CurrentDocument()->caret.linePointer;
	if ((le = ln_relgo(ft_CurrentDocument(),n)) == 0) return 0;
	
	if (bl_cut(bp,ls,le,0,(P_EQ(ls,le)) ? ls->len : 0, 0, 0)) {
		clp_setmine();
		return 1;
	}
	return 0;
}

/*---------------------------------*/
/* blborders() 				*/
/*---------------------------------*/
#if 0
int blborders(int x,int y,GRECT *gp)
{	register WINFO *wp;
	register LINE *lp;
	register int  start,end;
	FTABLE *fp;
	int colflg;

	fp = ft_CurrentDocument();
	wp = WIPOI(fp);
	colflg = ww_blkcolomn(wp);

	if (fp && fp->blstart && fp->blend) {
		lp = ln_gotouserel(fp,wp->minln);
		start = 0;
		while(!(lp->lflg & LNCPMARKED)) {
			start += wp->cheight;
			if (start > wp->cmx || (lp = lp->next) == 0L) return 0;
		}
		end = start;
		while (lp->lflg & LNCPMARKED) {
			end += wp->cheight;
			if (end > wp->cmy || (lp = lp->next) == 0L) break;
		}
		gp->g_y = start;
		gp->g_h = end - start;
		if (colflg) {
			start = fp->blcol1;
			end	 = fp->blcol2;
dragprt:		if (start < wp->mincol) 
				start = wp->mincol;
			end	= (end <= wp->maxcol + 1) ? end : wp->maxcol + 1;
			end -= start;
			start -= (int ) wp->mincol;
			gp->g_x = wp->rdarea.g_x + start * wp->cwidth;
			gp->g_w = end * wp->cwidth;
		} else {
			if (gp->g_h == wp->cheight) {
				lp	 = lp->prev;
				start = cphys2scr(lp->lbuf,fp->blstart->lc);
				end	 = cphys2scr(lp->lbuf,fp->blend->lc);
				goto dragprt;
			}
			gp->g_x = wp->rdarea.g_x;
			gp->g_w = wp->rdarea.g_w;
		}
		return (ingrect(gp,x,y));
	}
	return 0;
}
#else
EXPORT int blborders(int x,int y)
{
	return 1;
}
#endif

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

	fp  = ft_CurrentDocument();
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
		ln_addFlag (lpFirst, lpLast, LNREPLACED);
	} else for (ln = wp->minln; lp && ln <= wp->maxln; lp = lp->next, ln++) {
		if ((P_EQ(lp,lpolds) && !P_EQ(lp, lp1)) || 
		    (P_EQ(lp,lpolde) && !P_EQ(lp, lp2)) ||
		    (P_EQ(lp,lp1) && markc == MARKSTART) ||
		    (P_EQ(lp,lp2) && markc == MARKEND) || 
		    flags[ln-wp->minln] != lp->lflg)
			lp->lflg |= LNREPLACED;
	}

	p_redraw();
	ln_removeFlag(lpFirst, lpLast, LNREPLACED);
}

/*---------------------------------*/
/* markcolomns()				*/
/*---------------------------------*/
static void _markcolomns(FTABLE *fp)
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
 * markcolomns()
 */
EXPORT int markcolomns(FTABLE *fp)
{
	_markcolomns(fp);
	RedrawTotalWindow(fp);
	return 1;
}

/*---------------------------------*/
/* blcolcheck()				*/
/*---------------------------------*/
EXPORT int blcolcheck(int x,int y,int nx,int ny)
{	long 	ln,col1,col2;
	WINFO	*wp;

	wp = WIPOI(ft_CurrentDocument());
	if (ww_blkcolomn(wp)) {
		caret_calculateOffsetFromScreen(wp,x,y,&ln,&col1);
		caret_calculateOffsetFromScreen(wp,nx,ny,&ln,&col2);
		if (col1 <= col2) {
			ft_CurrentDocument()->blcol1 = col1;
			ft_CurrentDocument()->blcol2 = col2;
		} else {			
			ft_CurrentDocument()->blcol1 = col2;
			ft_CurrentDocument()->blcol2 = col1;
		}
		return 1;
	}
	return 0;
}

/*---------------------------------*/
/* markblk()					*/
/*---------------------------------*/
EXPORT int markblk(FTABLE *fp, LINE *lps, int cs, LINE *lpe, int ce)
{
	if (lps) 
		fp->blstart = mark_set(fp,lps,cs,MARKSTART);
	if (lpe) 
		fp->blend   = mark_set(fp,lpe,ce,MARKEND);
	if (lps && lpe) {
		ln_addFlag(lps,lpe,LNCPMARKED);
		return 1;
	}
	return 0;
}

/*---------------------------------*/
/* SetBlockMark() 				*/
/*---------------------------------*/
int SetBlockMark(FTABLE *fp, CARET *lpCaret, int flags)
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

	type = (flags & (~(MARK_COLUMN|MARK_RECALCULATE)));
	workmode = fp->documentDescriptor->workmode;

	if (type == MARK_ALL) {
		lp2 = fp->lastl->prev;
		markblk(fp,fp->firstl,0,lp2,lp2->len);
		fp->blcol1 = 0;
		fp->blcol2 = MAXLINELEN;
		RedrawTotalWindow(fp);
		return 1;
	}

	colflg = ww_blkcolomn(WIPOI(fp));

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
		} else {
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
	}

	fp->blstart = marks;
	fp->blend = marke;

	if (colflg && !(flags & MARK_COLUMN)) {
		_markcolomns(fp);
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
			redrawlinepart(fp, fp->ln, caret_lineOffset2screen(fp, &(CARET) {
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

	fp = ft_CurrentDocument();
	colflg = ww_blkcolomn(WIPOI(fp));

	EdMousePosition(0L);

	ln  = fp->ln;
	col = fp->col;

	if ((lp = ln_gotouserel(fp,ln)) == 0)
		return 0;

	EdBlockHide();

	o1 = 0;
	o2 = 0;
	if (type == MOT_SPACE || type == MOT_WORD) {
		LINE *(*func)() = (type == MOT_SPACE) ? 
			cadv_wordonly : cadv_word;

		o2 = caret_screen2lineOffset(fp, &(CARET) { lp, col });
		lp2 = (*func)(lp,&ln,&o2,1);
		o1  = o2;
		lp  = (*func)(lp,&ln,&o1,-1);
	} else if (type == MOT_PGRPH) {
		col = cparagrph(ln,1,1);
		ln  = cparagrph(col,-1,1);
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
	SetBlockMark(fp, &c1, MARK_START);
	return SetBlockMark(fp, &c2, MARK_END);
}

/*---------------------------------*/
/* EdBlockMark() 				*/
/*---------------------------------*/
EXPORT int EdBlockMark(int flags)
{	
	FTABLE *		fp;

	fp = ft_CurrentDocument();
	// for now: always consider to swap the block marks, if the end mark is placed
	// before the start mark. In fact we should introduce the concept of a selection head instead
	// of swapping marks.
	return SetBlockMark(fp, &fp->caret, flags | MARK_RECALCULATE);
}

