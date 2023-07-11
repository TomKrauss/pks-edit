/*
 * ColumnBlocks.c
 *
 * Cut and copy of text selection in column mode.
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author : TOM
 * created: 09.04.1992
 * 
 */

#include <windows.h>
#include <string.h>
#include "caretmovement.h"
#include "pksedit.h"
#include "edierror.h"
#include "winfo.h"

static const int TAB = 9;

/*---------------------------------*/
/* caret_copyIndent()					*/
/*---------------------------------*/
static LINE *caret_copyIndent(WINFO *wp, LINE **Lps, int cfirst, int clast,
	int freeflg, int ctrlmode)
{
	LINE *		lp;
	LINE *		lps;
	unsigned char *s,*d;
	int			i,col,len,size;
	FTABLE* fp = wp->fp;

	d = _linebuf;
	lps = *Lps;
	s   = lps->lbuf;
	len = lps->len;
	for (i = 0, col = 0; col < MAXLINELEN && i < len; i++) {
		char c;
		if ((c = *s++) == TAB && !ctrlmode) {
			size = indent_calculateTabStop(col, &wp->indentation);
			while(col < size) {
				*d++ = ' ';
				col++;
			}
		} else {
			*d++ = c, col++;
		}
	}
	if (col < cfirst) freeflg = 0;	/* alte Zeile nicht beschneiden	*/
	while (col < clast) {
		*d++ = ' ';
		col++;
	}
	size = clast-cfirst;
	if ((lp = ln_create((size))) == (LINE *) 0) return 0;
	d = &_linebuf[cfirst];
	memmove(lp->lbuf,d,size);
	if (freeflg) {
		memmove(d,&_linebuf[clast],col-cfirst);
		col -= size;
		if ((lps = ln_modify(fp,lps,len,col)) == (LINE *) 0) 
			return 0;
		memmove(lps->lbuf,_linebuf,col);
		*Lps = lps;
	}
	return lp;
}

/*----------------------------
 * bl_cutBlockInColumnMode()
 * Cut out a column shaped block of text and optionally delete it
 *----------------------------*/
EXPORT int bl_cutBlockInColumnMode(WINFO* wp, PASTE *pp,LINE *lnfirst,LINE *lnlast,int freeflg)
{
	LINE *	lpnew;
	LINE *	lpd = 0;
	LINE *	lp;
	long 	cnt;
	int     cfirst;
	int		clast;
	int		ctrl;

	lp     = lnfirst;
	cnt    = ln_cnt(lp,lnlast);
	cfirst = wp->blcol1;
	clast  = wp->blcol2;
	ctrl   = PLAINCONTROL(wp->dispmode);
	if (clast - cfirst <= 0) return 0;
	memset(pp,0,sizeof(* pp));
	while(cnt > 0) {
		if ((lpnew = caret_copyIndent(wp, &lp,cfirst,clast,freeflg,ctrl)) == (LINE *) 0) 
			return 0;
		pp->size   += lpnew->len;
		pp->nlines++;
		if (lpd) 			/* unpossible use of lpd before Definition ! */
			lpd->next = lpnew;
		else 
			pp->pln   = lpnew;
		lpnew->prev = lpd;
		lpd         = lpnew;
		lp          = lp->next;
		cnt--;
	}
	return 1;
}

/*
 * ln_pasteLine()
 * Paste a line at a given destination line and column into a file.
 * Return the new line, which is created as a result.
 */
LINE *ln_pasteLine(WINFO *wp, LINE *lpd, LINE *lps, int col, int bExpandTabs) {	
	int  oldsize;
	int  newsize;
	long nt;

	if (bExpandTabs) {
		if ((oldsize = ft_expandTabsWithSpaces(wp, _linebuf, LINEBUFSIZE, lpd->lbuf, lpd->len, &nt)) < 0)
			return 0;
	} else {
		oldsize = lpd->len;
		memmove(_linebuf,lpd->lbuf,oldsize);
	}

	newsize = ((col > oldsize) ? col : oldsize) + lps->len;
	FTABLE* fp = wp->fp;
	if ((lpd = ln_modify(fp,lpd,lpd->len,newsize)) == (LINE *) 0)
		return 0;

	if (col > oldsize) {
		memmove(lpd->lbuf,_linebuf,oldsize);
		memset(&lpd->lbuf[oldsize],' ', col - oldsize);
		memmove(&lpd->lbuf[col],lps->lbuf,lps->len);
	}
	else {
		memmove(lpd->lbuf,_linebuf,col);
		memmove(&lpd->lbuf[col],lps->lbuf,lps->len);
		memmove(&lpd->lbuf[col+lps->len],&_linebuf[col],oldsize-col);
	}

	return lpd;
}

/*
 * Paste a list of lines into a target line with a target offset column.
 * if bExpandTabs is 1, tabs are expanded by spaces along the way.
 */
int ln_pasteLines(WINFO* wp, LINE* lps, const LINE* lpLast, LINE* lpd, int col, int bExpandTabs) {
	LINE* lpnew;
	FTABLE* fp = wp->fp;
	while (lps && lps != lpLast) {
		if (lpd == fp->lastl) {
			if ((lpnew = ln_create(0)) == (LINE*)0) return 0;
			ln_insert(fp, lpd, lpnew);
			// convert the inserted line to be the temp line and free the memory of the inserted
			// line. Otherwise we produce a memory leak.
			if ((lpd = ln_modify(fp, lpnew, lpnew->len, lpnew->len)) == (LINE*)0) {
				return 0;
			}
		}
		if ((lpd = ln_pasteLine(wp, lpd, lps, col, bExpandTabs)) == (LINE*)0) {
			return 0;
		}
		lps = lps->next;
		lpd = lpd->next;
	}
	return 1;
}

/*--------------------------------------
 * bl_pastecol()
 * paste a text in column mode 
 *--------------------------------------*/
EXPORT int bl_pastecol(PASTE *pb,WINFO *wp, LINE *lpd, int col) {
	LINE *	lps;
	int 	bExpandTabs;

	lps  = pb->pln;
	col = caret_lineOffset2screen(wp, &(CARET) {.linePointer = lpd, .offset = col });
	bExpandTabs = PLAINCONTROL(wp->dispmode) ? 0 : 1;
	return ln_pasteLines(wp, lps, 0, lpd, col, bExpandTabs);
}

