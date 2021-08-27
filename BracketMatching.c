/*
 * BracketMatching.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose:
 * check brackets, uc_showMatchingBracket, bracket indent, shift between brackets...
 * shift parts of text translate character up 2 low and vc.vs.
 *
 * 										created: 20.02.87
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "trace.h"
#include "caretmovement.h"
#include "winfo.h"
#include "edierror.h"
#include "linkedlist.h"
#include "regexp.h"
#include "findandreplace.h"
#include "brackets.h"
#include "grammar.h"
#include "pksedit.h"
#include "publicapi.h"

/*--------------------------------------------------------------------------
 * EXTERNALS
 */

extern long	_multiplier;
extern int	edit_calculateTabs2Columns(INDENTATION *pIndent, int tabs);

extern PASTELIST 	*_esclist[];

/*--------------------------------------------------------------------------
 * uc_matchBracket()
 */
static BOOL uc_matchBracket(char *lineBuf, char *bracketPattern) {	
	char c;
	size_t patternLength = strlen(bracketPattern) - 1;
	c = lineBuf[0];
	if (patternLength < 0) {
		return FALSE;
	}
	while ((c = *bracketPattern++) != 0) {
		if (c != *lineBuf++) {
			return FALSE;
		}
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 * uc_find()
 */
EXPORT UCLIST *uc_find(GRAMMAR* pGrammar, char *lineBuffer, int column)
{	UCLIST *up = grammar_getUndercursorActions(pGrammar);
	int o;

	while (up) {
		if ((o = column - up->len) < 0) {
			up = up->next;
			continue;
		}
		unsigned char* pszString = &lineBuffer[o];
		if (up->action == UA_SHOWMATCH) {
			if (uc_matchBracket(pszString, up->p.uc_bracket->lefthand) || uc_matchBracket(pszString, up->p.uc_bracket->righthand)) {
				return up;
			}
		} else if (uc_matchBracket(pszString, up->pat)) {
			return up;
		}
		up = up->next;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * bracketmatch()
 */
static int _righthand;
static int bracketmatch(char *s, BRACKET_RULE *mp)
{
	if (uc_matchBracket(s,mp->lefthand)) {
		_righthand = 0;
		return 1;
	}

	if (uc_matchBracket(s,mp->righthand)) {
		_righthand = 1;
		return 1;
	}

	return 0;
}

/*--------------------------------------------------------------------------
 * ismatch()
 */
static BRACKET_RULE *_lastmatch;
static BRACKET_RULE *ismatch(UCLIST* mp, char *s) {	
	for (; mp; mp = mp->next) {
		if (mp->action == UA_SHOWMATCH && bracketmatch(s,mp->p.uc_bracket)) {
			_lastmatch = mp->p.uc_bracket;
			return _lastmatch;
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * scanmatch()
 */
static int scanmatch(int checkright, 
				 LINE *line,  	struct tagBRACKET_RULE *mp,
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
static int br_indentsum(LINE *lps, LINE *lp, BRACKET_RULE *mp, int *dcurr, int *hasind)
{	unsigned char *s,*send;
	int d1,d2,indent = 0;

	d1 = (mp->ci1[0]+mp->ci1[1]);
	d2 = (mp->ci2[0]+mp->ci2[1]);

	s = lp->lbuf;
	send = s+lp->len;
	
	*hasind = 0;

	while(s < send) {
		if (uc_matchBracket(s,mp->lefthand)) {
			*dcurr = mp->ci1[0];
			*hasind = 1;
			break;
		}
		if (uc_matchBracket(s,mp->righthand)) {
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
			if (uc_matchBracket(s,mp->lefthand)) {
				indent += d1;
			} else if (uc_matchBracket(s,mp->righthand)) {
				indent += d2;
			}
		}
		if (lp == lps) {
			break;
		}
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
{	
	int returnValue = -1;

	UCLIST* up = grammar_getUndercursorActions(fp->documentDescriptor->grammar);
	*di = 0;
	while(up) {
		if (up->action == UA_BRINDENT && *(long*)up->p.uc_bracket->ci1) {
			returnValue = indent + br_indentsum(lp1,lpcurr, up->p.uc_bracket,di,hbr);
		}
		up = up->next;
	}

	return returnValue;
}

/*--------------------------------------------------------------------------
 * nextmatch()
 */
static int nextmatch(UCLIST* pList, LINE *lp,long *ln,long *col)
{	register char	*s;
	register int	pos,j;
	BRACKET_RULE *mp;

	pos = *col;
	s = &lp->lbuf[pos];
	if ((mp = ismatch(pList, s)) != 0) 
		return (scanmatch(!_righthand,lp,mp,ln,col));
	s = lp->lbuf;
	for ( j = pos; --j >= 0; )
		if (ismatch(pList, &s[j]) != 0)
			goto matched;
	for ( j = pos; ++j < lp->len; )
		if (ismatch(pList, &s[j]) != 0)
			goto matched;

	return 0;

matched:
	*col = j; return 1;
}

/*------------------------------------------------------------
 * uc_waitForTimerElapsed()
 */
static void uc_waitForTimerElapsed(void)
{
	MSG msg;

	GetMessage(&msg, 0, WM_TIMER, WM_TIMER);
	DispatchMessage(&msg);
}

/*--------------------------------------------------------------------------
 * EdShowMatch()
 * show matching brackets
 */
EXPORT int EdShowMatch(void)
{	WINFO *wp = ww_getCurrentEditorWindow();
	long 	 ln,col;

	if (!wp) return 0;
	FTABLE* fp = wp->fp;
	ln = wp->caret.ln, col = wp->caret.offset;
	UCLIST* up = grammar_getUndercursorActions(fp->documentDescriptor->grammar);
	if (up && nextmatch(up, wp->caret.linePointer,&ln,&col)) {
		caret_placeCursorInCurrentFile(wp, ln,col);
		return 1;
	} 
	error_showErrorById(IDS_MSGNOBRACKETS);
	return 0;
}

/*--------------------------------------------------------------------------
 * uc_showMatchingBracket(s)
 * tries to highlight the matching bracket after a character had been inserted.
 */
EXPORT int uc_showMatchingBracket(WINFO* wp) {	
	FTABLE* fp = wp->fp;
	LINE* lp = wp->caret.linePointer;
	long   ln  = wp->caret.ln;
	long   col = wp->caret.offset;
	UCLIST *up;
	long lsav = ln, csav = col;

	if (col > 0) {
		// check character left to cursor.
		col--;
	}
	if ((up = uc_find(fp->documentDescriptor->grammar,lp->lbuf,col)) != 0 && up->action == UA_SHOWMATCH) {
		if (scanmatch(0,lp, up->p.uc_bracket,&ln,&col)) {
			if (ln >= wp->minln) {
				caret_placeCursorInCurrentFile(wp, ln,col);
				uc_waitForTimerElapsed();
				caret_placeCursorInCurrentFile(wp, lsav,csav);
				return 1;
			}
		} else {
			error_showErrorById(IDS_MSGNOBRACKETS);
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
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE *fp = wp->fp;

	lp	= wp->caret.linePointer;
	offs = wp->caret.offset;
	c    = lp->lbuf[offs];
	if (((c1 = _l2uset[c]) != c || (c1 = _u2lset[c]) != c) &&
	    (lp = ln_modify(fp,lp,offs,offs)) != (LINE *)0) {
		lp->lbuf[offs] = c1;
		render_repaintCurrentLine(wp);
	}
	return EdCursorRight(1);
}

/*--------------------------------------------------------------------------
 * uc_shiftLinesByIndent()
 * shift n lines
 */
EXPORT int uc_shiftLinesByIndent(WINFO *wp, long ln, long nlines, int dir)
{	register LINE	*lp;
	register char	*s,*send;
	register long  i,offset;
	size_t			blcount;
	register int	ind,col;
	int 			dummy;
	FTABLE* fp = wp->fp;

	if ((lp = ln_goto(fp,ln)) == 0L) 
		return 0;

	for (i = 0; i < nlines; i++) {
		if (!lp->next) break;
		ind = edit_calculateTabs2Columns(&wp->indentation,abs(dir));
		if (dir > 0) {
			if ((lp = ln_insertIndent(wp,lp,ind,&dummy)) == 0L) {
				return 0;
			}
		} else {
			s = lp->lbuf;
			send = s+lp->len;
			col = 0;
			while(col < ind && s < send) {
				if (*s != ' ') {
					if (*s == '\t')
						col = indent_calculateTabStop(col, &wp->indentation);
					else
						break;
				} else {
					col++;
				}
				s++;
			}
			blcount = s - lp->lbuf;
			if (blcount) {
				if ((lp = ln_modify(fp,lp,(int) blcount,0)) == 0L)
					return 0;
			} 
		}
		lp = lp->next;
	}
	render_repaintAllForFile(fp);

	offset = wp->caret.offset;
	if (wp->caret.ln >= ln && wp->caret.ln < ln+nlines)
		offset += dir;

	caret_placeCursorInCurrentFile(wp, wp->caret.ln,offset);

	return 1;
}

/*--------------------------------------------------------------------------
 * EdShiftBetweenBrackets()
 * shift the text between brackets
 */
EXPORT int EdShiftBetweenBrackets(int dir)
{	register FTABLE *fp;
	WINFO* wp = ww_getCurrentEditorWindow();
	register long ln2;
	long ln,col;
	
	fp = wp->fp;
	ln = wp->caret.ln, col = wp->caret.offset;

	UCLIST* up = uc_find(fp->documentDescriptor->grammar, wp->caret.linePointer->lbuf, col);
	if (!nextmatch(up, wp->caret.linePointer,&ln,&col)) {
		return 0;
	}

	if (ln > wp->caret.ln) {
		ln2 = ln;
		ln  = wp->caret.ln;
	} else {
		ln2 = wp->caret.ln;
	}
	
	/* care indentation style */
	if (_lastmatch) {
		ln  += _lastmatch->ci1[1];
		ln2 -= _lastmatch->ci2[1];
	}

	return uc_shiftLinesByIndent(wp,ln,ln2-ln,dir);
}

/*--------------------------------------------------------------------------
 * EdLinesShift()
 * shift _multiplier lines
 */
EXPORT int EdLinesShift(int dir)
{	WINFO *wp = ww_getCurrentEditorWindow();

	if (!wp) {
		return 0;
	}
	return uc_shiftLinesByIndent(wp,wp->caret.ln,_multiplier,dir);
}

/*--------------------------------------------------------------------------
 * uc_shiftRange()
 * shift text range
 */
EXPORT int uc_shiftRange(int scope, int dir)
{	MARK	*mps,*mpe;
	WINFO *wp = ww_getCurrentEditorWindow();
	long	ln1,ln2;
	
	if (!find_selectRangeWithMarkers(scope,&mps,&mpe))
		return 0;

	if (dir == 0)
		dir = -1;

	FTABLE* fp = wp->fp;
	ln1 = ln_cnt(fp->firstl,mps->m_linePointer)-1;
	ln2 = ln_cnt(mps->m_linePointer,mpe->m_linePointer);
	return uc_shiftLinesByIndent(wp,ln1,ln2,dir);
}

/*
 * Either insert a tab or shift the selected lines back and forth.
 */
EXPORT int edit_shiftSelection(int aDirection) {
	return uc_shiftRange(RNG_BLOCK_LINES, aDirection);
}


