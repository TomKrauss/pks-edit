/*
 * FindAndReplace.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: find and replace, substitution of tabs->spaces
 *
 * 										migrated from ATARI ST: 01.12.91
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
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

#include "dial2.h"
#include "pksedit.h"
#include "edctype.h"
#include "pksmacro.h"
#include "regexp.h"
#include "history.h"
#include "errordialogs.h"
#include "documentmodel.h"
#include "findandreplace.h"
#include "mouseutil.h"
#include "textblocks.h"

#define	UCHAR	unsigned char

/*--------------------------------------------------------------------------
 * EXTERNALS
 */

extern 	MARK		*mark_set(WINFO *wp, LINE *lp,int offs,int c);
extern 	MARK		*mark_find(WINFO *wp, int c);
unsigned char 		*translate_compileCharacterTable(unsigned char *transtab, 
						 unsigned char *t,
						 unsigned char *wt);

SEARCH_AND_REPLACE_PARAMETER _currentSearchAndReplaceParams = {
	"",
	"",
	0,
	"",
	"",
	-1,
	0
};

/*
 * Create an option object for compiling a regular expression. 
 */
static RE_OPTIONS *createREOptions(const char* expression, char *ebuf, int flags, unsigned char eof) {
	static RE_OPTIONS _options;
	memset(&_options, 0, sizeof _options);
	memset(ebuf, 0, ESIZE);
	_options.patternBuf = ebuf;
	_options.flags = flags;
	_options.endOfPatternBuf = &ebuf[ESIZE];
	_options.eof = eof;
	_options.expression = (char*)expression;
	return &_options;
}

/*--------------------------------------------------------------------------
 * regex_compilationFailed()
 */
static int regex_compilationFailed(int errorCode) {	
	error_showErrorById(errorCode);
	return 0;
}

/*--------------------------------------------------------------------------
 * find_regexCompile()
 */
RE_PATTERN *find_regexCompile(RE_PATTERN* pCompiled, char *ebuf, const char *pat, int flags) {
	flags &= (RE_DOREX|RE_IGNCASE|RE_SHELLWILD);
	if (!regex_compile(createREOptions(pat, ebuf, flags, 0), pCompiled)) {
		regex_compilationFailed(pCompiled->errorCode);
		return 0;
	}
	return pCompiled;
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
int find_initializeReplaceByExpression(unsigned const char* replaceByExpression) {
	unsigned char nlchar;
	if (ft_getCurrentDocument()) {
		nlchar = ft_getCurrentDocument()->documentDescriptor->nl;
	}
	else {
		nlchar = '\n';
	}
	int result = regex_initializeReplaceByExpressionOptions(&(REPLACEMENT_OPTIONS) { 
			.replacementPattern = (char*)replaceByExpression, 
			.flags = _currentSearchAndReplaceParams.options, 
			.newlineCharacter = nlchar, 
			.maxCaptureGroups = _currentSearchAndReplaceParams.compiled.nbrackets },
		& _currentReplacementPattern);
	if (_currentReplacementPattern.errorCode) {
		regex_compilationFailed(_currentReplacementPattern.errorCode);
	}
	return result;
}

/*--------------------------------------------------------------------------
 * find_setCurrentSearchExpression()
 * Remember the last expression searched for by the user.
 */
void find_setCurrentSearchExpression(const char *pExpression) {
	strcpy(_currentSearchAndReplaceParams.searchPattern, pExpression);
}

/*--------------------------------------------------------------------------
 * regex_compileWithDefault()
 * Compile a regular expression passed by argument with standard options. If
 * not successful, return null.
 */
RE_PATTERN* regex_compileWithError(const char* expression) {
	static UCHAR 	_expbuf[ESIZE];
	RE_PATTERN* pPattern;

	if ((pPattern = find_regexCompile(&_currentSearchAndReplaceParams.compiled, _expbuf, expression, _currentSearchAndReplaceParams.options)) == NULL) {
		return NULL;
	}
	find_setCurrentSearchExpression(expression);
	_currentSearchAndReplaceParams.compiled = *pPattern;
	return pPattern;
}

/*--------------------------------------------------------------------------
 * regex_compileWithDefault()
 * Compile a regular expression passed by argument with standard options.
 */
RE_PATTERN *regex_compileWithDefault(const char *expression) {
	RE_PATTERN* pPattern = regex_compileWithError(expression);
	if (!pPattern) {
		pPattern = &_currentSearchAndReplaceParams.compiled;
	}
	return pPattern;
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

	while (regex_match(pPattern, &sp[col], &sp[maxLen], pMatch)) {
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
 * find_abortProgress()
 */
static int _cancelled;
static int find_abortProgress(void )
{	static int _abort;

	if ((_abort++ & 0x3F) == 0 && progress_cancelMonitor(TRUE)) {
		_cancelled = 1;
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * find_expression()
 * ensure there is a current file !
 */
static LINE *find_expression(int dir,long *Ln,long *Col,LINE *lp,RE_PATTERN *pPattern, RE_MATCH *pMatch) {	
	long ln;

	ln = *Ln;
	if (dir > 0) {
		if (!*Col) goto fulline;
		if (!pMatch->circf && 
			regex_match(pPattern, &lp->lbuf[*Col], &lp->lbuf[lp->len], pMatch))
			goto fisuccess;
		for (;;) {
			if ((lp=lp->next) == 0) break;
			ln++;
fulline:		if (regex_match(pPattern, lp->lbuf,&lp->lbuf[lp->len], pMatch))
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
 * find_updateCursorToShowMatch()
 */
static int find_updateSelectionToShowMatch(WINFO *wp,long ln,int col, RE_MATCH *pMatch)
{	int    dc;

	dc = (int)(pMatch->loc2 - pMatch->loc1);
	caret_placeCursorMakeVisibleAndSaveLocation(wp, ln,col);
	bl_hideSelection(wp, 1);
	bl_setSelection(wp, wp->caret.linePointer, wp->caret.offset, wp->caret.linePointer, dc + wp->caret.offset);
	render_repaintWindowLine(wp, wp->caret.ln);
	return dc;
}

/*--------------------------------------------------------------------------
 * find_expressionInCurrentFile()
 */
static int find_expressionInCurrentFileStartingFrom(FTABLE* fp, CARET cCaret, int dir, RE_PATTERN* pPattern, int options, long *pFoundLine, long *pFoundCol,
	RE_MATCH *pMatch, int *pWrapped)
{
	long ln, col;
	int  ret = 0, wrap = 0, wrapped = 0;
	LINE* lp;

	ln = cCaret.ln;
	lp = cCaret.linePointer;
	col = cCaret.offset;
	if (dir > 0) {
		if (P_EQ(&lp->lbuf[col], pMatch->loc1) && P_NE(pMatch->loc2, pMatch->loc1))
			col = (long)(pMatch->loc2 - lp->lbuf);
		else col++;
		if (pMatch->circf || col > lp->len) {
			ln++, lp = lp->next;
			col = 0;
		}
	}

	if (options & RE_WRAPSCAN)
		wrap = 1;

	if (find_expression(dir, &ln, &col, lp, pPattern, pMatch))
		ret = 1;
	else if (wrap) {
		if (dir > 0) {
			ln = 0;
			col = 0;
			lp = fp->firstl;
		}
		else {
			ln = fp->nlines - 1;
			lp = fp->lastl->prev;
			col = lp->len - 1;
		}
		if (find_expression(dir, &ln, &col, lp, pPattern, pMatch)) {
			wrapped++;
			ret = 1;
		}
	}
	if (ret) {
		*pFoundLine = ln;
		*pFoundCol = col;
		*pWrapped = wrapped;
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * find_expressionInCurrentFile()
 */
int find_expressionInCurrentFile(WINFO* wp, int dir, char* pszString, RE_PATTERN *pPattern,int options) {
	long ln,col;
	int ret = 0;
	int wrap = options & RE_WRAPSCAN;
	int wrapped = 0;
	FTABLE *fp;
	static RE_MATCH match;
	static WINFO* lastWP;

	if (wp == NULL) {
		return 0;
	}
	if (lastWP != wp) {
		memset(&match, 0, sizeof match);
		lastWP = wp;
	}
	fp = wp->fp;
	mouse_setBusyCursor();

	ret = find_expressionInCurrentFileStartingFrom(fp, wp->caret, dir, pPattern, options, &ln, &col, &match, &wrapped);
	mouse_setDefaultCursor();

	if (ret == 1) {
		find_updateSelectionToShowMatch(wp,ln,col, &match);
		if (wrapped)
			error_showErrorById(IDS_MSGWRAPPED);
	}
	else {
		if (wrap)
			error_showErrorById(IDS_MSGSTRINGNOTFOUND, pszString);
		else
			error_showErrorById((dir > 0) ? IDS_MSGNOMATCHTOEND :
							 IDS_MSGNOMATCHTOSTART);
	}
	return ret;
}

/*
 * Find a string incremental with given options either forward or backward depending on nDirection.
 */
static CARET incrementalStart;
int find_incrementally(char* pszString, int nOptions, int nDirection, BOOL bContinue) {
	RE_PATTERN* pPattern;
	int ret;
	int wrapped;
	long ln;
	long col;
	WINFO* wp;
	RE_MATCH match;

	wp = ww_getCurrentEditorWindow();
	if (wp == NULL) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	_currentSearchAndReplaceParams.options = nOptions;
	if (bContinue && ww_hasSelection(wp)) {
		incrementalStart = (CARET){ .linePointer = wp->blstart->m_linePointer, .offset = wp->blstart->m_column };
		incrementalStart.ln = ln_indexOf(fp, wp->blstart->m_linePointer);
	} else if (incrementalStart.linePointer == NULL ||(incrementalStart.ln = ln_indexOf(fp, incrementalStart.linePointer)) < 0) {
		incrementalStart = wp->caret;
	}
	if (*pszString == 0) {
		if (incrementalStart.linePointer != NULL) {
			bl_hideSelectionInCurrentWindow(wp);
			caret_placeCursorMakeVisibleAndSaveLocation(wp, incrementalStart.ln, incrementalStart.offset);
		}
		return 1;
	}
	if (!(pPattern = regex_compileWithDefault(pszString))) {
		return 0;
	}
	memset(&match, 0, sizeof match);
	ret = find_expressionInCurrentFileStartingFrom(fp, incrementalStart, nDirection, pPattern, nOptions, &ln, &col, &match, &wrapped);
	if (ret) {
		find_updateSelectionToShowMatch(wp, ln, col, &match);
	} else if (pszString[0]) {
		error_showErrorById(IDS_MSGSTRINGNOTFOUND, pszString);
	}
	return ret;
}

/*
 * Start an incremental search operation. This will set the current search position start to the position
 * of the caret in the current file.
 */
int find_startIncrementalSearch() {
	incrementalStart.linePointer = NULL;
	return TRUE;
}

/*--------------------------------------------------------------------------
 * find_expressionAgainInCurrentFile()
 */
int find_expressionAgainInCurrentFile(WINFO* wp, int dir) {
	RE_PATTERN* pPattern;

	if (_currentSearchAndReplaceParams.compiled.compiledExpression == NULL && !_currentSearchAndReplaceParams.searchPattern[0]) {
		hist_getCurrentHistoryEntry(SEARCH_PATTERNS, _currentSearchAndReplaceParams.searchPattern, sizeof _currentSearchAndReplaceParams.searchPattern);
		if (!_currentSearchAndReplaceParams.searchPattern[0]) {
			error_showErrorById(IDS_MSGNOSEARCHSTRING);
			return 0;
		}
	}
	pPattern = _currentSearchAndReplaceParams.compiled.compiledExpression ? &_currentSearchAndReplaceParams.compiled : 
		regex_compileWithDefault(_currentSearchAndReplaceParams.searchPattern);
	return pPattern && find_expressionInCurrentFile(wp, dir, _currentSearchAndReplaceParams.searchPattern, pPattern, _currentSearchAndReplaceParams.options);
}

/*--------------------------------------------------------------------------
 * ft_countlinesStartingFromDirection()
 * Count all lines marked with the LNMARKED_FOR_COUNTING flag and reset the flag
 * on the corresponding lines as a side effect.
 */
long ft_countlinesStartingFromDirection(FTABLE* fp, long start, int dir) {	
	register long nl = 0;
	register LINE *lp;
	
	lp = ln_goto(fp,start);
	if (dir > 0) {
		while (lp->next) {
			if (lp->lflg & LN_MARKED_FOR_COUNTING) {
				nl++, lp->lflg &= ~LN_MARKED_FOR_COUNTING;
			}
			lp = lp->next;
		}
	} else {
		while (lp) {
			if (lp->lflg & LN_MARKED_FOR_COUNTING) {
				nl++, lp->lflg &= ~LN_MARKED_FOR_COUNTING;
			}
			lp = lp->prev;
		}
	} 
	return (nl);
}

/*
 * Returns the character used to fill "spaces" (inserted instead of a tab character) - typically ' '. 
 */
char ft_getSpaceFillCharacter(WINFO* wp) {
	FTABLE* fp = wp->fp;
	char chSpace = fp->documentDescriptor->expandTabsWith;
	if (!chSpace) {
		chSpace = ' ';
	}
	return chSpace;
}

/*--------------------------------------------------------------------------
 * ft_expandTabsWithSpaces()
 * Expand tabs in a given source buffer with a source len and replace with spaces and write
 * the result to a destination buffer. Return the result size of the destination buffer or -1
 * on failure.
 */
int ft_expandTabsWithSpaces(WINFO* wp, char* pszDest, size_t nDestLen, const char* pszSource, size_t nSourceLen, long *nt)
{	unsigned char *d    = pszDest,
			    *dend = &pszDest[nDestLen],c;
	const char *s  = pszSource, *send = &pszSource[nSourceLen];
	int col;

	char chSpace = ft_getSpaceFillCharacter(wp);
	while(s < send) {
		if (d >= dend) {
			return -1;
		}
		if ((c = *s++) == '\t') {
			col = (int)(d - _linebuf);
			col = indent_calculateNextTabStop(col, &wp->indentation) - col;
			memset(d,chSpace, col);
			(*nt)++;
			d += col;
		} else
			*d++ = c;
	}
	return (int)(d-_linebuf);
}

/*
 * Try to "optimize" spaces with tab characters in a given string. Return the number of tabs inserted as a replacement
 * for the corresponding spaces in nt and the total len of the resulting buffer as a return value.
 */
int ft_compressSpacesToTabs(WINFO* wp, char* pszDest, size_t nDestLen, const char* pszSource, size_t nSourceLen, long* nt) {
	const char* s = pszSource;
	char* pszDestStart = pszDest;
	const char* pszEnd = &s[nSourceLen];
	const char* pszDestEnd = &pszSource[nDestLen-1];
	const char* pszFound;
	int   col;
	int   nextTabStop;

	s = pszSource;
	col = 0;
	char chSpace = ft_getSpaceFillCharacter(wp);
	while (s < pszEnd && pszDest < pszDestEnd) {
		nextTabStop = indent_calculateNextTabStop(col, &wp->indentation);
		pszFound = NULL;
		char c = *s++;
		if (c == '\t') {
			col = nextTabStop;
		} else if (c == chSpace) {
			s--;
			for (int i = 0; s + i < pszEnd; i++) {
				if (i + col == nextTabStop) {
					*pszDest++ = '\t';
					s += i;
					col = nextTabStop;
					(* nt)++;
					break;
				}
				if (s[i] != chSpace) {
					// Cannot compress
					while (--i >= 0) {
						*pszDest++ = *s++;
						col++;
					}
					break;
				}
			}
			continue;
		} else {
			col++;
		}
		*pszDest++ = c;
	}
	return (int)(pszDest - pszDestStart);
}

/*--------------------------------------------------------------------------
 * edit_expandTabsInLineWithSpaces()
 */
static LINE *edit_expandTabsInLineWithSpaces(WINFO *wp, LINE *lp,long *nt)
{	long t = 0;
	int  size;

	size = ft_expandTabsWithSpaces(wp, _linebuf, LINEBUFSIZE, lp->lbuf, lp->len, &t);
	if (t) {
		*nt += t;
		if ((lp = ln_modify(wp->fp, lp, lp->len, size)) == 0L) {
			return 0;
		}
		lp->lflg |= LN_MARKED_FOR_COUNTING;
		memmove(lp->lbuf,_linebuf,size);
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * find_expandTabsInFormattedLines()
 */
LINE *find_expandTabsInFormattedLines(WINFO *wp, LINE *lp)
{	long t;

	if (PLAINCONTROL(wp->dispmode)) {
		return lp;
	}
	return edit_expandTabsInLineWithSpaces(wp,lp,&t);
}

/*--------------------------------------------------------------------------
 * edit_compactLineSpacingWithTabs()
 */
static LINE *edit_compactLineSpacingWithTabs(WINFO *wp, LINE *lp,long *nt)
{
	long ntabs;

	int nSize = ft_compressSpacesToTabs(wp, _linebuf, LINEBUFSIZE, lp->lbuf, lp->len, &ntabs);
	if (ntabs > 0) {
		if ((lp = ln_modify(wp->fp, lp, lp->len, nSize)) == 0L) {
			return 0;
		}
		lp->lflg |= LN_MARKED_FOR_COUNTING;
		memcpy(lp->lbuf, _linebuf, nSize);
		render_repaintLine(wp->fp, lp);
	}
	return lp;
}

/*--------------------------------------------------------------------------
 * find_modifyTextSection()
 * modify a section of a text (identified by block marks).
 */
static void find_modifyTextSection(WINFO *wp, LINE *(*func)(WINFO *wp, LINE *lp, long *nt),
				  long *cntel,long *cntln,MARK *mps, MARK *mpe) {
	LINE     *lp;
	FTABLE* fp = wp->fp;
	lp = mps->m_linePointer;

	for (; lp != 0 && (mpe->m_column != 0 || lp != mpe->m_linePointer); lp = lp->next) {
		if ((lp = (*func)(wp,lp,cntel)) == 0)
			break;
		if (find_abortProgress() || lp == mpe->m_linePointer)
			break;
	}
	caret_placeCursorInCurrentFile(wp, wp->caret.ln,0L);
	if (*cntel) {
		render_repaintCurrentFile();
		*cntln = ft_countlinesStartingFromDirection(fp,*cntln,1);
	}
	mouse_setDefaultCursor();
}

/*--------------------------------------------------------------------------
 * find_selectRangeWithMarkers()
 */
int find_selectRangeWithMarkers(RANGE_TYPE rngdefault, MARK** mps, MARK** mpe)
{
	WINFO* wp = ww_getCurrentEditorWindow();

	if (!wp ||
		find_setTextSelection(wp, rngdefault, mps, mpe) == RNG_INVALID)
		return 0;
	return 1;
}

/*--------------------------------------------------------------------------
 * find_replaceTabsWithSpaces()
 * flg = 1 : expand TABS to SPACES
 * flg = 0 : comp SPACES to TABS
 */
int find_replaceTabsWithSpaces(RANGE_TYPE scope, CT_OPTION flg)
{	long nt=0L,nl=0L;
	MARK *mps,*mpe;

	if (!find_selectRangeWithMarkers(scope,&mps,&mpe))
		return 0;

	progress_startMonitor(IDS_ABRTCONVERT, 1000);
	find_modifyTextSection(ww_getCurrentEditorWindow(),(flg) ? edit_expandTabsInLineWithSpaces : edit_compactLineSpacingWithTabs,&nt,&nl,mps,mpe);
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
		if (all || (lc->lflg & LNMODIFIED)) {
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
 * strxcpy()
 */
static void strxcpy(char *d, char *s, int newlen)
{
	while(--newlen >= 0)
		*d++ = *s++;
}

/*--------------------------------------------------------------------------
 * edit_replaceText()
 * replace, mark, count... lines with RE
 */
REPLACE_TEXT_RESULT edit_replaceText(WINFO* wp, const char* pszSearchPattern, const char* pszReplaceWith, int nOptions, int scope, REPLACE_TEXT_ACTION action) {
	long 	ln, col, startln, lastfln;
	long    nReplacements = 0L;
	unsigned char 	*q;
	LINE *lp;
	FTABLE *fp;
	MARK*		pMarkStart;
	MARK*		pNewMarkEnd;
	MARK *		pMarkEnd;
	LINE		*oldxpnd = 0;
	long newlen;
	int maxlen,delta;
	RE_MATCH	match;
	int			olen;
	REPLACE_TEXT_RESULT bResult = RTR_FINISHED;
	int  		splflg = _playing, column = 0, lastfcol, query,marked;

	memset(&match, 0, sizeof match);
	fp = wp->fp;

	if (ft_checkReadonlyWithError(fp) && action == REP_REPLACE) {
		return RTR_ERROR;
	}
	if (!pszReplaceWith) {
		return RTR_ERROR;
	}
	regex_compileWithDefault(pszSearchPattern);
	find_initializeReplaceByExpression(pszReplaceWith);
	newlen = _currentReplacementPattern.preparedReplacementString ? (long)strlen(_currentReplacementPattern.preparedReplacementString) : 0;

	undo_startModification(fp);
	caret_saveLastPosition();

	if (find_setTextSelection(wp, scope,&pMarkStart,&pNewMarkEnd) == RNG_INVALID)
		return 0;
	/* force register use */
	pMarkEnd = pNewMarkEnd;

	lp  = pMarkStart->m_linePointer;
	col = pMarkStart->m_column;
	startln = ln = ln_indexOf(fp,lp);

	lastfln = wp->caret.ln;
	lastfcol = wp->caret.offset;

	query  = nOptions & RE_CONFIRM_REPLACEMENT;
	marked = nOptions & RE_CONSIDER_MARKED_LINES;
	if (ww_isColumnSelectionMode(wp) && scope == RNG_BLOCK)
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
		progress_startMonitor(IDS_ABRTREPLACE, 1000);
	}

	while(lp) {

		if (column) {
			if (marked && (lp->lflg & LNXMARKED) == 0)
				goto nextline;
			if (P_NE(lp,oldxpnd)) {
				if ((lp = find_expandTabsInFormattedLines(wp,lp)) == 0)
					break;
				oldxpnd = lp;
			}
			if (col < wp->blcol1)
				col = wp->blcol1;
			maxlen = (wp->blcol2 < lp->len) ? wp->blcol2 : lp->len;
		} else
			maxlen = lp->len;

		if (P_EQ(lp,pMarkEnd->m_linePointer)) {
			if (!column) {
				maxlen = pMarkEnd->m_column;
				if (maxlen > lp->len)
					maxlen = lp->len;
			}
			if (col >= maxlen)
				break;
		}

		if (col) {
			if (!match.circf && regex_match(&_currentSearchAndReplaceParams.compiled, &lp->lbuf[col], &lp->lbuf[maxlen], &match))
				goto success;
		} else {
			if (marked && (lp->lflg & LNXMARKED) == 0)
				goto nextline;
			if (find_abortProgress())
				break;
			if (regex_match(&_currentSearchAndReplaceParams.compiled, lp->lbuf, &lp->lbuf[maxlen], &match))
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
			lp->lflg |= LNMODIFIED;
		}

nextline:
		if (P_EQ(lp,pMarkEnd->m_linePointer))
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
			ln_errorLineTooLong();
			break;
		}
		q = _linebuf;

		lastfln  = ln;
		lastfcol = col;

		if (query) {
			find_updateSelectionToShowMatch(wp,ln,col, &match);
			switch (dlg_queryReplace(match.loc1,olen,q,(int)newlen)) {
				case IDNO:
					delta = olen;
					goto advance;
				case IDD_ALL_OCCURRENCES:
					query = 0;
					bResult = RTR_ALL;
				case IDOK: break;
				default:
					bResult = RTR_CANCELLED;
					goto endrep;
			}
		}

		if ((lp = ln_modify(fp,lp,
					    (int)(col+olen),
		                   (int)(col+newlen))) == 0L)
			break;

		strxcpy(&lp->lbuf[col],q, (int)newlen);

		if (query)
			render_repaintCurrentLine(wp);

		delta = (int)newlen;
	advance1:
		nReplacements++;
		ln_markModified(lp);
		lp->lflg |= LN_MARKED_FOR_COUNTING;
		if (scope == RNG_ONCE)
			break;

advance:	if (delta <= 0 && olen <= 0)	/* empty expr. glitch */
			col++;
		else
			col += delta;
		
		if (!lp) {
			break;
		}
		if (col >= lp->len || (column && col >= maxlen))
			goto nextline;
	}

endrep:
	if (!(query || scope == RNG_ONCE)) {
		progress_closeMonitor(0);
	}
	dlg_closeQueryReplace();
	_playing = splflg;

	if (nReplacements) {
		if (action != REP_COUNT) {
			BOOL bKeepCaret = nOptions & RE_KEEP_CARET;
			if (_currentReplacementPattern.lineSplittingNeeded) {
				caret_placeCursorInCurrentFile(wp, startln,0L);
				ln = breaklines(fp,0,startln,ln);
				if (!bKeepCaret || ln == wp->caret.ln) {
					caret_placeCursorMakeVisibleAndSaveLocation(wp, ln, col);
				}
				render_repaintAllForFile(fp);
			} else {
				if (!bKeepCaret || lastfln == wp->caret.ln) {
					caret_placeCursorMakeVisibleAndSaveLocation(wp, lastfln, lastfcol);
				}
				if (scope == RNG_ONCE && action == REP_REPLACE) {
					render_repaintCurrentLine(wp);
				} else {
					render_repaintAllForFile(fp);
				}
			}
		}
		/* ft_countlinesStartingFromDirection -> clear change line flags */
		ln = ft_countlinesStartingFromDirection(fp, startln, 1);
		if (action != REP_REPLACE) {
			error_showMessageInStatusbar(IDS_MSGNFOUND, pszSearchPattern, ln, nReplacements);
		} else {
			error_showMessageInStatusbar(IDS_MSGNREPL, pszSearchPattern, nReplacements, ln);
		}
	} else if (bResult != RTR_CANCELLED) {
		error_showErrorById(IDS_MSGSTRINGNOTFOUND, pszSearchPattern);
		return RTR_NOT_FOUND;
	}
	return bResult;
}

/*--------------------------------------------------------------------------
 * find_setTextSelection()
 * 
 * Select a range of text in the file identified by fp.
 */
int find_setTextSelection(WINFO *wp, RANGE_TYPE rngetype, MARK **markstart, MARK **markend) {
	LINE *lps,*lpe;
	int  ofs,ofe;
	FTABLE* fp = wp->fp;

	/*
	 * if rnge == RNG_FREE, range is already selected, return MARK positions
	 */
	if (rngetype == RNG_INVALID)
		return RNG_INVALID;

	if ( rngetype == RNG_FREE &&
	   ((*markstart = mark_find(wp,MARKSELSTART)) == 0 ||
	    (*markend   = mark_find(wp,MARKSELEND))   == 0)) {
	    	error_showErrorById(IDS_MSGNORANGESELECTED);
		return RNG_INVALID;
	}

	/*
	 * make range marks: initial is .,$ (RNG_FROMCURS,RNG_ONCE)
	 */

	lps = wp->caret.linePointer;
	ofs = wp->caret.offset;
	lpe = fp->lastl->prev;
	ofe = lpe->len;

	switch(rngetype) {
		case RNG_LINE:
			ofs = 0;
			lpe = lps;
			ofe = lpe->len;
			break;
		case RNG_CHAPTER:
			lps = lpe = wp->caret.linePointer;
			while(!ln_lineIsEmpty(lps) && lps->prev)
				lps = lps->prev;
			while(!ln_lineIsEmpty(lpe) && lpe->next != fp->lastl)
				lpe = lpe->next;
			ofs = 0;
			ofe = lpe->len;
			break;
		case RNG_BLOCK_LINES:
		case RNG_BLOCK:
			if (!ww_checkSelectionWithError(wp))
				return RNG_INVALID;
			lps = wp->blstart->m_linePointer;
			lpe = wp->blend->m_linePointer;
			if (ww_isColumnSelectionMode(wp)) {
				ofs = wp->blcol1;
				ofe = wp->blcol2;
			} else {
				ofs = wp->blstart->m_column;
				ofe = wp->blend->m_column;
				if (rngetype == RNG_BLOCK_LINES && ofe == 0) {
					lpe = lpe->prev;
				}
			}
			break;
		case RNG_TOCURS:
			lpe = wp->caret.linePointer;
			ofe = wp->caret.offset;
			/* drop through */
		case RNG_GLOBAL:
			ofs = 0;
			lps = fp->firstl;
	}

	if ((*markstart = mark_set(wp,lps,ofs,MARKSELSTART)) == 0 ||
	    (*markend = mark_set(wp,lpe,ofe,MARKSELEND)) == 0)
		return RNG_INVALID;

	return rngetype;
}

