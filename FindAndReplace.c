/*
 * FindAndReplace.c
 *
 * PROJEKT: PKS-EDIT for WINDOWS 3.0..
 *
 * purpose: find and replace, substitution of tabs->spaces
 *
 * 										adapted      : 01.12.91
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include <string.h>

#include "trace.h"
#include "caretmovement.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"
#include "xdialog.h"

#pragma hdrstop

#include "pksedit.h"
#include "edctype.h"
#include "edfuncs.h"
#include "regexp.h"
#include "edhist.h"
#include "errordialogs.h"
#include "lineoperations.h"
#include "findandreplace.h"
#include "mouseutil.h"

#define	UCHAR	unsigned char

/*--------------------------------------------------------------------------
 * EXTERNALS
 */

extern 	MARK		*mark_set(FTABLE *fp, LINE *lp,int offs,int c);
extern 	MARK		*mark_find(FTABLE *fp, int c);
extern	int    	_playing,cursor_width;
unsigned char 		*tlcompile(unsigned char *transtab, 
						 unsigned char *t,
						 unsigned char *wt);

static UCHAR 	_expbuf[ESIZE];

SEARCH_AND_REPLACE_PARAMETER _currentSearchAndReplaceParams;
static RE_PATTERN	_lastCompiledPattern;

/*
 * Create an option object for compiling a regular expression. 
 */
static RE_OPTIONS *createREOptions(char* expression, char *ebuf, int flags, unsigned char eof) {
	static RE_OPTIONS _options;
	memset(&_options, 0, sizeof _options);
	memset(ebuf, 0, ESIZE);
	_options.patternBuf = ebuf;
	_options.flags = flags;
	_options.endOfPatternBuf = &ebuf[ESIZE];
	_options.eof = eof;
	_options.expression = expression;
	return &_options;
}

/*--------------------------------------------------------------------------
 * RegError()
 */
int RegError(int errorCode) {	
	error_showErrorById(errorCode);
	return 0;
}

/*--------------------------------------------------------------------------
 * regex_compile()
 */
RE_PATTERN *regex_compile(char *ebuf, char *pat, int flags) {
	flags &= (RE_DOREX|RE_IGNCASE|RE_SHELLWILD);
	if (!compile(createREOptions(pat, ebuf, flags, 0), &_lastCompiledPattern)) {
		RegError(_lastCompiledPattern.errorCode);
		return 0;
	}
	return &_lastCompiledPattern;
}

static REPLACEMENT_PATTERN _currentReplacementPattern;

/*
 * Answer TRUE if a replacement had been performed before. 
 */
BOOL find_replacementHadBeenPerformed() {
	return _currentReplacementPattern.preparedReplacementString != NULL;
}

/*--------------------------------------------------------------------------
 * find_initializeReplaceByExpression()
 * Initialize the pattern used as the replace by pattern in find and replace.
 */
int find_initializeReplaceByExpression(unsigned char* replaceByExpression) {
	unsigned char nlchar;
	if (ft_getCurrentDocument()) {
		nlchar = ft_getCurrentDocument()->documentDescriptor->nl;
	}
	else {
		nlchar = '\n';
	}
	int result = regex_initializeReplaceByExpressionOptions(&(REPLACEMENT_OPTIONS) { replaceByExpression, _currentSearchAndReplaceParams.options, nlchar, _lastCompiledPattern.nbrackets },
		& _currentReplacementPattern);
	if (_currentReplacementPattern.errorCode) {
		RegError(_currentReplacementPattern.errorCode);
	}
	return result;
}

/*--------------------------------------------------------------------------
 * find_setCurrentSearchExpression()
 * Remember the last expression searched for by the user.
 */
void find_setCurrentSearchExpression(char *pExpression)
{
	strcpy(_currentSearchAndReplaceParams.searchPattern, pExpression);
	hist_enq(SEARCH_PATTERNS, pExpression);
}

/*--------------------------------------------------------------------------
 * regex_compileWithDefault()
 * Compile a regular expression passed by argument with standard options.
 */
RE_PATTERN *regex_compileWithDefault(char *expression) {
	find_setCurrentSearchExpression(expression);
	return regex_compile(_expbuf,expression, _currentSearchAndReplaceParams.options) ? &_lastCompiledPattern : NULL;
}

/*--------------------------------------------------------------------------
 * stepback()
 */
static int stepback(UCHAR *sp, RE_PATTERN *pPattern, int currcol, int maxLen, RE_MATCH* pMatch)
{	register char	*	lsav=0;
	register char *	olloc2 = 0;
	register int 		i;
	int				col = 0;
	int				nLen;
	char *			brsave[18];

	while (step(pPattern, &sp[col], &sp[maxLen], pMatch)) {
		col = (int)(pMatch->loc2 - sp);
		nLen = (int )(pMatch->loc2 - pMatch->loc1);
		if (!nLen) {			/* x* */
			col++;
		}
		if (col > currcol) {
			break;
		}
		lsav = pMatch->loc1;
		olloc2 = pMatch->loc2;
		for (i = 0; i < pMatch->nbrackets; i++) {
			brsave[i]   = pMatch->braslist[i];
			brsave[i+9] = pMatch->braelist[i];
		}
		if (pMatch->circf || col >= maxLen) {
			break;
		}
	}
	if (lsav) {
		pMatch->loc2 = olloc2;
		pMatch->loc1 = lsav;
		for (i = 0; i < pMatch->nbrackets; i++) {
			pMatch->braslist[i] = brsave[i];
			pMatch->braelist[i] = brsave[i+9];
		 }
		 return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * xabort()
 */
static int _cancelled;
static int xabort(void )
{	static int _abort;

	if ((_abort++ & 0x3F) == 0 && progress_cancelMonitor(TRUE)) {
		_cancelled = 1;
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * find_expr()
 * ensure there is a current file !
 */
static LINE *find_expr(int dir,long *Ln,long *Col,LINE *lp,RE_PATTERN *pPattern, RE_MATCH *pMatch) {	
	long ln;

	ln = *Ln;
	if (dir > 0) {
		if (!*Col) goto fulline;
		if (!pMatch->circf && 
		     step(pPattern, &lp->lbuf[*Col], &lp->lbuf[lp->len], pMatch))
			goto fisuccess;
		for (;;) {
			if ((lp=lp->next) == 0) break;
			ln++;
fulline:		if (step(pPattern, lp->lbuf,&lp->lbuf[lp->len], pMatch))
				goto fisuccess;
		}
	} else {
		if (stepback(lp->lbuf, pPattern, (int ) *Col, lp->len, pMatch)) {
			goto fisuccess;
		}
		while (ln--, (lp = lp->prev) != 0) {
			if (stepback(lp->lbuf, pPattern, lp->len + 1, lp->len, pMatch)) {
				goto fisuccess;
			}
		}
	}

	return 0;

fisuccess:

	if (lp->next == 0) 
		return 0;

	*Ln = ln, *Col = (long)(pMatch->loc1 - lp->lbuf);
	return lp;
}

/*--------------------------------------------------------------------------
 * searchcpos()
 */
static int searchcpos(FTABLE *fp,long ln,int col, RE_MATCH *pMatch)
{	int col2,dc;
	WINFO  *wp;

	wp = WIPOI(fp);
	caret_placeCursorMakeVisibleAndSaveLocation(ln,col);
	col2 = caret_lineOffset2screen(fp, &(CARET) {
		fp->caret.linePointer, (int)(pMatch->loc2 - pMatch->loc1) + fp->caret.offset
	});
	cursor_width = dc = col2 - wp->col;
	wt_curpos(wp,wp->ln,wp->col);
	return dc;
}

/*--------------------------------------------------------------------------
 * find_expressionInCurrentFile()
 */
int find_expressionInCurrentFile(int dir, RE_PATTERN *pPattern,int options)
{	long ln,col;
	int  ret = 0,wrap = 0,wrapped = 0;
	LINE *lp;
	FTABLE *fp;
	RE_MATCH match;

	memset(&match, 0, sizeof match);
	fp = ft_getCurrentDocument();
	mouse_setBusyCursor();
	ln  = fp->ln;
	lp  = fp->caret.linePointer;
	col = fp->caret.offset;
	if (dir > 0) {
		if (P_EQ(&lp->lbuf[col], match.loc1) && P_NE(match.loc2, match.loc1))
			col = (long) (match.loc2 - lp->lbuf);
		else col++;
		if (match.circf || col > lp->len) {
			ln++, lp = lp->next;
			col = 0;
		}
	}

	if (options & O_WRAPSCAN)
		wrap = 1;

	if (find_expr(dir,&ln,&col,lp,pPattern, &match))
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
		if (find_expr(dir,&ln,&col,lp,pPattern, &match)) {
			wrapped++;
			ret = 1;
		}
	}
	mouse_setDefaultCursor();

	if (ret == 1) {
		searchcpos(fp,ln,col, &match);
		if (wrapped)
			error_showErrorById(IDS_MSGWRAPPED);
	}
	else {
		if (wrap)
			error_showErrorById(IDS_MSGSTRINGNOTFOUND);
		else
			error_showErrorById((dir > 0) ? IDS_MSGNOMATCHTOEND :
							 IDS_MSGNOMATCHTOSTART);
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * find_expressionAgainInCurrentFile()
 */
int find_expressionAgainInCurrentFile(dir) {
	RE_PATTERN* pPattern;

	return ((pPattern = regex_compileWithDefault(_currentSearchAndReplaceParams.searchPattern)) && find_expressionInCurrentFile(dir,pPattern, _currentSearchAndReplaceParams.options));
}

/*--------------------------------------------------------------------------
 * ft_countlinesStartingFromDirection()
 */
long ft_countlinesStartingFromDirection(FTABLE* fp, long start, int dir) {	
	register long nl = 0;
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
 * ft_expandTabsWithSpaces()
 * Expand tabs and replace with spaces.
 */
int ft_expandTabsWithSpaces(LINE *lp, long *nt)
{	unsigned char *d    = _linebuf,
			    *dend = &_linebuf[LINEBUFSIZE],c;
	unsigned char *s    = lp->lbuf,
			    *send = &lp->lbuf[lp->len];
	int col;

	while(s < send && d < dend) {
		if ((c = *s++) == '\t') {
			col = (int)(d - _linebuf);
			col = doctypes_calculateTabStop(col,ft_getCurrentDocument()->documentDescriptor) - col;
			memset(d,' ', col);
			(*nt)++;
			d += col;
		} else
			*d++ = c;
	}
	return (int)(d-_linebuf);
}

/*--------------------------------------------------------------------------
 * expandLine()
 */
static LINE *expandLine(FTABLE *fp, LINE *lp,long *nt)
{	long t = 0;
	int  size;

	size = ft_expandTabsWithSpaces(lp,&t);
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
 * find_expandTabsInFormattedLines()
 */
LINE *find_expandTabsInFormattedLines(FTABLE *fp, LINE *lp)
{	long t;

	if (PLAINCONTROL(fp->documentDescriptor->dispmode)) {
		return lp;
	}
	return expandLine(fp,lp,&t);
}

/*--------------------------------------------------------------------------
 * compline()
 */
static LINE *compline(FTABLE *fp, LINE *lp,long *nt)
{	char   *s;
	int    i,col,tab,start,foundpos,n2,ntabs;
	DOCUMENT_DESCRIPTOR *linp = fp->documentDescriptor;

	s = lp->lbuf, i = 0, col = 0; 

	while (i < lp->len) {
		ntabs = 0;
		tab = doctypes_calculateTabStop(col,linp);
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
				tab = doctypes_calculateTabStop(col,linp);
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
				memset(&lp->lbuf[start],'\t',ntabs);
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
	caret_placeCursorInCurrentFile(fp->ln,0L);
	if (*cntel) {
		render_redrawAndPaintCurrentFile();
		*cntln = ft_countlinesStartingFromDirection(fp,*cntln,1);
	}
	mouse_setDefaultCursor();
}

/*--------------------------------------------------------------------------
 * find_selectRangeWithMarkers()
 */
int find_selectRangeWithMarkers(int rngdefault, MARK** mps, MARK** mpe)
{
	if (!ft_getCurrentDocument() ||
		find_setTextSelection(rngdefault, ft_getCurrentDocument(), mps, mpe) == RNG_INVALID)
		return 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * find_replaceTabsWithSpaces()
 * flg = 1 : expand TABS to SPACES
 * flg = 0 : comp SPACES to TABS
 */
int find_replaceTabsWithSpaces(int scope, int flg)
{	long nt=0L,nl=0L;
	MARK *mps,*mpe;

	if (!find_selectRangeWithMarkers(scope,&mps,&mpe))
		return 0;

	progress_startMonitor(IDS_ABRTCONVERT);
	modifypgr(ft_getCurrentDocument(),(flg) ? expandLine : compline,&nt,&nl,mps,mpe);
	progress_closeMonitor(0);

	if (nt) 
		error_showMessageInStatusbar(IDS_MSGEXPANDTABS,nt,nl);
	return 1;
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
	term = fp->documentDescriptor->nl;
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
	FTABLE *	fp = ft_getCurrentDocument();
	LINE *	lp;
	int		len;

	if (!fp || string == 0)
		return 0;
	
	len = (int) strlen(string);
	if ((lp = ln_modify(fp,fp->caret.linePointer,fp->caret.offset,fp->caret.offset+len)) == 0L)
		return 0;

	memmove(&lp->lbuf[fp->caret.offset],string,len);
	breaklines(fp,1,fp->ln,fp->ln);

	render_repaintAllForFile(fp);

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
	size_t newlen;
	register	maxlen,delta;
	RE_MATCH	match;
	register	olen;
	int  	sc1flg = 1,splflg = _playing, column = 0, lastfcol,
			query,marked;

	memset(&match, 0, sizeof match);
	fp = ft_getCurrentDocument();

	if (ft_checkReadonlyWithError(fp) && action == REP_REPLACE) {
		return 0;
	}
	
	if (_playing) {
		regex_compileWithDefault(_currentSearchAndReplaceParams.searchPattern);
		find_initializeReplaceByExpression(_currentSearchAndReplaceParams.replaceWith);
	}
	newlen = strlen(_currentReplacementPattern.preparedReplacementString);

	/* call before assigning firstline	*/
	hist_enq(SEARCH_AND_REPLACE, _currentSearchAndReplaceParams.replaceWith);

	undo_startModification(fp);
	caret_saveLastPosition();

	if (find_setTextSelection(scope,fp,&markstart,&Markend) == RNG_INVALID)
		return 0;
	/* force register use */
	markend = Markend;

	lp  = markstart->lm;
	col = markstart->lc;
	startln = ln = ln_indexOf(fp,lp);

	startln = ln;
	lastfln = fp->ln;
	lastfcol = fp->caret.offset;

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
		progress_startMonitor(IDS_ABRTREPLACE);
	}

	while(lp) {

		if (column) {
			if (marked && (lp->lflg & LNXMARKED) == 0)
				goto nextline;
			if (P_NE(lp,oldxpnd)) {
				if ((lp = find_expandTabsInFormattedLines(fp,lp)) == 0)
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
			if (!match.circf && step(&_lastCompiledPattern, &lp->lbuf[col], &lp->lbuf[maxlen], &match))
				goto success;
		} else {
			if (marked && (lp->lflg & LNXMARKED) == 0)
				goto nextline;
			if (xabort())
				break;
			if (step(&_lastCompiledPattern, lp->lbuf, &lp->lbuf[maxlen], &match))
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

success:	olen = (int)(match.loc2 - match.loc1);
		col  = (int)(match.loc1 - lp->lbuf);

		if (action == REP_MARK || action == REP_COUNT) {
		if (action == REP_MARK) {
				delta = FBUFSIZE;
				lp->lflg |= LNXMARKED;
			} else
				delta = olen; 
			goto advance1;
		}

		if ((newlen = regex_replaceSearchString(&_currentReplacementPattern,_linebuf,MAXLINELEN, &match)) < 0) {
			linetoolong();
			break;
		}
		q = _linebuf;

		lastfln  = ln;
		lastfcol = col;

		if (query) {
			cursor_width = searchcpos(fp,ln,col, &match);
			switch (dlg_queryReplace(match.loc1,olen,q,(int)newlen)) {
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

		strxcpy(&lp->lbuf[col],q, (int)newlen);

		if (query)
			render_redrawCurrentLine();

		delta = (int)newlen;
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
		progress_closeMonitor(0);
	}
	dlg_closeQueryReplace();
	_playing = splflg;

	if (rp) {
		if (action != REP_COUNT) {
			if (_currentReplacementPattern.lineSplittingNeeded) {
				caret_placeCursorInCurrentFile(startln,0L);
				ln = breaklines(fp,0,startln,ln);
				caret_placeCursorMakeVisibleAndSaveLocation(ln,col);
				render_repaintAllForFile(fp);
			} else {
				caret_placeCursorMakeVisibleAndSaveLocation(lastfln,lastfcol);
				if (scope == RNG_ONCE && action == REP_REPLACE) {
					render_redrawCurrentLine();
				} else {
					render_repaintAllForFile(fp);
				}
			}
		}
		/* ft_countlinesStartingFromDirection MUST be called to clear lineflags !!!! */
		ln = ft_countlinesStartingFromDirection(fp,startln,1);
		if (action != REP_REPLACE) {
			error_showMessageInStatusbar(IDS_MSGNFOUND, _currentSearchAndReplaceParams.searchPattern,ln,rp);
		} else {
			error_showMessageInStatusbar(IDS_MSGNREPL, _currentSearchAndReplaceParams.searchPattern,rp,ln);
		}
	} else if (sc1flg && !_playing) {
		error_showErrorById(IDS_MSGSTRINGNOTFOUND);
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
	unsigned char nlchar;
	char		replace[256];
	RE_MATCH	match;

	_linebuf[0] = 0;
	RE_PATTERN* pPattern;
	if (ft_getCurrentDocument()) {
		nlchar = ft_getCurrentDocument()->documentDescriptor->nl;
	}
	else {
		nlchar = '\n';
	}
	if (pattern &&
	    string &&
	    with &&
	    (pPattern = regex_compile(ebuf, pattern, flags)) &&
		regex_initializeReplaceByExpressionOptions(&(REPLACEMENT_OPTIONS) {	with, flags, nlchar, pPattern->nbrackets}, & _currentReplacementPattern)) {
		src = string;
		send = _linebuf;
		
		/* copy default string */
		while((*send++ = *src++) != 0)
			;
		src = _linebuf;

		while(nmax-- > 0) {
			if (!step(pPattern, src,send, &match)) {
				break;
			}
			if (match.circf && match.loc1 != _linebuf) {
				break;
			}
			if ((newlen = regex_replaceSearchString(&_currentReplacementPattern, replace, sizeof replace, &match)) < 0) {
				break;
			}
			memmove(match.loc1+newlen, match.loc2,(int)(send- match.loc2));
			send += (newlen - (match.loc2 - match.loc1));
			*send = 0;
			strxcpy(match.loc1,replace,newlen);
			src = match.loc2;
		}
	}

	macro_returnString(_linebuf);
}

/*--------------------------------------------------------------------------
 * find_setTextSelection()
 * 
 * Select a range of text in the file identified by fp.
 */
int find_setTextSelection(int rngetype, FTABLE *fp, MARK **markstart, MARK **markend) {
	LINE *lps,*lpe;
	int  ofs,ofe;

	/*
	 * if rnge == RNG_FREE, range is already selected, return MARK positions
	 */
	if (rngetype == RNG_INVALID)
		return RNG_INVALID;

	if ( rngetype == RNG_FREE &&
	   ((*markstart = mark_find(fp,MARKSELSTART)) == 0 ||
	    (*markend   = mark_find(fp,MARKSELEND))   == 0)) {
	    	error_showErrorById(IDS_MSGNORANGESELECTED);
		return RNG_INVALID;
	}

	/*
	 * make range marks: initial is .,$ (RNG_FROMCURS,RNG_ONCE)
	 */

	lps = fp->caret.linePointer;
	ofs = fp->caret.offset;
	lpe = fp->lastl->prev;
	ofe = lpe->len;

	switch(rngetype) {
		case RNG_LINE:
			ofs = 0;
			lpe = lps;
			ofe = lpe->len;
			break;
		case RNG_CHAPTER:
			lps = lpe = fp->caret.linePointer;
			while(!ln_lineIsEmpty(lps) && lps->prev)
				lps = lps->prev;
			while(!ln_lineIsEmpty(lpe) && lpe->next != fp->lastl)
				lpe = lpe->next;
			ofs = 0;
			ofe = lpe->len;
			break;
		case RNG_BLOCK:
			if (!ft_checkSelectionWithError(fp))
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
			lpe = fp->caret.linePointer;
			ofe = fp->caret.offset;
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

