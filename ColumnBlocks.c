/*
 * ColumnBlocks.c
 *
 * cut and copy of column blocks
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 09.04.1992
 *									
 */

#include <windows.h>
#include <string.h>
#include "caretmovement.h"
#include "pksedit.h"
#include "edierror.h"

#define	TAB		9

/*---------------------------------*/
/* tabcpy()					*/
/*---------------------------------*/
static LINE *tabcpy(FTABLE *fp, LINE **Lps, int cfirst, int clast,
	int freeflg, int ctrlmode)
{
	LINE *		lp;
	LINE *		lps;
	unsigned char *s,*d,c;
	int			i,col,len,size;

	d = _linebuf;
	lps = *Lps;
	s   = lps->lbuf;
	len = lps->len;
	for (i = 0, col = 0; col < MAXLINELEN && i < len; i++) {
		if ((c = *s++) == TAB && !ctrlmode) {
			size = TabStop(col, fp->documentDescriptor);
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
 * Cut out a colomn shaped block of text and optionally delete it
 *----------------------------*/
EXPORT int bl_cutBlockInColumnMode(PASTE *pp,LINE *lnfirst,LINE *lnlast,int freeflg)
{
	FTABLE *	fp;
	LINE *	lpnew;
	LINE *	lpd = 0;
	LINE *	lp;
	long 	cnt;
	int      	cfirst;
	int		clast;
	int		ctrl;

	fp     = ft_CurrentDocument();
	lp     = lnfirst;
	cnt    = ln_cnt(lp,lnlast);
	cfirst = fp->blcol1;
	clast  = fp->blcol2;
	ctrl   = PLAINCONTROL(fp->documentDescriptor->dispmode);
	if (clast - cfirst <= 0) return 0;
	blfill(pp,sizeof(pp),0);
	while(cnt > 0) {
		if ((lpnew = tabcpy(fp, &lp,cfirst,clast,freeflg,ctrl)) == (LINE *) 0) 
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

/*---------------------------------*/
/* tabinsl()					*/
/*---------------------------------*/
static LINE *tabinsl(FTABLE *fp, LINE *lpd, LINE *lps, int col, int ctrlmode)
{	int  oldsize,newsize;
	long nt;

	if (ctrlmode == 0) {
		if ((oldsize = tab_expand(lpd,&nt)) < 0)
			return 0;
	} else {
		oldsize = lpd->len;
		memmove(_linebuf,lpd->lbuf,oldsize);
	}

	newsize = ((col > oldsize) ? col : oldsize) + lps->len;

	if ((lpd = ln_modify(fp,lpd,lpd->len,newsize)) == (LINE *) 0)
		return 0;

	if (col > oldsize) {
		memmove(lpd->lbuf,_linebuf,oldsize);
		blfill(&lpd->lbuf[oldsize],col-oldsize,' ');
		memmove(&lpd->lbuf[col],lps->lbuf,lps->len);
	}
	else {
		memmove(lpd->lbuf,_linebuf,col);
		memmove(&lpd->lbuf[col],lps->lbuf,lps->len);
		memmove(&lpd->lbuf[col+lps->len],&_linebuf[col],oldsize-col);
	}

	return lpd;
}

/*--------------------------------------*/
/* bl_pastecol()					*/
/* paste a textcol					*/
/*--------------------------------------*/
EXPORT int bl_pastecol(PASTE *pb,FTABLE *fp, LINE *lpd, int col)
{
	LINE *	lps;
	LINE *	lpnew;
	int 		ctrl;

	lps  = pb->pln;
	col = caret_lineOffset2screen(fp, &(CARET) { lpd, col });
	ctrl = PLAINCONTROL(fp->documentDescriptor->dispmode) ? 1 : 0;
	while (lps) {
		if (P_EQ(lpd,fp->lastl)) {
			if ((lpnew = ln_create(0)) == (LINE *) 0) return 0;
			ln_insert(fp,lpd,lpnew);
			lpd = lpnew;
		}
		if ((lpd = tabinsl(fp, lpd, lps, col, ctrl)) == (LINE *) 0) {
			return 0;
		}
		lps = lps->next;
		lpd = lpd->next;
	}
	return 1;
}

