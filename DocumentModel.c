/*
 * DocumentModel.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * Operations on the "document model" of PKS-Edit. Maintain list of lines and the 
 * operations on these lines delete, insert, modify,....
 *
 * 										created: 10.02.87
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <string.h>
#include "pksedit.h"
#include "textblocks.h"
#include "edierror.h"
#include "linkedlist.h"
#include "syntaxhighlighting.h"
#include "markpositions.h"
#include "stringutil.h"
#include "fileutil.h"
#include "arraylist.h"

extern int 	_flushing;
extern PASTE* bl_addrbyid(int id, int insert);

// TODO: get rid of this hack.
char* _linebuf;

/*---------------------------------*/
/* ln_newbl()					*/
/* CUT&PASTE-marked line deleted	*/
/*---------------------------------*/
/* return Blockmarks modified => true */
static int  ln_newbl(WINFO* wp, MARK* mp) {
	if (wp->blstart == mp) {
		mp->lm = mp->lm->next;
		mp->lc = 0;
	}
	else if (wp->blend == mp) {
		mp->lm = mp->lm->prev;
		mp->lc = mp->lm->len;
	}
	else return 0;
	return 1;
}

/*----------------------------*/
/* ln_delmarks()			*/
/* release the marks related	*/
/* to a specific line		*/
/* lnew == 0 => free them	*/
/*----------------------------*/
static int ln_delmarks(WINFO* wp, LINE* lp)
{
	MARK* mp = wp->fmark;
	MARK* mp2 = 0;
	int  	blflg = (wp->blstart && wp->blend);

	if (blflg && wp->blstart->lm == lp && wp->blend->lm == lp) {
		mark_killSelection(wp);
		mp = wp->fmark;
		blflg = 0;
	}
	while (mp) {
		if (mp->lm == lp) {
			if (blflg && ln_newbl(wp, mp)) goto walk;
			if (mp2) mp2->next = mp->next;
			else wp->fmark = mp->next;
			if (mp == wp->blstart) wp->blstart = 0;
			if (mp == wp->blend)   wp->blend = 0;
			free(mp); mp = mp->next;
		}
		else {
		walk:			mp2 = mp;
			mp = mp->next;
		}
	}
	return 1;
}

static int ln_modelChanged(WINFO* wp, MODEL_CHANGE* pChanged) {
	highlight_modelChange(wp->highlighter, pChanged);
	switch (pChanged->type) {
	case EVERYTHING_CHANGED:
		bl_hideSelection(wp, 0);
		wp->caret.linePointer = NULL;
		ll_destroy((LINKED_LIST**)&wp->fmark, (int (*)(void* elem))0);
		wp->lpMinln = 0;
		break;
	case LINE_MODIFIED: {
		if (wp->lpMinln == pChanged->lp) {
			wp->lpMinln = 0;
		}
		MARK* mp = wp->fmark;
		while (mp) {
			if (mp->lm == pChanged->lp) {
				/* if (mp != wp->blstart && mp != wp->blend) { */
				if (mp->lc >= pChanged->col1) mp->lc += pChanged->len;
				else if (mp->lc > pChanged->col2) mp->lc = pChanged->col2;
				/* } */
			}
			mp = mp->next;
		}
		if (wp->blstart && wp->blend &&
			wp->blstart->lm == wp->blend->lm &&
			wp->blstart->lc >= wp->blend->lc
			)
			bl_hideSelection(wp, 0);
	}
    break;
	case LINE_REPLACED: {
			MARK* mp = wp->fmark;

			while (mp) {
				if (mp->lm == pChanged->lp) {
					mp->lm = pChanged->lpNew;
				}
				mp = mp->next;
			}
			if (wp->caret.linePointer == pChanged->lp) {
				wp->caret.linePointer = pChanged->lpNew;
			}
	}
		break;
	case LINES_JOINED: {
		MARK* mp = wp->fmark;
		int		delta = pChanged->lp->len;

		while (mp) {
			if (mp->lm == pChanged->lpNew) {
				mp->lm = pChanged->lp;
				mp->lc += delta;
			}
			mp = mp->next;
		}
	}
	break;
	case LINE_SPLIT: {
			MARK* mp = wp->fmark;
			LINE* lnext = pChanged->lpNew;
			LINE* lp = pChanged->lp;

			while (mp) {
				if (mp->lm == lp) {
					if (mp->lc >= pChanged->col1) {
						mp->lc -= pChanged->col1;
						mp->lm = lnext;
					}
				}
				mp = mp->next;
			}
		}
		break;
	case LINE_DELETED:
		ln_delmarks(wp, pChanged->lp);
		// drop through
	case LINE_INSERTED:
		if (wp->caret.linePointer == pChanged->lp) {
			wp->caret.linePointer = pChanged->lpNew;
		}
		break;
	}
	wp->maxVisibleLineLen = -1;
	return 1;
}

/*
 * Invoked, when a line pointer was replaced / inserted / deleted. Update all dependents on that particular
 * line.
 */
static void ln_singleLineChanged(FTABLE* fp, MODEL_CHANGE_TYPE type, LINE* lp, LINE* pNewLine) {
	MODEL_CHANGE param = {type, fp};
	param.lp = lp;
	param.lpNew = pNewLine;
	ft_forAllViews(fp, ln_modelChanged, &param);
}

/*------------------------------------------------------------
 * ft_bufdestroy().
 * Release all resources associated with a file. Do not close associated views.
 */
void ft_bufdestroy(FTABLE* fp) {
	destroy(&fp->documentDescriptor);
	ln_listfree(fp->firstl);
	fp->tln = fp->firstl = 0;
	file_closeFile(&fp->lockFd);
	undo_destroyManager(fp);
	MODEL_CHANGE change = {EVERYTHING_CHANGED, fp};
	ft_forAllViews(fp, ln_modelChanged, &change);
}

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
	error_showErrorById(IDS_MSGLINETOOLONG); 
}

/*---------------------------------*/
/* invalidhiddenop()			*/
/*---------------------------------*/
static void invalidhiddenop() {
	error_showErrorById(IDS_MSGINVALIDHIDEOP); 
}

/*----------------------------*/
/* ln_insert() 			*/
/* insert a line at the given */
/* position				*/
/*----------------------------*/
void ln_insert(FTABLE *fp, LINE *pos, LINE *lp) {
	ft_setFlags(fp, fp->flags | F_CHANGEMARK);
	if (fp->documentDescriptor->nl < 0) {
		lp->lflg |= LNNOTERM;
	}

	fp->nlines++;
	lp->next = pos;
	if ((lp->prev = pos->prev) == 0) 			/* insert first line    */
		fp->firstl = lp;
	else lp->prev->next = lp;				/* insert another	    */
	pos->prev = lp;
	ln_singleLineChanged(fp, LINE_INSERTED, pos, lp);
	undo_saveOperation(fp, lp, pos, O_INSERT);
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
	ft_setFlags(fp, fp->flags | F_CHANGEMARK);
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
	ln_singleLineChanged(fp, LINE_DELETED, lp, next);
	fp->nlines--;
	return 1;
}

/*---------------------------------
 * ft_cutMarkedLines()
 * Cut out the lines which which have a line marker flag.
 * The cut operation is one of the MLN_... constants defined for files.
 *---------------------------------*/
void ft_cutMarkedLines(FTABLE *fp, int op)
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
LINE *ln_break(FTABLE *fp, LINE *linep, int col) {
	LINE *	nlp = 0;
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
		MODEL_CHANGE change = {LINE_SPLIT, fp};
		change.col1 = col;
		change.lp = linep;
		change.lpNew = nlp;
		ft_forAllViews(fp, ln_modelChanged, &change);
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
		log_vsprintf("error in ln_join");
#endif
		return 0;
	}

	memmove(&lp1->lbuf[olen],lp2->lbuf,lp2->len); 
	lp1->lbuf[len] = 0;

	if (bRemove) {
		MODEL_CHANGE change = { LINES_JOINED, fp };
		change.lp = lp1;
		change.lpNew = lp2;
		ft_forAllViews(fp, ln_modelChanged, &change);
		lp1->lflg |= lp2->lflg;
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
/* goto line l in File and return the corresponding line pointer */
/*----------------------------*/
LINE *ln_goto(FTABLE *fp, long l)
{	LINE *cl;

	if (l < 0 || l >= fp->nlines) {
		return 0L;
	}
	if (l > fp->nlines / 2) {
		cl = fp->lastl;
		l = fp->nlines - l;
		while (l > 0) {
			if ((cl = cl->prev) == NULL) {
				break;
			}
			l--;
		}
	} else {
		cl = fp->firstl;
		while (l > 0) {
			if ((cl = cl->next) == NULL) {
				break;
			}
			l--;
		}
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

/*----------------------------*/
/* ln_replace()			*/
/*----------------------------*/
void ln_replace(FTABLE *fp, LINE *oln, LINE *nl)
{
	nl->next = oln->next;
	if ((nl->prev = oln->prev) != 0) nl->prev->next = nl;
	nl->next->prev = nl;
	ln_singleLineChanged(fp, LINE_REPLACED, oln, nl);
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

		memset(lptmp,0,sizeof(LINE));
	}
	return lptmp;
}

/*---------------------------------*/
/* ln_modify()					*/
/*---------------------------------*/
LINE *ln_modify(FTABLE *fp, LINE *lp, int col1, int col2) {
	int		len;
	int		lplen;
	int		copylen;
	char *	s;

	if (lp != fp->tln) {
		if ((lp = ln_settmp(fp,lp,(LINE**)&s)) == 0L) {
			return 0;
		}
	}
	ft_setFlags(fp, fp->flags | F_CHANGEMARK);
	fp->lastmodoffs = col2;

	/* little to do */
	if ((len = col2 - col1) == 0) {
		return lp;
	}

	lplen = lp->len;
	copylen = lplen - col1;
	lplen += len;
	if (lplen > MAXLINELEN) {		/* genau �berpr�fen	 		*/
		linetoolong();
		return 0;
	}
	lp->len = lplen;

	s = lp->lbuf;
	memmove(s+col2,s+col1,copylen);
	s[lplen] = 0;
	MODEL_CHANGE change = {LINE_MODIFIED, fp};
	change.lp = lp;
	change.col1 = col1;
	change.col2 = col2;
	change.len = len;
	ft_forAllViews(fp, ln_modelChanged, &change);
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
		ln_singleLineChanged(fp, LINE_REPLACED, lp, lpind);
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

	ln_singleLineChanged(fp, LINE_REPLACED, lpind, lp1);
	free(lpind);
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
	free(lp);
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
unsigned char* ln_createFromBuffer(FTABLE* fp, EDIT_CONFIGURATION* documentDescriptor, unsigned char* p, unsigned char* pend) {
	int  nl;
	long size;
	int rightMargin = documentDescriptor->rmargin;

	nl = 0;
	if (rightMargin <= 0)
		rightMargin = 1;
	while (p < pend) {
		size = (long)(pend - p);
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
unsigned char* ln_createMultipleLinesFromBuffer(FTABLE* fp, EDIT_CONFIGURATION* documentDescriptor, unsigned char* p, unsigned char* pend) {
	unsigned char 		t1;

	t1 = documentDescriptor->nl;
	*pend = t1;
	return ln_createMultipleLinesUsingSeparators(fp, p, pend, t1, documentDescriptor->nl2, documentDescriptor->cr);
}

/*-----------------------------------------
 * ln_createMultipleLinesUsingSeparators()
 * Split a passed buffer (end is marked by pend) with given line separator
 * characters t1 and t2. If cr is not 0, treat it as a secondary line separator.
 */
unsigned char* ln_createMultipleLinesUsingSeparators(FTABLE* fp, unsigned char* p,
	unsigned char* pend, int t1, int t2, int cr) {
	int 			flags;
	int				nl;
	size_t			len;
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
		if (!ln_createAndAdd(fp, q, (int)len, flags)) {
			return 0;
		}
		nl++;
	}
}
