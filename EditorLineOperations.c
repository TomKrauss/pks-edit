/*
 * EditorLineOperations.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * maintain list of lines: delete, insert, modify,....
 *
 * 										created      : 10.02.87
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include <string.h>
#include "pksedit.h"
#include "textblocks.h"
#include "edierror.h"

extern int 	_flushing;
char* _linebuf;
extern PASTE *	bl_addrbyid(int id, int insert);

LINE *	ln_settmp(FTABLE *fp,LINE *lp,LINE **lpold);
LINE *	ln_modify(FTABLE *fp,LINE *lp,int col1,int col2);

/*---------------------------------
 * ln_addFlag()
 * 
 * Add a flag to all lines between lpstart and lpend. 
 */
void ln_addFlag(LINE *lpstart, LINE *lpend, int flg) {

	while (lpstart != 0) {	
		lpstart->lflg |= flg;
		if (lpstart == lpend) return;
		lpstart = lpstart->next;
	}
}

/*---------------------------------
 * ln_changeFlag()
 *
 * Add / remove flags from multiple lines - all lines having an expected flag
 * are considered.
 */
void ln_changeFlag(LINE *lpstart, LINE *lpend, int flagsearch, int flagmark, int set) {
	if (!set)
		flagmark = ~flagmark;

	while (lpstart != 0) {	
		if (lpstart->lflg & flagsearch) {
			if (set) {
				lpstart->lflg |= flagmark;
			} else {
				lpstart->lflg &= flagmark;
			}
		}
		if (lpstart == lpend) return;
		lpstart = lpstart->next;
	}
}


/*---------------------------------
 * ln_removeFlag()
 *
 * Remove a flag from all lines between lpstart and lpend.
 *---------------------------------*/
void ln_removeFlag(LINE *lpstart, LINE *lpend, int flg) {
	flg = ~flg;
	while (lpstart != 0) {
		lpstart->lflg &= flg;
		if (lpstart == lpend) return;
		lpstart = lpstart->next;
	}
}

/*---------------------------------*/
/* linetoolong()				*/
/*---------------------------------*/
void linetoolong(void) {
	ed_error(IDS_MSGLINETOOLONG); 
}

/*---------------------------------*/
/* invalidhiddenop()			*/
/*---------------------------------*/
static void invalidhiddenop() {
	ed_error(IDS_MSGINVALIDHIDEOP); 
}

/*---------------------------------*/
/* ln_newbl()					*/
/* CUT&PASTE-marked line deleted	*/
/*---------------------------------*/
/* return Blockmarks modified => true */
static int  ln_newbl(FTABLE *fp, MARK *mp) {	
	if (fp->blstart == mp) {
		mp->lm = mp->lm->next; 
		mp->lc = 0; 
	} else if (fp->blend == mp) {
		mp->lm = mp->lm->prev; 
		mp->lc = mp->lm->len;
	} else return 0;
	mp->lm->lflg |= LNMARKED;
	return 1;
}

/*----------------------------*/
/* ln_delmarks()			*/
/* release the marks related	*/
/* to a specific line		*/
/* lnew == 0 => free them	*/
/*----------------------------*/
static void ln_delmarks(FTABLE *fp, LINE *lp)
{
	MARK *	mp = fp->fmark;
	MARK *	mp2=0;
	int  	blflg = (fp->blstart && fp->blend);

	if (blflg && fp->blstart->lm == lp && fp->blend->lm == lp) {
		mcp_release(fp);
		mp = fp->fmark;
		blflg = 0;
	}
	while (mp) {
		if (mp -> lm == lp) {
			if (blflg && ln_newbl(fp,mp)) goto walk;
			if (mp2) mp2->next = mp->next;
			else fp->fmark = mp->next;
			if (mp == fp->blstart) fp->blstart = 0;
			if (mp == fp->blend)   fp->blend = 0;
			_free(mp); mp = mp->next;
		} else {
walk:			mp2 = mp;
				mp = mp->next;
		}
	}
}

/*----------------------------*/
/* ln_insert() 			*/
/* insert a line at the given */
/* position				*/
/*----------------------------*/
void ln_insert(FTABLE *fp, LINE *pos, LINE *lp) {
	fp->flags |= F_CHANGEMARK;
	if (fp->documentDescriptor->nl < 0) {
		lp->lflg |= LNNOTERM;
	}

	fp->nlines++;
	if (fp->caret.linePointer == pos)
		fp->caret.linePointer = lp;

	lp->next = pos;
	if ((lp->prev = pos->prev) == 0) 			/* insert first line    */
		fp->firstl = lp;
	else lp->prev->next = lp;				/* insert another	    */
	pos->prev = lp;
	undo_saveOperation(fp, lp, pos, O_INSERT);

	if (pos->lflg & LNCPMARKED) {
		if (fp->blstart->lm != pos) {
			lp->lflg |= LNCPMARKED;
		}
	}
}

/*----------------------------*/
/* ln_delete() 			*/
/* remove a line from the list*/
/*----------------------------*/
int ln_delete(FTABLE *fp, LINE *lp)
{
	LINE *next;

	if (!lp || (next = lp->next) == 0L) {
		return 0;
	}

	undo_saveOperation(fp, lp, next, O_DELETE);
	fp->flags |= F_CHANGEMARK;
	if (lp -> lflg & LNMARKED) {
		ln_delmarks(fp,lp);
	}
	if (!lp->prev) {
		if (next == fp->lastl) {
			return 0;
		}
		next->prev = 0;
		fp->firstl = next;
	} else {
		next->prev = lp->prev;
		lp->prev->next = next;
	}
	if (lp == fp->tln) {
		fp->tln = 0;
	}
	if (fp->caret.linePointer == lp) {
		fp->caret.linePointer = next;
	}
	fp->nlines--;
	return 1;
}

/*---------------------------------*/
/* mln_cutlines()				*/
/*---------------------------------*/
void mln_cutlines(FTABLE *fp, int op)
{	LINE *		lp;
	LINE *		lpnext;
	PASTE *		pb;

	pb = (op == MLN_DELETE) ? _undobuf : bl_addrbyid(0, 0);

	lp = fp->firstl;
	bl_free(pb);
	do {
		lpnext = lp->next;
		if (lp->lflg & LNXMARKED) {

			if (lpnext == 0) {
				return;
			}

			if (lpnext->next == 0) {
				bl_append(pb,lp,lp,0,lp->len);
				if (op == MLN_DELETE) {
					lp->len   = 0;
					lp->lbuf[0] = 0;
					lp->lflg &= ~LNXMARKED;
				}
				return;
			}

			if (!bl_append(pb,lp,lpnext,0,0)) {
				return;
			}

			if (op == MLN_DELETE) {
				ln_delete(fp,lp);
			}

		}
		lp = lpnext;
	} while(lp != 0);
}

/*--------------------------------------------------------------------------
 * ln_deepcopy()
 * copy lines to a buffer, recursive copy indirect line lists too
 */
LINE *ln_deepcopy(LINE *lp, int physize, int start, int end)
{
	LINE *		lpList;
	LINE *		lpDest;
	LINE *		lpStart;
	LINE *		lpTmp;

	if (!LpHasHiddenList(lp)) {

		/* no recursive structure: return shallow copy */
		return ln_cut(lp, physize, start, end);
	}

	lpList = LpIndHiddenList(lp);
	lpStart = 0;
	while(lpList) {
		lpTmp = ln_deepcopy(lpList, lpList->len, 0, lpList->len);
		if (lpStart == 0) {
			lpStart = lpDest = lpTmp;
		} else {
			lpDest->next = lpTmp;
			lpTmp->prev = lpDest;
			lpDest = lpTmp;
		}
		lpList = lpList->next;
	}

	lpDest = ln_cut(lp, sizeof(LINEINDIRECT), 0, sizeof(LINEINDIRECT));
	lpDest->len = 0;
	lpDest->lflg |= LNINDIRECT;
	LpIndHiddenList(lpDest) = lpStart;
	return lpDest;
}

/*--------------------------------------*/
/* ln_break()						*/
/* break a line into two pieces		*/
/*--------------------------------------*/
static void ln_breakmarks(FTABLE *fp,LINE *lp,LINE *lnext,int col)
{	MARK *	mp = fp->fmark;
	int 		flg;

	lp->lflg &= ~LNMARKED;
	flg = lp->lflg;
	while (mp) {
		if (mp->lm == lp) {
			if (mp->lc >= col) {
				mp->lc -= col;
				mp->lm = lnext;
				lnext->lflg |= LNMARKED;
				if (mp->mchar == MARKSTART) {
					lp->lflg 	  &= ~LNCPMARKED;
					lnext->lflg |= flg;
				}
			}  else {
				if (mp->mchar == MARKEND) 
					flg &= ~LNCPMARKED;
				lp->lflg |= LNMARKED;
			}
		}
		mp = mp->next;
	}
	lnext->lflg |= flg;
}

LINE *ln_break(FTABLE *fp, LINE *linep, int col)
{	LINE *	nlp = 0;
	LINE *	lp;
	int  	len;

	len = linep->len-col;

	/* to have linep in a register */
	lp = linep;
	if ((nlp = ln_settmp(fp,(LINE *) 0L,&lp)) != 0L) {
		linep = lp;
		nlp->len = len;
		nlp->lflg = linep->lflg & (LNNOTERM|LNNOCR);
		len++;
		memmove(nlp->lbuf,&linep->lbuf[col],len);
		if (linep->lflg & LNMARKED) 
			ln_breakmarks(fp,linep,nlp,col);
		ln_insert(fp,linep->next,nlp);

		if ((linep = ln_modify(fp,linep,linep->len,col)) != 0L) {
			return linep->next;
		}

	}
	return 0;
}

/*---------------------------------*/
/* ln_split()					*/
/*---------------------------------*/
LINE *ln_split(FTABLE *fp, LINE *lc, int pos2, int pos1)
{
	if ((lc = ln_break(fp,lc,pos2)) != 0L)
     	lc = ln_modify(fp,lc->prev,pos2,pos1);
	return lc;
}

/*--------------------------------------------------------------------------
 * ln_joinmarks()
 */
static void ln_joinmarks(FTABLE *fp, LINE *lp1, LINE *lp2)
{	MARK *	mp = fp->fmark;
	int		delta = lp1->len;

	while (mp) {
		if (mp->lm == lp2) {
			mp->lm = lp1;
			mp->lc += delta;
		}
		mp = mp->next;
	}
	lp1->lflg |= lp2->lflg;
}

/*--------------------------------------------------------------------------
 * ln_join()
 * join line1 and line2
 * bRemove: remove old line from files linelist ?
 */
LINE *ln_join(FTABLE *fp, LINE *lp1, LINE *lp2, int bRemove)
{	int 		len;
	int		olen;

	if (lp2 == fp->lastl) {
		return 0;
	}

	olen = lp1->len;

	if (lp2->lflg & LNINDIRECT) {
		if (olen || !bRemove) {
			invalidhiddenop();
			return 0;
		}
		ln_delete(fp,lp1);
		return lp2;
	} else if ((len = olen + lp2->len) > MAXLINELEN) {
		linetoolong();
		return 0;
	}

	if (lp1 != fp->tln && (lp1 = ln_settmp(fp,lp1,&lp2)) == 0L) {
#ifdef DEBUG
		edidebug("error in ln_join");
#endif
		return 0;
	}

	memmove(&lp1->lbuf[olen],lp2->lbuf,lp2->len); 
	lp1->lbuf[len] = 0;

	if (bRemove) {
		if (lp2->lflg & LNMARKED) ln_joinmarks(fp,lp1,lp2);
		ln_delete(fp,lp2);
	}
	lp1->len = len;			/* modify lp1->len after joinmarks !!	*/
	lp1->lflg = (lp1->lflg & (~LNNOCR|LNNOTERM)) | (lp2->lflg & (LNNOCR|LNNOTERM));
	return lp1;
}

/*---------------------------------*/
/* lnjoin_lines()				*/
/*---------------------------------*/
int lnjoin_lines(FTABLE *fp)
{
	LINE *	lp;
	LINE	*	lpnext;
	int  	flg;

	lp = fp->firstl;
	do {
		lpnext = lp->next;
		if (lp->lflg & LNXMARKED) {
			if (lpnext == 0 || lpnext->next == 0)
				return 1;
			flg = lpnext->lflg;
			if ((lp = ln_join(fp,lp,lp->next,1)) == 0) {
				return 0;
			}
			if ((flg & LNXMARKED) == 0) {
				lp = lp->next;
			}
		} else
			lp = lpnext;
	} while(lp != 0);
	return 1;
}

/*----------------------------*/
/* ln_goto()				*/
/* goto line deltaLines in File		*/
/*----------------------------*/
LINE *ln_goto(FTABLE *fp,long l)
{	LINE *cl = fp->firstl;

	if (l < 0 || l >= fp->nlines) return 0L;
	while (l > 0) {
		if ((cl = cl->next) == 0L) break;
		l--;
	}
	return cl;
}

/*---------------------------------*/
/* ln_relative()				*/
/*---------------------------------*/
LINE *ln_relative(LINE *cl, long l)
{
	if (l >= 0L)  
		while (l > 0 && cl) { 
			cl = cl->next; l--; 
		}
	else {
		l = -l;
		while (l && cl) { 
			cl = cl->prev; l--; 
		}
	}
	return((cl && cl->next) ? cl : 0);
}

/*---------------------------------*/
/* ln_crelgo()					*/
/*---------------------------------*/
LINE *ln_crelgo(FTABLE *fp, long deltaLines)
{	LINE *cl = fp->caret.linePointer;

	if (deltaLines > 0L) {
		while (deltaLines && cl != fp->lastl) { 
			cl   = cl->next; 
			deltaLines--;
		}
		if (cl == fp->lastl) {
			return 0;
		}
	} else {
		deltaLines = -deltaLines;
		while (deltaLines) { 
			cl = cl->prev; deltaLines--; 
			if (!cl) break;
		}
	}
	return cl;
}

/*----------------------------*/
/* ln_relgo()				*/
/* relativ goto line deltaLines in File*/
/*----------------------------*/
LINE *ln_relgo(FTABLE *fp, long l)
{
	return ln_relative(fp->caret.linePointer,l);
}

/*----------------------------*/
/* ln_gotouserel()			*/
/* goto, using relativ mv	*/
/*----------------------------*/
LINE *ln_gotouserel(FTABLE *fp, long ln)
{
	return ln_relative(fp->caret.linePointer,ln-fp->ln);
}

/*---------------------------------*/
/* ln_cnt()					*/
/* Anzahl Zeilen in einer Liste	*/
/* zaehlen					*/
/*---------------------------------*/
long ln_cnt(LINE *lps,LINE *lpe)
{	register long nl = 1;

	while (lps != lpe) {
		if (lps == 0) return 0;
		nl++;
		lps = lps->next;
	}
	return nl;
}

/*---------------------------------*/
/* ln_findbit()				*/
/*---------------------------------*/
LINE *ln_findbit(LINE *lp, int bit)
{
	while(lp != (LINE *) 0) {
		if (lp->lflg & bit)
			break;
		lp = lp->next;
	}
	return lp;
}

/*---------------------------------*/
/* ln_repmark()				*/
/*---------------------------------*/
static void ln_repmark(FTABLE *fp, LINE *oldln, LINE *newln)
{
	MARK *mp = fp->fmark;

	while (mp) {
		if (mp->lm == oldln) mp->lm = newln;
		mp = mp->next;
	}
}

/*----------------------------*/
/* ln_replace()			*/
/*----------------------------*/
void ln_replace(FTABLE *fp, LINE *oln, LINE *nl)
{
	nl->next = oln->next;
	if ((nl->prev = oln->prev) != 0) nl->prev->next = nl;
	nl->next->prev = nl;
	if (oln -> lflg & LNMARKED) ln_repmark(fp,oln,nl);
	if (fp->caret.linePointer == oln)  fp->caret.linePointer  = nl;
	if (fp->firstl == oln) fp->firstl = nl;
}

/*---------------------------------*/
/* ln_settmp()					*/
/*---------------------------------*/
LINE *ln_settmp(FTABLE *fp,LINE *lp,LINE **lpold)
{
	LINE *	lptmp;
	LINE *	lp1;

	if (lp && (lp->lflg & LNINDIRECT)) {
		invalidhiddenop();
		return 0;
	}

	if ((lptmp = fp->tln) == 0L) {

		/* no current edit line, so create one	*/

		if ((lptmp = ln_create(FBUFSIZE)) == 0L) {
			return 0;
		}
		lptmp->len = 0;
		fp->tln = lptmp;
	} else {

		/* release current edit line */

		if ((lp1 = ln_cut(lptmp,lptmp->len+1,0,lptmp->len)) == 0L) 
			return 0;
		if (*lpold == lptmp) {
			*lpold = lp1;
		}
		ln_replace(fp,lptmp,lp1);
		lp1->lflg = lptmp->lflg;
	}

	if (lp) {				

		/* create copy of line for modify */

		memmove(lptmp->lbuf,lp->lbuf,lp->len);	/* fast memmove */
		lptmp->len = lp->len;
		lptmp->attr = lp->attr;
		lptmp->lbuf[lp->len] = 0;
		lptmp->lflg = lp->lflg;
		ln_replace(fp,lp,lptmp);	  /* replace old line */
		undo_saveOperation(fp, lp, lptmp->prev, O_MODIFY);

	} else {
		/* create a temp line for several purposes 	*/

		blfill(lptmp,sizeof(LINE),0);
	}
	return lptmp;
}

/*---------------------------------*/
/* ln_modify()					*/
/*---------------------------------*/
LINE *ln_modify(FTABLE *fp, LINE *lp, int col1, int col2)
{
	int		len;
	int		lplen;
	int		copylen;
	MARK *	mp;
	char *	s;

	if (lp != fp->tln) {
		if ((lp = ln_settmp(fp,lp,(LINE**)&s)) == 0L) {
			return 0;
		}
	}
	fp->flags |= F_CHANGEMARK;
	fp->lastmodoffs = col2;

	/* little to do */
	if ((len = col2 - col1) == 0) {
		return lp;
	}

	lplen = lp->len;
	copylen = lplen - col1;
	lplen += len;
	if (lplen > MAXLINELEN) {		/* genau ÅberprÅfen	 		*/
		linetoolong();
		return 0;
	}
	lp->len = lplen;

	s = lp->lbuf;
	memmove(s+col2,s+col1,copylen);
	s[lplen] = 0;

	if (lp->lflg & LNMARKED) {
		mp = fp->fmark;
		while (mp) {
			if (mp->lm == lp) {
				/* if (mp != fp->blstart && mp != fp->blend) { */
					if (mp->lc >= col1) mp->lc += len;
					else if (mp->lc > col2) mp->lc = col2;
				/* } */
			}
			mp = mp->next;
		}
		if (fp->blstart && fp->blend &&
		    fp->blstart->lm == fp->blend->lm && 
		    fp->blstart->lc >= fp->blend->lc
		   )
			bl_hideSelection(0);			   
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * ln_hide()
 */
LINE *ln_hide(FTABLE *fp, LINE *lp1, LINE *lp2)
{
	LINE *		lpind;
	LINE *		lp;
	long			nHidden;
	long			nTotalHidden;

	if ((lpind = ln_create(sizeof(LINEINDIRECT))) == 0) {
		return 0;
	}

	LpIndirectTyp(lpind) = LI_HIDDENLIST;
	lpind->lflg = LNINDIRECT;
	lpind->len = 0;
	lpind->next = lp2->next;
	lpind->prev = lp1->prev;
	LpIndHiddenList(lpind) = lp1;
	LpIndNHidden(lpind) = nHidden = ln_cnt(lp1,lp2);
	fp->nlines += (1 - nHidden);

	lpind->next = lp2->next;
	lp2->next->prev = lpind;
	if ((lpind->prev = lp1->prev) == 0) {
		fp->firstl = lpind;
	} else {
		lp1->prev->next = lpind;
	}
	lp2->next = 0;
	undo_saveOperation(fp, lpind, (LINE*)0, O_HIDE);

	for (lp = lp1, nTotalHidden = 0; lp; lp = lp->next) {
		if (fp->caret.linePointer == lp) {
			fp->caret.linePointer = lpind;
		}
		if (LpHasHiddenList(lp)) {
			nTotalHidden += LpIndNTotal(lp);
		} else {
			nTotalHidden++;
		}
		if (lp == lp2) {
			break;
		}
	}
	LpIndNTotal(lpind) = nTotalHidden;

	return lpind;
}

/*--------------------------------------------------------------------------
 * ln_unhide()
 */
int ln_unhide(FTABLE *fp, LINE *lpind) {
	LINE *		lp1;
	LINE *		lp2;
	long			nHidden;

	if ((lpind->lflg & LNINDIRECT) == 0 ||
		LpIndirectTyp(lpind) != LI_HIDDENLIST) {
		return 0;
	}

	lp1 = LpIndHiddenList(lpind);
	for (lp2 = lp1; lp2->next; lp2 = lp2->next)
		;

	if (lpind->lflg & LNCPMARKED) {
		ln_addFlag(lp1,lp2,LNCPMARKED);
	} else {
		ln_removeFlag(lp1,lp2,LNCPMARKED);
	}

	nHidden = LpIndNHidden(lpind);
	fp->nlines += (nHidden - 1);

	if ((lp1->prev = lpind->prev) == 0) {
		fp->firstl = lp1;
	} else {
		lp1->prev->next = lp1;
	}

	lp2->next = lpind->next;
	lp2->next->prev = lp2;

	undo_saveOperation(fp, lp1, lp2, O_UNHIDE);

	if (fp->caret.linePointer == lpind) {
		fp->caret.linePointer = lp1;
	}

	_free(lpind);
	return 1;

}

/*--------------------------------------------------------------------------
 * ln_destroy()
 */
void ln_destroy(LINE *lp)
{
	if (lp->lflg & LNINDIRECT) {
		if (LpIndirectTyp(lp) == LI_HIDDENLIST) {
			ln_listfree(LpIndHiddenList(lp));
		}
	}
	_free(lp);
}

/*------------------------------------------------------------
 * ln_needbytes()
 * Calculates the number of bytes needed for one line.
 */
EXPORT long ln_calculateMemorySizeRequired(LINE *lp, int nl, int cr)
{ 	
	long fsize = 0L;
	long ln = 0L;

	/* test for lp quicker (than for lpnext), subtract len(lastl) */
	if (lp) do { 
		fsize += lp->len; 
		lp = lp->next; 
		ln++;
	} while (lp);

	if (nl >= 0) {
		fsize += ln;
		if (cr >= 0)
			fsize += ln;
	}
	return fsize;
}


/*---------------------------------
 * ln_createFromBuffer()
 * Creates a line from a buffer p. The end of the buffer
 * is passed in pend.
 */
unsigned char* ln_createFromBuffer(FTABLE* fp, DOCUMENT_DESCRIPTOR* documentDescriptor, unsigned char* p, unsigned char* pend) {
	int  nl;
	long size;
	int rightMargin = documentDescriptor->rmargin;

	nl = 0;
	if (rightMargin <= 0)
		rightMargin = 1;
	while (p < pend) {
		size = pend - p;
		if (size >= rightMargin) {
			size = rightMargin;
		}
		else if (!_flushing) {
			break;
		}
		if (!ln_createAndAdd(fp, p, size, LNNOTERM)) {
			return 0;
		}
		p += size;
		nl++;
	}
	fp->nlines += nl;
	return p;
}

/*------------------------------------
 * ln_createMultipleLinesFromBuffer()
 * Create multiple lines from a passed buffer p. The
 * end of the buffer is passed in pend.
 */
unsigned char* ln_createMultipleLinesFromBuffer(FTABLE* fp, DOCUMENT_DESCRIPTOR* documentDescriptor, unsigned char* p, unsigned char* pend) {
	unsigned char 		t1;

	t1 = documentDescriptor->nl;
	*pend = t1;
	return ln_createMultipleLinesUsingSeparators(fp, p, pend, t1, documentDescriptor->nl2, documentDescriptor->cr);
}

/*-----------------------------------------
 * ln_createMultipleLinesUsingSeparators()
 * Split a passed buffer (end is marked by pend) with given line separator
 * characters t1 and t2. If cr s
 */
unsigned char* ln_createMultipleLinesUsingSeparators(FTABLE* fp, unsigned char* p,
	unsigned char* pend, int t1, int t2, int cr) {
	int 				flags;
	int				nl;
	int				len;
	unsigned char* q;
	unsigned char* pstart;

	nl = 0;
	pstart = p + 1;

	for (;;) {
		q = p;
		if (t2 >= 0) {
			while (*p != t1 && *p != t2) {
				p++;
			}
			p++;
		}
		else {
			while (*p++ != t1) {
				;
			}
		}

		len = p - q;

		if (cr >= 0 && p > pstart && p[-2] == cr) {
			len -= 2;
			flags = 0;
		}
		else {
			len--;
			flags = LNNOCR;
		}

		/* is this line too long ? */
		if (len > MAXLINELEN) {
			p = q + MAXLINELEN;
			len = MAXLINELEN;
			fp->longLinesSplit++;
			goto createline;
		}

		if (p > pend) {
			fp->nlines += nl;
			return q;
		}

	createline:
		if (!ln_createAndAdd(fp, q, len, flags)) {
			return 0;
		}
		nl++;
	}
}


