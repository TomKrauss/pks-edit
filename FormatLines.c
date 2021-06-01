/*
 * FormatLines.c
 *
 * format lines - 4 justifikation types
 *
 * PROJECT: PKSEDIT
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 14.03.1991
 *									
 */


#include <string.h>

#include "alloc.h"
#include "caretmovement.h"
#include "edierror.h"
#include "textblocks.h"

#include "pksedit.h"

#define	isblnk(c)			(c == ' ' || c == '\t' || c == '' || c == '' || c == '')

extern long cparagrph(long ln,int dir,int start);
extern MARK *mark_set(FTABLE *fp, LINE *lp,int offs,int c);
extern LINE *mark_goto(FTABLE *fp, int c, long *ln, long *col);
extern int IsSpace(unsigned char c);

static FTABLE 		_fmtfile;
static unsigned char _fillc1,_fillc2;
static long   		_deltaln;
static LINE		*_currl;

/*---------------------------------*/
/* skipblank()					*/
/*---------------------------------*/
static unsigned char *skipblank(unsigned char *s, unsigned char *send)
{
	while(IsSpace(*s))
		s++;
	return (s > send) ? send : s;
}

/*---------------------------------*/
/* skipnoblank()				*/
/*---------------------------------*/
static unsigned char *skipnoblank(unsigned char *s,unsigned char *send)
{
	while(s < send && !IsSpace(*s))
		s++;
	return s;
}

/*---------------------------------*/
/* blkformat()					*/
/*---------------------------------*/
static void blkformat(unsigned char *d, unsigned char *s, 
				  int inter, int needfill)
{	long 		count;
	long 		delta;

	while(IsSpace(*s)) {
		*d++ = *s++;
	}
	delta = (inter > 0) ? 1000L * (long)needfill / (long)inter : 1000L;
	count = 500L;
	while(*s) {
		/* copy a word */
		while(*s && !IsSpace(*s))
			*d++ = *s++;
		while(IsSpace(*s))
			*d++ = *s++;

		/* 
		 * special: last betweening - assure all fills are done
		 */
		if (--inter <= 0) {
			while(needfill-- > 0)
				*d++ = _fillc1;
		} else {
			/*
			 * stray in spaces at certain positions
			 */
			count += delta;
			while(count > 1000L) {
				*d++ = _fillc1;
				count -= 1000L, needfill--;
			}
		}
	}
	*d = 0;
}

/*---------------------------------*/
/* format()					*/
/*---------------------------------*/
static int format(int type, int nwords, int rmar, int add,
			   unsigned char *buf,unsigned char *endp,
			   int lastline)
{	unsigned char *	buf2;
	int 			l;
	int				rm;
	int				delta;

	if (!endp) {
		return 1;
	}
	*endp = 0;

	rm = (int)(endp-buf);

	/*
	 * if the lines right border is bigger than wm, adjust rmar
	 * for correct allocation...
	 */
	if (rm > rmar)
		rmar = rm;

	l = add + ((type == FMT_ADJLEFT) ? rm : rmar);
	delta = rmar - rm;

	if (type == FMT_ADJCENTER)	/* dont need the whole line */
		l -= (delta - delta/2);

	if (!ln_createAndAdd(&_fmtfile,(char *)0,l,(lastline) ? 0 : LNNOCR))
		return 0;
	_fmtfile.nlines++;

	buf2  = _fmtfile.caret.linePointer->lbuf;

	switch(type) {
		case FMT_ADJCENTER:
			delta /= 2;
		case FMT_ADJRIGHT:
			if (delta)
				memset(buf2,_fillc2,delta);
			memmove(&buf2[delta],buf,rm);
			break;
		case FMT_ADJBLOCK:
			/*
			 * fill up **rm** spaces with **nwords**
			 */
			if (nwords > 1) {
				blkformat(buf2,buf,nwords-1,delta);
				break;
			}
			/* 
			 * drop through
			 */
		default:
			memset(buf2,_fillc2,l);
			memmove(buf2,buf,rm);
	}

	if (add)
		buf2[l-1] = _fillc1;

	return 1;
}

/*---------------------------------*/
/* formatlines()				*/
/*---------------------------------*/
static int formatlines(LINE *lps, LINE *lplast, 
				   int fmttype, int doindent, int rmar)
{	char *dend,*d,*s1,*s,*send;
	int  dword,spgrph,lmar;

	if (rmar >= LINEBUFSIZE) {
		/*
		 * bad: invalid right margin given!
		 */
		return 0;
	}

	d = _linebuf;
	dend = &d[rmar];

	spgrph = 1;
	dword  = 0;
	lmar = 0;

	goto start;

	for (;;) {
		/* eol reached */
		if (s >= send) {
			if (lps == 0)
				return 1;
			if (!(lps->lflg & LNNOCR) || lps == lplast || !lps->next) {
				if (!format((fmttype == FMT_ADJBLOCK) ? 
						   FMT_ADJLEFT : fmttype,
				        	   dword,rmar,0,_linebuf,d,1)) {
					return 0;
				}
				dword = 0;
				spgrph = 1;
			}
			if (lps == lplast || (lps = lps->next) == 0) {
				/* eo input reached */
				return 1;
			}
start:		s = lps->lbuf;
			send = &s[lps->len];
		}

		if (_currl && _currl == lps) {
			_deltaln = _fmtfile.nlines;
			_currl = 0;
		}

		s = skipblank(s,send);

		/* 
		 * start of new pgrph: recalculate left margin 
		 */
		if (spgrph) {
			/*
			 * print end of paragraph left justified instead of block
			 * justifikation and with delimiter line
			 */
			if (doindent) {
				lmar = (int)(s-lps->lbuf);
				lmar = caret_lineOffset2screen(NULL, &(CARET) {
					lps, lmar
				});
			}
			spgrph = 0;
		}

		if (dword == 0) {
nextdline:	d = memset(_linebuf,_fillc1,lmar);
			if (_fillc2 == '' && lmar > 0)
				_linebuf[0] = '';
		}

		s1 = s;
		s = skipnoblank(s,send);
		if (s == s1)
			continue;

		/*
		 * wrap to next line
		 */
		if (&d[(int)(s-s1)] >= dend && dword > 0) {
#if 0
			while(d > _linebuf && d[-1] == _fillc1)
				d--;
# endif
			if (!format(fmttype,dword,rmar,(_fillc1 == ''),_linebuf,d,0))
				return 0;
			s = s1;
			dword = 0;
			goto nextdline;
		}

		/* copy next word to destination */

		if (dword) {
			/* handle hyphens */
			if (d[-1] == '')
				d--;
			else {
				*d++ = _fillc1;
				dword++;
			}
		} else {
			dword++;
		}
		while(s1 < s) {
			*d++ = *s1++;
		}
	}
}

/*---------------------------------*/
/* EdFormatText()				*/
/*---------------------------------*/
FormatText(int scope, int type, int flags)
{	FTABLE *fp;
	PASTE  	paste;
	MARK *	mps;
	MARK *	mpe;
	int    	rmargin;
	int		savecol;
	int    	ret;
	long   	startln;

	fp = ft_CurrentDocument();
	flags |= type;

	if (SelectRange(scope,fp,&mps,&mpe) == RNG_INVALID ||
	    !mark_set(fp,fp->caret.linePointer,0,MARKDOT))
		return 0;

	startln  = ln_indexOf(fp,mps->lm);
	_deltaln = -1;
	_currl   = fp->caret.linePointer;
	savecol  = caret_lineOffset2screen(fp, &fp->caret);

	if (flags & FMT_WPFORMAT) {
		_fillc1 = '';
		_fillc2 = '';
	} else {
		_fillc1 = ' ';
		_fillc2 = ' ';
	}
	rmargin = RightMargin(fp);
	MouseBusy();

	memset(&_fmtfile,0,sizeof _fmtfile);

	if ((ret = formatlines(mps->lm,mpe->lm,flags & FMT_TYPEMASK,
					   flags & FMT_INDENT,rmargin)) > 0) {
		paste.pln = _fmtfile.firstl;
		if (bl_cutTextWithOptions((PASTE *)0,mps->lm,mpe->lm,0,mpe->lm->len,1))
			ret = bl_paste(&paste,fp,mps->lm,0,0);

		ln_listfree(_fmtfile.firstl);
	}

	/*
	 * text cursor position was in working area
	 */
	if (_currl == 0) {
		fp->ln = startln+_deltaln;
		fp->caret.linePointer = ln_goto(fp,fp->ln);
	} else {
		fp->caret.linePointer = mark_goto(fp,MARKDOT,&fp->ln,&startln);
		savecol = fp->caret.offset;
	}

	if (fp->caret.linePointer == 0) {
		fp->caret.linePointer = fp->firstl;
		fp->ln = 0;
	}

	curpos(fp->ln,savecol);
	RedrawTotalWindow(fp);
	changemouseform();

	return ret;
}

