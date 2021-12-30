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
#include "winterf.h"
#include "stringutil.h"

/*--------------------------------------------------------------------------
 * EXTERNALS
 */
extern long	_multiplier;
extern int	edit_calculateTabs2Columns(INDENTATION *pIndent, int tabs);

extern PASTELIST 	*_esclist[];

/*--------------------------------------------------------------------------
 * uc_matchBracket()
 */
static BOOL uc_matchBracket(char *lineBuf, char *bracketPattern, BOOL bCaseIgnore) {	
	if (!bracketPattern[0]) {
		return FALSE;
	}
	return string_compareWithSecond(lineBuf, bracketPattern, bCaseIgnore) == 0;
}

/*--------------------------------------------------------------------------
 * uc_find()
 */
EXPORT UCLIST *uc_find(GRAMMAR* pGrammar, char *lineBuffer, int column)
{	UCLIST *up = grammar_getUndercursorActions(pGrammar);
	int o;
	int nBest = 0;
	UCLIST* upBest = NULL;

	while (up) {
		o = column - up->len;
		unsigned char* pszString = o < 0 ? NULL : &lineBuffer[o];
		BOOL bMatch = FALSE;
		if (up->action == UA_SHOWMATCH) {
			if (pszString && uc_matchBracket(pszString, up->p.uc_bracket->lefthand, up->p.uc_bracket->ignoreCase)) {
				bMatch = TRUE;
			}
			int nLen = (int)strlen(up->p.uc_bracket->righthand);
			if ((o = column - nLen) >= 0 && nLen > nBest) {
				pszString = &lineBuffer[o];
				if (uc_matchBracket(pszString, up->p.uc_bracket->righthand, up->p.uc_bracket->ignoreCase)) {
					bMatch = TRUE;
				}
			}
		} else if (pszString && uc_matchBracket(pszString, up->pat, up->ignoreCase)) {
			bMatch = TRUE;
		}
		if (bMatch && up->len > nBest) {
			upBest = up;
			nBest = up->len;
		}
		up = up->next;
	}
	return upBest;
}

/*--------------------------------------------------------------------------
 * bracketmatch()
 */
static int _righthand;
static int bracketmatch(char *s, BRACKET_RULE *mp)
{
	if (uc_matchBracket(s,mp->lefthand, mp->ignoreCase)) {
		_righthand = 0;
		return 1;
	}

	if (uc_matchBracket(s,mp->righthand, mp->ignoreCase)) {
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
				 LINE *line, BRACKET_RULE *mp,
				 long *Ln,	long *Col)
{	long ln = *Ln;
	char *s = &line->lbuf[*Col],*send;
	int  level;

	/*
	 * check for right bracket
	 */
	if (checkright) {
		level = 1;
		while (line->next) {
			send = &line->lbuf[line->len];
			while (s < send) {
				if (bracketmatch(s,mp)) {
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
				if (bracketmatch(s,mp)) {
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
static int br_indentsum(const LINE *lps, LINE *lp, BRACKET_RULE *mp, int *dcurr, int *hasind)
{	unsigned char *s,*send;
	int d1,d2,indent = 0;

	d1 = (mp->ci1[0]+mp->ci1[1]);
	d2 = (mp->ci2[0]+mp->ci2[1]);

	s = lp->lbuf;
	send = s+lp->len;
	
	*hasind = 0;

	while(s < send) {
		if (uc_matchBracket(s,mp->lefthand, mp->ignoreCase)) {
			*dcurr = mp->ci1[0];
			*hasind = 1;
			break;
		}
		if (uc_matchBracket(s,mp->righthand, mp->ignoreCase)) {
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
			if (uc_matchBracket(s,mp->lefthand, mp->ignoreCase)) {
				indent += d1;
			} else if (uc_matchBracket(s,mp->righthand, mp->ignoreCase)) {
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
 * nextmatch()
 */
static int nextmatch(UCLIST* pList, LINE *lp,long *ln,long *col)
{	register char	*s;
	register int	pos,j;
	BRACKET_RULE *mp;

	pos = *col;
	s = &lp->lbuf[pos];
	if ((mp = ismatch(pList, s)) != 0) {
		if (!_righthand) {
			*col += (int)strlen(mp->lefthand);
		}
		return (scanmatch(!_righthand, lp, mp, ln, col));
	}
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
static void uc_waitForTimerElapsed(int ms) {
	MSG msg;

	// Set the default timer to wait for ms seconds.
	SetTimer(hwndMain, TIM_FRAME, ms, NULL);
	while (GetMessage(&msg, 0, 0, 0)) {
		if (msg.message == WM_TIMER) {
			break;
		}
		DispatchMessage(&msg);
	}
	// Re-set the default timer to wait for TIMER_INTERVAL seconds.
	SetTimer(hwndMain, TIM_FRAME, TIMER_INTERVALL, NULL);
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

	if ((up = uc_find(fp->documentDescriptor->grammar,lp->lbuf,col)) != 0 && up->action == UA_SHOWMATCH) {
		int nOffs = (int)strlen(up->p.uc_bracket->righthand);
		col -= nOffs;
		if (nOffs <= 0) {
			lp = lp->prev;
			if (lp) {
				col = lp->len;
			}
		}
		if (lp && scanmatch(0,lp, up->p.uc_bracket,&ln,&col)) {
			if (ln >= wp->minln) {
				caret_placeCursorInCurrentFile(wp, ln,col);
				uc_waitForTimerElapsed(300);
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


