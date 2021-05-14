/*
 * FindAndReplace.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS 3.0..
 *
 * purpose: find and replace, substitution of tabs->spaces
 *
 * 										adapted      : 01.12.91
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>

#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"

#pragma hdrstop

#include <string.h>
#include "pksedit.h"
#include "edctype.h"
#include "regexp.h"
#include "edhist.h"

#define	UCHAR	unsigned char

/*--------------------------------------------------------------------------
 * EXTERNALS
 */

extern	FTABLE 	*_currfile;
extern	LINE   	*ln_relgo(),*ln_goto(),
				*ln_modify(),*ln_split(),*lninsblnk();
extern 	MARK		*mark_set(FTABLE *fp, LINE *lp,int offs,int c);
extern 	MARK		*mark_find(FTABLE *fp, int c);
extern	int    	_playing,cursor_width;
extern	long 	cparagrph(long ln,int dir,int start);

extern	void 	*_alloc();

extern	long	  	ln_find(FTABLE *fp, LINE *lp);

unsigned char 		*tlcompile(unsigned char *transtab, 
						 unsigned char *t,
						 unsigned char *wt);
static unsigned char  *_transtabs[8];
static unsigned char  _chartab[256];
static char 		  _replexbuf[500];

extern	unsigned char *_octalloc;

UCHAR 	_expbuf[ESIZE];
int  	_rflg = 0;			/* something replaced until now ? */
char		_finds[500];
char		_repls[500];
int		_findopt;

/*--------------------------------------------------------------------------
 * RegError()
 */
extern int _reerrmsg[];
void RegError()
{	ed_error(_reerrmsg[_regerror-1]); }

static int doregerror(error) 
int error; 
{
	_regerror = error;
	RegError();
	return 0;
}

/*--------------------------------------------------------------------------
 * _regcompile()
 */
int _regcompile(char *ebuf, char *pat, int flags)
{
	flags &= (RE_DOREX|RE_IGNCASE|RE_SHELLWILD);
	if (!compile(pat,ebuf,&ebuf[ESIZE],0,flags)) {
		RegError();
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * setfinds()
 */
void setfinds(char *pat)
{
	strcpy(_finds, pat);
	hist_enq(&_findhist,pat);
}

/*--------------------------------------------------------------------------
 * exprinit()
 * call compile()
 */
int exprinit(char *pat)
{
	setfinds(pat);
	return _regcompile(_expbuf,pat,_findopt);
}

/*--------------------------------------------------------------------------
 * repinit()
 * init replace expression
 * should be calle after exprinit !
 */
static unsigned char *newtab(tl) int tl;
{	unsigned char *t;

	if (tl >= DIM(_transtabs))
		return (unsigned char *)doregerror(2);
	return ((t = _transtabs[tl]) != 0) ? 
		    t 
		    :
		    (_transtabs[tl] = _alloc(256));
}

static int _roptim;		/* use special copyout routines for replace ? */

static int _repinit(unsigned char *pat, int findopt)
{
	unsigned char 		c;
	unsigned char 		nlchar;
	unsigned char *	dest;
	unsigned char *	trpat;
	unsigned char *	tab;
	int 				tl = 0;
	int				i;

	for (i = 0; i < 256; i++)		/* translation default */
		_chartab[i] = i;
	_roptim = 0;
	_rflg = 1;
	if (!(findopt & RE_DOREX)) {
		strcpy(_replexbuf,pat);
		return 1;
	}
	if (_currfile) {
		nlchar = _currfile->lin->nl;
	} else {
		nlchar = _lineal.nl;
	}

	dest = _replexbuf;
	while ((c = *pat++) != 0) {
		if (c == '\\') {
			switch ((c = *pat++)) {
				case 'u':
					trpat = _l2uset;
					goto mktrans;
				case 'l':
					trpat = _u2lset;
mktrans:				if ((tab = newtab(tl++)) == 0)
						return 0;
					memmove(tab,trpat,256);
					c = '[';
					goto special;
				case '[':
					if ((tab = newtab(tl++)) == 0)
						return 0;
					if ((pat = tlcompile(tab,pat,
									 (unsigned char *)0)) == 0) 
						return doregerror(4);
				case 'e':
				case '&':
					goto special;
				default:
					if (c >= '1' && c <= '9') {
						if ((c - '0') > _nbrackets) 
							return doregerror(11);
					} else {
						c   = octalinput(pat-1);
						pat = _octalloc;
						if (!c)
							c = '0';
						else
							break;
					}
special:				_roptim |= 1;
						*dest++ = '\\';
					break;
			}
		}
		if (c == nlchar)
			_roptim |= 2;
		*dest++ = c;
	}
	*dest = 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * repinit()
 */
int repinit(unsigned char *pat)
{
	return _repinit(pat, _findopt);
}

/*--------------------------------------------------------------------------
 * stepback()
 */
static int stepback(UCHAR *sp, UCHAR *ebuf, int currcol, int maxLen)
{	register char	*	lsav=0;
	register char *	olloc2 = 0;
	register int 		i;
	int				col = 0;
	int				nLen;
	char *			brsave[18];

	while (step(&sp[col],ebuf, &sp[maxLen])) {
		col = (int)(__loc2 - sp);
		nLen = (int )(__loc2 - __loc1);
		if (!nLen) {			/* x* */
			col++;
		}
		if (col > currcol) {
			break;
		}
		lsav = __loc1;
		olloc2 = __loc2;
		for (i = 0; i < _nbrackets; i++) {
			brsave[i]   = _braslist[i];
			brsave[i+9] = _braelist[i];
		}
		if (_circf || col >= maxLen) {
			break;
		}
	}
	if (lsav) {
		__loc2 = olloc2;
		__loc1 = lsav; 
		for (i = 0; i < _nbrackets; i++) {
			_braslist[i] = brsave[i];
			_braelist[i] = brsave[i+9];
		 }
		 return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * xabort()
 */
static int _aborted;
static int xabort(void )
{	static int _abort;

	if ((_abort++ & 0x3F) == 0 && ed_abort(TRUE)) {
		_aborted = 1;
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * find_expr()
 * ensure there is a current file !
 */
LINE *find_expr(int dir,long *Ln,long *Col,LINE *lp,UCHAR *ebuf)
{	register	long ln;

	ln = *Ln;
	if (dir > 0) {
		if (!*Col) goto fulline;
		if (!_circf && 
		     step(&lp->lbuf[*Col],ebuf, &lp->lbuf[lp->len])) 
			goto fisuccess;
		for (;;) {
			if ((lp=lp->next) == 0) break;
			ln++;
fulline:		if (step(lp->lbuf,ebuf, &lp->lbuf[lp->len])) 
				goto fisuccess;
		}
	} else {
		if (stepback(lp->lbuf, ebuf, (int ) *Col, lp->len)) { 
			goto fisuccess;
		}
		while (ln--, (lp = lp->prev) != 0) {
			if (stepback(lp->lbuf, ebuf, lp->len + 1, lp->len)) {
				goto fisuccess;
			}
		}
	}

	return 0;

fisuccess:

	if (lp->next == 0) 
		return 0;

	*Ln = ln, *Col = (long)(__loc1 - lp->lbuf);
	return lp;
}

/*--------------------------------------------------------------------------
 * searchcpos()
 */
static int searchcpos(FTABLE *fp,long ln,int col)
{	int col2,dc;
	WINFO  *wp;

	wp = WIPOI(fp);
	centernewpos(ln,col);
	col2 = cphys2scr(fp->currl->lbuf,(int)(__loc2-__loc1)+fp->lnoffset);
	cursor_width = dc = col2 - wp->col;
	wt_curpos(wp,wp->ln,wp->col);
	return dc;
}

/*--------------------------------------------------------------------------
 * _findstr()
 */
int _findstr(int dir,UCHAR *ebuf,int options)
{	long ln,col;
	int  ret = 0,wrap = 0,wrapped = 0;
	LINE *lp;
	FTABLE *fp;

	fp = _currfile;
	MouseBusy();
	ln  = fp->ln;
	lp  = fp->currl;
	col = fp->lnoffset;
	if (dir > 0) {
		if (P_EQ(&lp->lbuf[col],__loc1) && P_NE(__loc2,__loc1))
			col = (long) (__loc2 - lp->lbuf);
		else col++;
		if (_circf || col > lp->len) {
			ln++, lp = lp->next;
			col = 0;
		}
	}

	if (options & O_WRAPSCAN)
		wrap = 1;

	if (find_expr(dir,&ln,&col,lp,ebuf))
		ret = 1;
	else if (wrap) {
		if (dir > 0) {
			ln  = 0;
			col = 0;
			lp  = fp->firstl;
		} else {
			ln  = fp->nlines-1;
			lp  = fp->lastl->prev;
			col = lp->len-1;
		}
		if (find_expr(dir,&ln,&col,lp,ebuf)) {
			wrapped++;
			ret = 1;
		}
	}
	changemouseform();

	if (ret == 1) {
		searchcpos(fp,ln,col);
		if (wrapped)
			ed_error(IDS_MSGWRAPPED);
	}
	else {
		if (wrap)
			ed_error(IDS_MSGSTRINGNOTFOUND);
		else
			ed_error((dir > 0) ? IDS_MSGNOMATCHTOEND :
							 IDS_MSGNOMATCHTOSTART);
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * findstr()
 */
int findstr(dir)
{
	return (exprinit(_finds) && _findstr(dir,_expbuf,_findopt));
}

/*--------------------------------------------------------------------------
 * countlines()
 */
long countlines(fp,start,dir)
FTABLE *fp;
long   start;
{	register long nl = 0;
	register LINE *lp;
	
	lp = ln_goto(fp,start);
	if (dir > 0) {
		while (lp->next) {
			if (lp->lflg & LNREPLACED) {
				nl++, lp->lflg &= ~LNREPLACED;
			}
			lp = lp->next;
		}
	} else {
		while (lp) {
			if (lp->lflg & LNREPLACED) {
				nl++, lp->lflg &= ~LNREPLACED;
			}
			lp = lp->prev;
		}
	} 
	return (nl);
}

/*--------------------------------------------------------------------------
 * tab_expand()
 */
int tab_expand(LINE *lp, long *nt)
{	unsigned char *d    = _linebuf,
			    *dend = &_linebuf[LINEBUFSIZE],c;
	unsigned char *s    = lp->lbuf,
			    *send = &lp->lbuf[lp->len];
	int col;

	while(s < send && d < dend) {
		if ((c = *s++) == '\t') {
			col = d - _linebuf;
			col = TabStop(col,_currfile->lin) - col;
			blfill(d,col,' ');
			(*nt)++;
			d += col;
		} else
			*d++ = c;
	}
	return (int)(d-_linebuf);
}

/*--------------------------------------------------------------------------
 * expline()
 */
static LINE *expline(FTABLE *fp, LINE *lp,long *nt)
{	long t = 0;
	int  size;

	size = tab_expand(lp,&t);
	if (t) {
		*nt += t;
		if ((lp = ln_modify(fp,lp,lp->len,size)) == 0L)
			return 0;
		lp->lflg |= LNREPLACED;
		memmove(lp->lbuf,_linebuf,size);
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * condexpline()
 */
LINE *condexpline(FTABLE *fp, LINE *lp)
{	long t;

	if (PLAINCONTROL(fp->lin->dispmode)) {
		return lp;
	}
	return expline(fp,lp,&t);
}

/*--------------------------------------------------------------------------
 * compline()
 */
static LINE *compline(FTABLE *fp, LINE *lp,long *nt)
{	char   *s;
	int    i,col,tab,start,foundpos,n2,ntabs;
	LINEAL *linp = fp->lin;

	s = lp->lbuf, i = 0, col = 0; 

	while (i < lp->len) {
		ntabs = 0;
		tab = TabStop(col,linp);
		start = i;
		while(i < lp->len) {
			if (*s == '\t') {
				col = tab;
			} else if (*s == ' ') {
				col++;
			} else {
				break;
			}
			s++, i++;
			if (col == tab) {
				foundpos = i;
				ntabs++;
				tab = TabStop(col,linp);
			}
		}

		/* we crossed a tab position */
		if (ntabs > 0) {
			/* worth compressing ? */
			n2 = foundpos-start-ntabs;
			if (n2 > 0) {
				if ((lp = ln_modify(fp,lp,foundpos,start+ntabs)) == 0L) 
					return 0;
				lp->lflg |= LNREPLACED;
				blfill(&lp->lbuf[start],ntabs,'\t');
				(*nt) += ntabs;
				i -= n2;
				s = lp->lbuf+i;
			}
		}
		
		start = i;
		while(i < lp->len && *s != '\t' && *s != ' ') {
			s++,i++;
		}
		col += (i-start);
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * modifypgr()
 * modify a block of text
 */
static void modifypgr(FTABLE *fp, LINE *(*func)(FTABLE *fp, LINE *lp, long *nt),
				  long *cntel,long *cntln,MARK *mps, MARK *mpe)
{	LINE     *lp;

	lp = mps->lm;

	for (; lp != 0 && (mpe->lc != 0 || lp != mpe->lm); lp = lp->next) {
		if ((lp = (*func)(fp,lp,cntel)) == 0)
			break;
		if (xabort() || lp == mpe->lm)
			break;
	}
	curpos(fp->ln,0L);
	if (*cntel) {
		p_redraw();
		*cntln = countlines(fp,*cntln,1);
	}
	changemouseform();
}

/*--------------------------------------------------------------------------
 * ReplaceTabs()
 * flg = 1 : expand TABS to SPACES
 * flg = 0 : comp SPACES to TABS
 */
int ReplaceTabs(int scope, int flg)
{	long nt=0L,nl=0L;
	MARK *mps,*mpe;

	if (!range(scope,&mps,&mpe))
		return 0;

	abrt_start(IDS_ABRTCONVERT);
	modifypgr(_currfile,(flg) ? expline : compline,&nt,&nl,mps,mpe);
	abrt_close(0);

	if (nt) 
		ShowMessage(IDS_MSGEXPANDTABS,nt,nl);
	return 1;
}

/*--------------------------------------------------------------------------
 * exprsub()
 * create replace-target for replaced Expressions with \
 * return length of target
 */
int exprsub(unsigned char *pat,unsigned char *newpat,int maxlen)
{	int len = 0;
	unsigned char *q,*qend,c,*trans = _chartab,tl=0;

	maxlen -= 2;
	while (*pat) {
		if (*pat == '\\') {
			c = pat[1];
			switch (c) {
				case '&':
					q	= __loc1;
					qend = __loc2;
					break;
				case '[':
					trans = _transtabs[tl++];
					goto nocpy;
				case 'e':
					trans = _chartab;
					goto nocpy;
				case '0':
					*newpat++ = 0;
					len++;
					goto nocpy;
				default:
					if (c >= '1' && c <= '9') {
						c   -= '1';
						q    = _braslist[c];
						qend = _braelist[c];
					} else goto normal;
					break;
			}
			len += (qend-q);
			if (len > maxlen) 
				return -1;
			while (q < qend)
				*newpat++ = trans[*q++];
nocpy:
			pat += 2;
			continue;
		}
normal:
		if (len++ > maxlen) return -1;
		*newpat++ = trans[*pat++];
	}
	*newpat = 0;
	return len;
}

/*--------------------------------------------------------------------------
 * breaklines()
 */
static long breaklines(FTABLE *fp,int all, long start,long end)
{	LINE *lc;
	register unsigned char term;
	register int i;
	long nl = start;

	lc   = ln_goto(fp,start);
	term = fp->lin->nl;
	while (lc && start <= end) {
		if (all || (lc->lflg & LNREPLACED)) {
rescan:
			for (i = 0; i < lc->len; i++)
				if (lc->lbuf[i] == term) {
					if ((lc = ln_split(fp,lc,i+1,i)) == 0)
						return nl;
					nl++, end++;
					lc = lc->next;
					goto rescan;
				}
		}
		start++;
		lc = lc->next;
	}
	return nl;
}

/*--------------------------------------------------------------------------
 * EdPasteString()
 */
int EdPasteString(long dummy1, long dummy2, char *string)
{
	FTABLE *	fp = _currfile;
	LINE *	lp;
	int		len;

	if (!fp || string == 0)
		return 0;
	
	len = strlen(string);
	if ((lp = ln_modify(fp,fp->currl,fp->lnoffset,fp->lnoffset+len)) == 0L)
		return 0;

	memmove(&lp->lbuf[fp->lnoffset],string,len);
	breaklines(fp,1,fp->ln,fp->ln);

	RedrawTotalWindow(fp);

	return 1;
}

/*--------------------------------------------------------------------------
 * strxcpy()
 */
static void strxcpy(char *d, char *s, int newlen)
{
	while(--newlen >= 0)
		*d++ = *s++;
}

/*--------------------------------------------------------------------------
 * EdReplaceText()
 * replace, mark, count... lines with RE
 */
int EdReplaceText(int scope, int action, int flags)
{	long 	ln,col,startln,lastfln,rp=0L;
	unsigned char 	*q;
	register	LINE *lp;
	register 	FTABLE *fp;
	MARK		*markstart,*Markend;
	register MARK *markend;
	LINE		*oldxpnd = 0;
	register	maxlen,delta,newlen;
	register	olen;
	int  	sc1flg = 1,splflg = _playing, column = 0, lastfcol,
			query,marked;

	fp = _currfile;

	if (readonly(fp) && action == REP_REPLACE) {
		return 0;
	}
	
	if (_playing) {
		exprinit(_finds);
		repinit(_repls);
	}
	newlen = strlen(_replexbuf);

	/* call before assigning firstline	*/
	hist_enq(&_replhist, _repls);

	u_init(fp);
	savecpos();

	if (select_range(scope,fp,&markstart,&Markend) == RNG_INVALID)
		return 0;
	/* force register use */
	markend = Markend;

	lp  = markstart->lm;
	col = markstart->lc;
	startln = ln = ln_find(fp,lp);

	startln = ln;
	lastfln = fp->ln;
	lastfcol = fp->lnoffset;

	query  = flags & OREP_INQ;
	marked = flags & OREP_MARKED;
	if (ww_blkcolomn(WIPOI(fp)) && scope == RNG_BLOCK)
		column = 1;

	/*
	 * to make following algorithm easier
	 */
	if (marked && (lp->lflg & LNXMARKED) == 0) {
		lp = lp->next;
		ln++;
		col = 0;
	}

	if (query)
		_playing = 0;

	if (!(query || scope == RNG_ONCE)) {
		abrt_start(IDS_ABRTREPLACE);
	}

	while(lp) {

		if (column) {
			if (marked && (lp->lflg & LNXMARKED) == 0)
				goto nextline;
			if (P_NE(lp,oldxpnd)) {
				if ((lp = condexpline(fp,lp)) == 0)
					break;
				oldxpnd = lp;
			}
			if (col < fp->blcol1)
				col = fp->blcol1;
			maxlen = (fp->blcol2 < lp->len) ? fp->blcol2 : lp->len;
		} else
			maxlen = lp->len;

		if (P_EQ(lp,markend->lm)) {
			if (!column) {
				maxlen = markend->lc;
				if (maxlen > lp->len)
					maxlen = lp->len;
			}
			if (col >= maxlen)
				break;
		}

		if (col) {
			if (!_circf && step(&lp->lbuf[col],_expbuf,
							 &lp->lbuf[maxlen]))
				goto success;
		} else {
			if (marked && (lp->lflg & LNXMARKED) == 0)
				goto nextline;
			if (xabort())
				break;
			if (step(lp->lbuf,_expbuf, &lp->lbuf[maxlen])) 
				goto success;
		}

		/*
		 * nothing found
		 *
		 * hide mark during MARK, if line is not selected and
		 * option "SELECTED ONLY" is set
		 */
		if (marked && action == REP_MARK) {
			lp->lflg &= ~LNXMARKED;
			lp->lflg |= LNREPLACED;
		}

nextline:
		if (P_EQ(lp,markend->lm))
			break;
		lp = lp->next; ln++; col = 0; continue;

success:	olen = (int)(__loc2 - __loc1);
		col  = (int)(__loc1 - lp->lbuf);

		if (action == REP_MARK || action == REP_COUNT) {
		if (action == REP_MARK) {
				delta = FBUFSIZE;
				lp->lflg |= LNXMARKED;
			} else
				delta = olen; 
			goto advance1;
		}

		if (_roptim) {		/* substitute expressions with \1 ...	  */
			if ((newlen = exprsub(_replexbuf,_linebuf,MAXLINELEN)) < 0) {
				linetoolong();
				break;
			}
			q = _linebuf;
		} else 
			q = _replexbuf;

		lastfln  = ln;
		lastfcol = col;

		if (query) {
			cursor_width = searchcpos(fp,ln,col);
			switch (QueryReplace(__loc1,olen,q,newlen)) {
				case IDNO:
					delta = olen;
					goto advance;
				case IDRETRY:
					query = 0;
				case IDOK: break;
				default:
					sc1flg = 0;
					goto endrep;
			}
		}

		if ((lp = ln_modify(fp,lp,
					    (int)(col+olen),
		                   (int)(col+newlen))) == 0L)
			break;

		strxcpy(&lp->lbuf[col],q,newlen);

		if (query)
			redrawline();

		delta = newlen;
advance1:	rp++;
		lp->lflg |= LNREPLACED;
		if (scope == RNG_ONCE)
			break;

advance:	if (delta <= 0 && olen <= 0)	/* empty expr. glitch */
			col++;
		else
			col += delta;

		if (col >= lp->len || (column && col >= maxlen))
			goto nextline;
	}

endrep:
	if (!(query || scope == RNG_ONCE)) {
		abrt_close(0);
	}
	CloseQueryReplace();
	_playing = splflg;

	if (rp) {
		if (action != REP_COUNT) {
			if (_roptim & 2) {
				curpos(startln,0L);
				ln = breaklines(fp,0,startln,ln);
				centernewpos(ln,col);
				RedrawTotalWindow(fp);
			} else {
				centernewpos(lastfln,lastfcol);
				if (scope == RNG_ONCE && action == REP_REPLACE) {
					redrawline();
				} else {
					RedrawTotalWindow(fp);
				}
			}
		}
		/* countlines MUST be called to clear lineflags !!!! */
		ln = countlines(fp,startln,1);
		if (action != REP_REPLACE) {
			ShowMessage(IDS_MSGNFOUND,_finds,ln,rp);
		} else {
			ShowMessage(IDS_MSGNREPL,_finds,rp,ln);
		}
	} else if (sc1flg && !_playing) {
		ed_error(IDS_MSGSTRINGNOTFOUND);
		return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * EdStringSubstitute()
 * substitute in a string
 */
void EdStringSubstitute(unsigned long nmax, long flags, char *string, char *pattern, char *with)
{
	char		ebuf[ESIZE];
	char *	src;
	char *	send;
	int		newlen;
	char		replace[256];

	_linebuf[0] = 0;
	if (pattern &&
	    string &&
	    with &&
	    _regcompile(ebuf, pattern, flags) &&
	    _repinit(with, flags)) {
		src = string;
		send = _linebuf;
		
		/* copy default string */
		while((*send++ = *src++) != 0)
			;
		src = _linebuf;

		while(nmax-- > 0) {
			if (!step(src,ebuf,send)) {
				break;
			}
			if (_circf && __loc1 != _linebuf) {
				break;
			}
			if ((newlen = exprsub(_replexbuf,replace,sizeof replace)) < 0) {
				break;
			}
			memmove(__loc1+newlen,__loc2,(int)(send-__loc2));
			send += (newlen - (__loc2 - __loc1));
			*send = 0;
			strxcpy(__loc1,replace,newlen);
			src = __loc2;
		}
	}

	ReturnString(_linebuf);
}

/*--------------------------------------------------------------------------
 * select_range()
 */
int select_range(int rngetype, FTABLE *fp, MARK **markstart, MARK **markend)
{	LINE *lps,*lpe;
	int  ofs,ofe;

	/*
	 * if rnge == RNG_FREE, range is already selected, return MARK positions
	 */
	if (rngetype == RNG_INVALID)
		return RNG_INVALID;

	if ( rngetype == RNG_FREE &&
	   ((*markstart = mark_find(fp,MARKSELSTART)) == 0 ||
	    (*markend   = mark_find(fp,MARKSELEND))   == 0)) {
	    	ed_error(IDS_MSGNORANGESELECTED);
		return RNG_INVALID;
	}

	/*
	 * make range marks: initial is .,$ (RNG_FROMCURS,RNG_ONCE)
	 */

	lps = fp->currl;
	ofs = fp->lnoffset;
	lpe = fp->lastl->prev;
	ofe = lpe->len;

	switch(rngetype) {
		case RNG_LINE:
			ofs = 0;
			lpe = lps;
			ofe = lpe->len;
			break;
		case RNG_CHAPTER:
			lps = lpe = fp->currl;
			while(!isempty(lps) && lps->prev)
				lps = lps->prev;
			while(!isempty(lpe) && lpe->next != fp->lastl)
				lpe = lpe->next;
			ofs = 0;
			ofe = lpe->len;
			break;
		case RNG_BLOCK:
			if (!chkblk(fp))
				return RNG_INVALID;
			lps = fp->blstart->lm;
			lpe = fp->blend->lm;
			if (ww_blkcolomn(WIPOI(fp))) {
				ofs = fp->blcol1;
				ofe = fp->blcol2;
			} else {
				ofs = fp->blstart->lc;
				ofe = fp->blend->lc;
			}
			break;
		case RNG_TOCURS:
			lpe = fp->currl;
			ofe = fp->lnoffset;
			/* drop through */
		case RNG_GLOBAL:
			ofs = 0;
			lps = fp->firstl;
	}

	if ((*markstart = mark_set(fp,lps,ofs,MARKSELSTART)) == 0 ||
	    (*markend = mark_set(fp,lpe,ofe,MARKSELEND)) == 0)
		return RNG_INVALID;

	return rngetype;
}

/*--------------------------------------------------------------------------
 * range()
 */
int range(int rngdefault,MARK **mps, MARK **mpe)
{
	if (!_currfile || 
	    select_range(rngdefault,_currfile,mps,mpe) == RNG_INVALID)
		return 0;
	return 1;
}

