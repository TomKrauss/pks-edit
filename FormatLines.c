/*
 * FormatLines.c
 *
 * format lines - 4 justifikation types
 *
 * PROJECT: PKSEDIT
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 14.03.1991
 */

#include <windows.h>
#include <string.h>

#include "alloc.h"
#include "caretmovement.h"
#include "edierror.h"
#include "textblocks.h"

#include "pksedit.h"
#include "mouseutil.h"
#include "winfo.h"

#define	isblnk(c)			(c == ' ' || c == '\t' || c == '' || c == '' || c == '')

extern MARK *mark_set(WINFO *wp, LINE *lp,int offs,int c);
extern LINE *mark_goto(WINFO *wp, int c, long *ln, long *col);
extern int string_isSpace(unsigned char c);

static FTABLE 		_fmtfile;
static unsigned char _fillc1,_fillc2;
static long   		_deltaln;
static LINE		*_currl;

/*---------------------------------*/
/* string_skipSpacesIn()					*/
/*---------------------------------*/
static unsigned char *string_skipSpacesIn(unsigned char *s, unsigned char *send)
{
	while(string_isSpace(*s))
		s++;
	return (s > send) ? send : s;
}

/*---------------------------------*/
/* string_skipNonSpaceCharactersIn()				*/
/*---------------------------------*/
static unsigned char *string_skipNonSpaceCharactersIn(unsigned char *s,unsigned char *send)
{
	while(s < send && !string_isSpace(*s))
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

	while(string_isSpace(*s)) {
		*d++ = *s++;
	}
	delta = (inter > 0) ? 1000L * (long)needfill / (long)inter : 1000L;
	count = 500L;
	while(*s) {
		/* copy a word */
		while(*s && !string_isSpace(*s))
			*d++ = *s++;
		while(string_isSpace(*s))
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

	buf2  = _fmtfile.lpReadPointer->lbuf;

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

		s = string_skipSpacesIn(s,send);

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
		s = string_skipNonSpaceCharactersIn(s,send);
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

/*---------------------------------
 * ft_formatText()	
 * Formt the text in the current file.
 *---------------------------------*/
int ft_formatText(WINFO* wp, int scope, int type, int flags)
{	PASTE  	paste;
	MARK *	mps;
	MARK *	mpe;
	int    	rmargin;
	int		savecol;
	int    	ret;
	long   	startln;
	FTABLE* fp;

	flags |= type;

	if (find_setTextSelection(wp, scope, &mps,&mpe) == RNG_INVALID ||
	    !mark_set(wp,wp->caret.linePointer,0,MARKDOT))
		return 0;

	fp = wp->fp;
	startln  = ln_indexOf(fp,mps->lm);
	_deltaln = -1;
	_currl   = wp->caret.linePointer;
	savecol  = caret_lineOffset2screen(wp, &wp->caret);

	if (flags & FMT_WPFORMAT) {
		_fillc1 = '';
		_fillc2 = '';
	} else {
		_fillc1 = ' ';
		_fillc2 = ' ';
	}
	rmargin = ww_getRightMargin(wp);
	mouse_setBusyCursor();
	memset(&_fmtfile,0,sizeof _fmtfile);

	if ((ret = formatlines(mps->lm,mpe->lm,flags & FMT_TYPEMASK,
					   flags & FMT_INDENT,rmargin)) > 0) {
		paste.pln = _fmtfile.firstl;
		if (bl_cutTextWithOptions((PASTE *)0,mps->lm,mpe->lm,0,mpe->lm->len,1))
			ret = bl_paste(&paste,wp,mps->lm,0,0);

		ln_listfree(_fmtfile.firstl);
	}

	/*
	 * text cursor position was in working area
	 */
	if (_currl == 0) {
		caret_moveToLine(wp, startln+_deltaln);
		wp->caret.linePointer = ln_goto(fp,wp->caret.ln);
	} else {
		wp->caret.linePointer = mark_goto(wp,MARKDOT,&wp->caret.ln,&startln);
		savecol = wp->caret.offset;
	}

	if (wp->caret.linePointer == 0) {
		wp->caret.linePointer = fp->firstl;
		caret_moveToLine(wp, 0);
	}

	caret_placeCursorInCurrentFile(wp, wp->caret.ln, savecol);
	render_repaintAllForFile(fp);
	mouse_setDefaultCursor();

	return ret;
}

