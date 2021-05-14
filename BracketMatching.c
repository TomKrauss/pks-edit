/*
 * BracketMatching.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose:
 * check brackets, showmatch, bracket indent, shift between brackets...
 * shift parts of text
 * translate character up 2 low and vc.vs.
 *
 * 										created      : 20.02.87
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#include	<windows.h>
#include "trace.h"
#include	"lineoperations.h""
#include	"winfo.h"
#include	"edierror.h"
#include	"regexp.h"
#include	"uc.h"
#include	"brackets.h"
#include	"pksedit.h"
#include	"publicapi.h"

/*--------------------------------------------------------------------------
 * EXTERNALS
 */

extern FTABLE	*_currfile;
extern long	_multiplier;
extern long	ln_cnt(LINE *lps,LINE *lpe);
extern LINE 	*ln_gotouserel(FTABLE *fp,long ln);
extern void	*ll_insert(void *head,long size);
extern LINE	*optinswhite(FTABLE *fp, LINE *lp, int col, int *inserted);
extern int	CalcTabs2Col(LINEAL *linp, int tabs);

extern PASTELIST 	*_esclist[];
static BRACKET		*_brackets;
static UCLIST		*_undercursor;

/*--------------------------------------------------------------------------
 * eq_id()
 */
static int eq_id(int ctx,int ctxlin)
{
	if (ctx == LIN_DEFCTX || ctx == ctxlin)
		return 1;
	return 0;
}

/*--------------------------------------------------------------------------
 * key_globs()
 */
EXPORT void key_globs(BRACKET **bp, PASTELIST **pp[], UCLIST **up)
{
	if (!bp)
		return;
	*bp = _brackets;
	*pp = _esclist;
	*up = _undercursor;
}

/*--------------------------------------------------------------------------
 * mcmp()
 */
EXPORT int mcmp(char *buf,char *pat) 
{	char c;

	while ((c = *pat++) != 0)
		if (c != *buf++)
			return 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * uc_find()
 */
EXPORT struct uclist *uc_find(int ctx, char *b, int col,int actype)
{	struct uclist *up = _undercursor;
	int o;

	while (up) {
		if (up->action == actype		&&
		    eq_id(up->ctx,ctx)		&&
		    (o = col - up->len) >= 0	&&
		    mcmp(&b[o],up->pat)) 
		    return up;
		up = up->next;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * uc_init()
 */
EXPORT void uc_init(void)
{
	ll_kill(&_undercursor,(void*)0);
}

/*--------------------------------------------------------------------------
 * uc_add()
 */
EXPORT int uc_add(char *pat,char *p,int type,int id)
{	struct uclist *up;
	int len;
	
	if (!pat || !p) {
		return 0;
	}

	len = strlen(pat);
	for (up = _undercursor; up != 0; up = up->next) {
		if (up->action == type 	&&
		    up->len == len 		&& 
		    eq_id(up->ctx,id) 	&& 
		    strcmp(up->pat,pat) 	== 0) {
			goto done;
		}
	}

	if ((up = ll_insert(&_undercursor,sizeof *up)) == 0) {
		return 0;
	}

	up->pat      = pat;
	up->len	   = len;
	up->action   = type;
done:
	up->ctx	   = id;
	up->p	   = p;
	return 1;
}

/*--------------------------------------------------------------------------
 * sm_setup()
 * put brackets to uclist, to allow
 * uc_finds on them
 */
EXPORT int sm_setup(void)
{
	struct matchbox *	mp;
	char *				rb;
	char *				lb;

	for (mp = _brackets; mp; mp = mp->next) {

		rb = mp->righthand;
		lb = mp->lefthand;
		if (rb == 0 || lb == 0)
			continue;

		if (strcmp(rb,lb) &&				/* skip " .. " ! */
		    uc_add(rb,(char *)mp,UA_SHOWMATCH,mp->ctx) == 0)
			return 0;

	}
	return 1;
}

/*--------------------------------------------------------------------------
 * sm_define()
 */
EXPORT int sm_define(char *l,char *r,int v, 
		    int up1, int down1, int up2, int down2, 
		    int id) 
{	struct matchbox *mp;

	if (!l || !r) {
		return 0;
	}

	for (mp = _brackets; mp != 0;	mp = mp->next) {
		if ((eq_id(mp->ctx,id) && 
		     strcmp(l,mp->lefthand) == 0 && 
			strcmp(r,mp->righthand) == 0)) {
			break;
		}
	}
	
	if (!mp && (mp = ll_insert(&_brackets,sizeof *mp)) == 0)
		return 0;

	mp->lefthand  = l;
	mp->righthand = r;
	mp->d1 =  v;
	mp->d2 = -v;
	mp->ci1[0] = up1;
	mp->ci1[1] = down1;
	mp->ci2[0] = up2;
	mp->ci2[1] = down2;
	mp->ctx = id;
	return 1;
}

/*--------------------------------------------------------------------------
 * sm_init()
 */
EXPORT void sm_init(void)
{
	ll_kill(&_brackets,(void *)0);
}

/*--------------------------------------------------------------------------
 * bracketmatch()
 */
static int _righthand;
static int bracketmatch(char *s, struct matchbox *mp)
{
	if (mcmp(s,mp->lefthand)) {
		_righthand = 0;
		return 1;
	}

	if (mcmp(s,mp->righthand)) {
		_righthand = 1;
		return 1;
	}

	return 0;
}

/*--------------------------------------------------------------------------
 * ismatch()
 */
static struct matchbox *_lastmatch;
static struct matchbox *ismatch(char *s)
{	int id = _currfile->lin->id;
	struct matchbox *mp;

	for (mp = _brackets; mp; mp = mp->next) {
		if (eq_id(mp->ctx,id) && 
		    bracketmatch(s,mp)) {
			_lastmatch = mp;
			return mp;
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * scanmatch()
 */
static int scanmatch(int checkright, 
				 LINE *line,  	struct matchbox *mp,
				 long *Ln,	long *Col)
{	long ln = *Ln;
	char *s = &line->lbuf[*Col],*send;
	int  level;
	char c1,c2;

	c1 = mp->lefthand[0];
	c2 = mp->righthand[0];

	/*
	 * check for right bracket
	 */
	if (checkright) {
		level = 1;
		s++;
		while (line->next) {
			send = &line->lbuf[line->len];
			while (s < send) {
				if ((*s == c1 || *s == c2) && bracketmatch(s,mp)) {
					if (_righthand) {
						if (mp->d2 == 0)
							level = 0;
						else level += mp->d2;
					} else {
						level += mp->d1;
					}
					if (!level) {
						goto success;
					}
				}
				s++;
			}
			line = line->next;
			ln++, s = line->lbuf;
		}
	}
	/*
	 * check for left bracket
	 */
	else {
	 	level = -1;
		for(;;) {
			send = line->lbuf;
			while (--s >= send) {
				if ((*s == c1 || *s == c2) && bracketmatch(s,mp)) {
					if (!_righthand) {
						if (mp->d1 == 0)
							level = 0;
						else level += mp->d1;
					} else {
						level += mp->d2;
					}
					if (!level)
						goto success;
				}
			}
			if ((line = line->prev) == 0L) break;
			ln--;
			s = &line->lbuf[line->len];
		}
	}
	return 0;			/* no match */
success:
	 *Ln = ln ,*Col = (long)(s - line->lbuf);
	 return 1;
}

/*--------------------------------------------------------------------------
 * br_indentsum()
 */
static int br_indentsum(LINE *lps, LINE *lp, struct matchbox *mp, 
				    int *dcurr, int *hasind)
{	unsigned char *s,*send,c1,c2;
	int d1,d2,indent = 0;

	c1 = mp->lefthand[0];
	c2 = mp->righthand[0];
	d1 = (mp->ci1[0]+mp->ci1[1]);
	d2 = (mp->ci2[0]+mp->ci2[1]);

	s = lp->lbuf;
	send = s+lp->len;
	
	*hasind = 0;

	while(s < send) {
		if (mcmp(s,mp->lefthand)) {
			*dcurr = mp->ci1[0];
			*hasind = 1;
			break;
		}
		if (mcmp(s,mp->righthand)) {
			*dcurr = mp->ci2[0];
			*hasind = 1;
			break;
		}
		s++;
	}

	lp = lp->prev;

	while (lp) {
		send = lp->lbuf;
		s = send + lp->len;
		while (--s >= send) {
			if (*s == c1 && mcmp(s,mp->lefthand)) {
				indent += d1;
			}
			if (*s == c2 && mcmp(s,mp->righthand)) {
				indent += d2;
			}
		}
		if (lp == lps)
			break;
		lp = lp->prev;
	}
	return indent;
}

/*--------------------------------------------------------------------------
 * sm_bracketindent()
 *
 * this is called, when the user types Return:
 *
 * -	calculate the # of indents from line lp1 up to (not including!)
 *   the current line due to open brackets
 *
 * - calculate a  value for the current line (if this includes parenthesis)
 */
EXPORT int sm_bracketindent(FTABLE *fp, LINE *lp1, LINE *lpcurr, 
				 int indent, int *di, int *hbr)
{	int id = fp->lin->id;
	struct matchbox *mp;

	*di = 0;
	for (mp = _brackets; mp; mp = mp->next) {
		if (eq_id(mp->ctx,id) && *(long*)mp->ci1) {
			indent += br_indentsum(lp1,lpcurr,mp,di,hbr);
		}
	}

	return indent;
}

/*--------------------------------------------------------------------------
 * nextmatch()
 */
static int nextmatch(LINE *lp,long *ln,long *col)
{	register char	*s;
	register int	pos,j;
	struct matchbox *mp;

	pos = *col;
	s = &lp->lbuf[pos];
	if ((mp = ismatch(s)) != 0) 
		return (scanmatch(!_righthand,lp,mp,ln,col));
	s = lp->lbuf;
	for ( j = pos; --j >= 0; )
		if (ismatch(&s[j]) != 0) 
			goto matched;
	for ( j = pos; ++j < lp->len; )
		if (ismatch(&s[j]) != 0) 
			goto matched;

	return 0;

matched:
	*col = j; return 1;
}

/*--------------------------------------------------------------------------
 * EdShowMatch()
 * show matching brackets
 */
EXPORT int EdShowMatch(void)
{	register FTABLE *fp = _currfile;
	long 	 ln,col;

	if (!fp) return 0;
	ln = fp->ln, col = fp->lnoffset;
	if (nextmatch(fp->currl,&ln,&col)) {
		curpos(ln,col);
		return 1;
	} 
	ed_error(IDS_MSGNOBRACKETS);
	return 0;
}

/*--------------------------------------------------------------------------
 * showmatch(s)
 */
EXPORT int showmatch(LINE *lp,int Col)
{	FTABLE *fp = _currfile;
	long   ln  = fp->ln;
	long   col = Col;
	struct uclist *up;

	if ((up = uc_find(fp->lin->id,lp->lbuf,col,UA_SHOWMATCH)) != 0) {
		long lsav=ln,csav=col;

		col -= up->len;
		if (scanmatch(0,lp,(struct matchbox *)up->p,&ln,&col)) {
			if (ln >= WIPOI(fp)->minln) {
				curpos(ln,col);
				EdSleep();
				curpos(lsav,csav);
				return 1;
			}
		} else {
			ed_error(IDS_MSGNOBRACKETS);
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdCharUpToLow()
 * Map low Chars to up
 * and the other way
 */
extern unsigned char _l2uset[],_u2lset[];
EXPORT int EdCharUpToLow(void )
{	LINE	 *lp;
	unsigned char c,c1;
	int  offs;
	FTABLE *fp;

	fp   = _currfile;
	lp	= fp->currl;
	offs = fp->lnoffset;
	c    = lp->lbuf[offs];
	if (((c1 = _l2uset[c]) != c || (c1 = _u2lset[c]) != c) &&
	    (lp = ln_modify(fp,lp,offs,offs)) != (LINE *)0) {
		lp->lbuf[offs] = c1;
		redrawline();
	}
	return EdCursorRight(1);
}

/*--------------------------------------------------------------------------
 * shift_lines()
 * shift n lines
 */
EXPORT int shift_lines(FTABLE *fp, long ln, long nlines, int dir)
{	register LINE	*lp;
	register char	*s,*send;
	register long  i,offset;
	register int	ind,blcount,col;
	int 			dummy;

	if ((lp = ln_gotouserel(fp,ln)) == 0L) 
		return 0;

	for (i = 0; i < nlines; i++) {
		if (!lp->next) break;
		ind = CalcTabs2Col(fp->lin,abs(dir));
		if (dir > 0) {
			if ((lp = optinswhite(fp,lp,ind,&dummy)) == 0L) {
				return 0;
			}
		} else {
			s = lp->lbuf;
			send = s+lp->len;
			col = 0;
			while(col < ind && s < send) {
				if (*s != ' ') {
					if (*s == '\t')
						col = TabStop(col,fp->lin);
					else
						break;
				} else {
					col++;
				}
				s++;
			}
			blcount = s - lp->lbuf;
			if (blcount) {
				if ((lp = ln_modify(fp,lp,blcount,0)) == 0L)
					return 0;
			} 
		}
		lp = lp->next;
	}
	EdRedrawWindow(WIPOI(fp));

	offset = fp->lnoffset;
	if (fp->ln >= ln && fp->ln < ln+nlines)
		offset += dir;

	curpos(fp->ln,offset);

	return 1;
}

/*--------------------------------------------------------------------------
 * EdShiftBetweenBrackets()
 * shift the text between brackets
 */
EXPORT int EdShiftBetweenBrackets(int dir)
{	register FTABLE *fp;
	register long ln2;
	long ln,col;
	
	fp = _currfile;
	ln = fp->ln, col = fp->lnoffset;

	if (!nextmatch(fp->currl,&ln,&col)) {
		return 0;
	}

	if (ln > fp->ln) {
		ln2 = ln;
		ln  = fp->ln;
	} else {
		ln2 = fp->ln;
	}
	
	/* care indentation style */
	if (_lastmatch) {
		ln  += _lastmatch->ci1[1];
		ln2 -= _lastmatch->ci2[1];
	}

	return shift_lines(fp,ln,ln2-ln,dir);
}

/*--------------------------------------------------------------------------
 * EdLinesShift()
 * shift _multiplier lines
 */
EXPORT int EdLinesShift(int dir)
{	FTABLE *fp = _currfile;

	if (!fp)
		return 0;
	return shift_lines(fp,fp->ln,_multiplier,dir);
}

/*--------------------------------------------------------------------------
 * RangeShift()
 * shift text range
 */
EXPORT int RangeShift(int scope, int dir)
{	MARK	*mps,*mpe;
	FTABLE *fp = _currfile;
	long	ln1,ln2;
	
	if (!range(scope,&mps,&mpe))
		return 0;

	if (dir == 0)
		dir = -1;

	ln1 = ln_cnt(fp->firstl,mps->lm)-1;
	ln2 = ln_cnt(mps->lm,mpe->lm);
	if (mpe->lc == 0)
		ln2--;

	return shift_lines(fp,ln1,ln2,dir);
}


