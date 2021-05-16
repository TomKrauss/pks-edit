/*
 * EditorLinesIO.c
 *
 * split ascii data into lines
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 29.12.1990
 *									
 */
#include <windows.h>
#include "lineoperations.h"

#define	MAXLSIZE	MAXLINELEN

extern int 	_broken,_flushing;
extern LINEAL	*_rlp;

/*---------------------------------*/
/* createl()					*/
/* create a line 				*/
/*---------------------------------*/
int createl(FTABLE *fp, char *q, int len, int flags)
{	LINE  *lp;

	if (len < 0) {
		len = (int)strlen(q);
	}
	if ((lp = _alloc(len+sizeof(LINE))) == 0L) {
		lnlistfree(fp->firstl);
		fp->firstl = 0;
		return(0);
	}
	lp->len = len;
	lp->lflg = flags;
	lp->attr = 0;
	lp->next = 0L;
	lp->lbuf[len]     = 0;
	if (q) {
		memmove(lp->lbuf,q,len);
	}

	if ((lp->prev = fp->currl) != 0) {
		fp->currl->next = lp;
	} else {
		fp->firstl = lp;
	}
	fp->currl = lp;
	return 1;
}

/*---------------------------------*/
/* readnoterm()				*/
/*---------------------------------*/
unsigned char *readnoterm(FTABLE *fp, unsigned char *p, unsigned char *pend)
{	int  nl,width;
	long size;

	nl    = 0;
	width = _rlp->rmargin;
	if (width <= 0)
		width = 1;
	while(p < pend) {
		size = pend-p;
		if (size >= width) {
			size = width;
		} else if (!_flushing) {
			break;
		}
		if (!createl(fp,p,size,LNNOTERM)) {
			return 0;
		}
		p += size;
		nl++;
	}
	fp->nlines += nl;
	return p;
}

/*---------------------------------*/
/* readlines()					*/
/*---------------------------------*/
unsigned char *BufAsLinelist(FTABLE *fp, unsigned char *p, 
	unsigned char *pend, int t1, int t2, int cr)
{
	int 				flags;
	int				nl;
	int				len;
	unsigned char *	q;
	unsigned char *	pstart;

	nl = 0;
	pstart = p + 1;

	for (;;) {
		q = p;
		if (t2 >= 0) {
			while(*p != t1 && *p != t2) {
				p++;
			}
			p++;
		} else {
			while(*p++ != t1) {
				;
			}
		}

		len = p - q;

		if (cr >= 0 && p > pstart && p[-2] == cr) {
			len -= 2;
			flags = 0;
		} else {
			len--;
			flags = LNNOCR;
		}

		/* is this line too long ? */
		if (len > MAXLSIZE) {
			p = q + MAXLSIZE;
			len = MAXLSIZE;
			_broken++;
			goto createline;
		}

		if (p > pend) {
			fp->nlines += nl;
			return q;
		}

createline:
		if (!createl(fp,q,len,flags)) {
			return 0;
		}
		nl++;
	}
}

/*---------------------------------*/
/* readlines()					*/
/*---------------------------------*/
unsigned char *readlines(FTABLE *fp, unsigned char *p, unsigned char *pend)
{
	unsigned char 		t1;

	t1 = _rlp->nl;
	*pend = t1;
	return BufAsLinelist(fp, p, pend, t1, _rlp->nl2, _rlp->cr);
}

