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
#include "edctype.h"
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
/*
 * Calculate the lexical start state at a given line.
 */
extern LEXICAL_CONTEXT highlight_getLexicalStartStateFor(HIGHLIGHTER* pHighlighter, WINFO* wp, LINE* lp);

extern PASTELIST 	*_esclist[];

/*--------------------------------------------------------------------------
 * uc_matchBracket()
 */
static BOOL uc_matchBracket(GRAMMAR* pGrammar, LINE* pLine, int nOffset, UC_MATCH_PATTERN* pPattern) {	
	if (nOffset < 0 || !pPattern->pattern[0]) {
		return FALSE;
	}
	const char* pBuffer = &pLine->lbuf[nOffset];
	BOOL bMatch = FALSE;
	if (pPattern->rePattern) {
		RE_MATCH matchResult;
		bMatch = regex_match(pPattern->rePattern, pBuffer, &pLine->lbuf[pLine->len], &matchResult);
	} else if (string_compareWithSecond(pBuffer, pPattern->pattern, pPattern->ignoreCase) == 0) {
		// special case - start end patterns like begin and end in pascal - make sure, they start
		// on a word boundary - RE would make us more flexible here.
		if (pks_isalpha(pPattern->pattern[0])) {
			if (nOffset != 0 && pks_isalpha(pBuffer[-1])) {
				return FALSE;
			}
			int nO2 = (int)(nOffset + strlen(pPattern->pattern));
			if (nO2 < pLine->len && pks_isalpha(pLine->lbuf[nO2])) {
				return FALSE;
			}
		}
		bMatch = TRUE;
	}
	// TODO: determine start state using LEXICAL_CONTEXT lcStart = highlight_getLexicalStartStateFor() ... would
	// better help to eliminate multiline comment matches.
	if (bMatch &&  grammar_getLexicalContextAt(pGrammar, LC_START, pLine->lbuf, pLine->len, nOffset) == LC_START) {
		return TRUE;
	}

	return FALSE;
}

/*--------------------------------------------------------------------------
 * uc_find()
 */
EXPORT UCLIST *uc_find(GRAMMAR* pGrammar, LINE *lp, int column) {	
	UCLIST *up = grammar_getUndercursorActions(pGrammar);
	int o;
	int nBest = 0;
	UCLIST* upBest = NULL;
	char* lineBuffer = lp->lbuf;

	while (up) {
		UC_MATCH_PATTERN* pPattern = &up->uc_pattern;
		o = column - pPattern->len;
		BOOL bMatch = FALSE;
		if (up->action == UA_SHOWMATCH) {
			if (uc_matchBracket(pGrammar, lp, o, pPattern)) {
				bMatch = TRUE;
			}
			int nLen = up->p.uc_bracket->righthand.len;
			if ((o = column - nLen) >= 0 && nLen > nBest) {
				if (uc_matchBracket(pGrammar, lp, o, &up->p.uc_bracket->righthand)) {
					bMatch = TRUE;
				}
			}
		} else if (uc_matchBracket(pGrammar, lp, o, pPattern)) {
			bMatch = TRUE;
		}
		if (bMatch && pPattern->len > nBest) {
			upBest = up;
			nBest = pPattern->len;
		}
		up = up->next;
	}
	return upBest;
}

/*--------------------------------------------------------------------------
 * uc_matchBracketRule()
 */
static int uc_matchBracketRule(GRAMMAR* pGrammar, LINE* lp, int nOffset, BRACKET_RULE *mp, int *pIsRightHand) {
	if (uc_matchBracket(pGrammar, lp, nOffset, &mp->lefthand)) {
		*pIsRightHand = 0;
		return 1;
	}

	if (uc_matchBracket(pGrammar, lp, nOffset, &mp->righthand)) {
		*pIsRightHand = 1;
		return 1;
	}

	return 0;
}

/*--------------------------------------------------------------------------
 * uc_findMatchinBracketRule()
 */
static BRACKET_RULE *uc_findMatchinBracketRule(GRAMMAR* pGrammar, UCLIST* mp, LINE* lp, int nOffset, int *pIsRightHand) {	
	for (; mp; mp = mp->next) {
		if (mp->action == UA_SHOWMATCH && uc_matchBracketRule(pGrammar, lp, nOffset, mp->p.uc_bracket, pIsRightHand)) {
			return mp->p.uc_bracket;
		}
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * br_findMatching()
 * Find the matching bracket. Returns 1 if successful, 0 otherwise
 */
static int br_findMatching(GRAMMAR* pGrammar, int checkright, LINE *lp, BRACKET_RULE *mp, long *pLine, long *pColumn) {
	long ln = *pLine;
	int righthand;
	char *s = &lp->lbuf[*pColumn],*send;
	int  level;

	// check for right bracket
	if (checkright) {
		level = 1;
		while (lp->next) {
			send = &lp->lbuf[lp->len];
			while (s < send) {
				if (uc_matchBracketRule(pGrammar, lp, (int)(s-lp->lbuf), mp, &righthand)) {
					if (righthand) {
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
			lp = lp->next;
			ln++, s = lp->lbuf;
		}
	}

	// check for left bracket
	else {
	 	level = -1;
		for(;;) {
			send = lp->lbuf;
			while (--s >= send) {
				if (uc_matchBracketRule(pGrammar, lp, (int)(s - lp->lbuf), mp, &righthand)) {
					if (!righthand) {
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
			if ((lp = lp->prev) == 0L) break;
			ln--;
			s = &lp->lbuf[lp->len];
		}
	}
	return 0;			/* no match */
success:
	 *pLine = ln ,*pColumn = (long)(s - lp->lbuf);
	 return 1;
}

/*--------------------------------------------------------------------------
 * br_indentsum()
 */
static int br_indentsum(GRAMMAR* pGrammar, const LINE *lps, LINE *lp, BRACKET_RULE *mp, int *dcurr, int *hasind)
{	unsigned char *s,*send;
	int d1,d2,indent = 0;

	d1 = (mp->ci1[0]+mp->ci1[1]);
	d2 = (mp->ci2[0]+mp->ci2[1]);

	s = lp->lbuf;
	send = s+lp->len;
	
	*hasind = 0;

	while(s < send) {
		if (uc_matchBracket(pGrammar, lp, (int)(s-lp->lbuf), &mp->lefthand)) {
			*dcurr = mp->ci1[0];
			*hasind = 1;
			break;
		}
		if (uc_matchBracket(pGrammar, lp, (int)(s - lp->lbuf), &mp->righthand)) {
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
			if (uc_matchBracket(pGrammar, lp, (int)(s - lp->lbuf),&mp->lefthand)) {
				indent += d1;
			} else if (uc_matchBracket(pGrammar, lp, (int)(s - lp->lbuf), &mp->righthand)) {
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
 * uc_findBracketMatchInLine()
 */
static BRACKET_RULE* uc_findBracketMatchInLine(GRAMMAR* pGrammar, UCLIST* pList, LINE *lp,long *ln,long *col)
{
	int	pos,j;
	int righthand;
	BRACKET_RULE *mp = 0;

	pos = *col;
	if ((mp = uc_findMatchinBracketRule(pGrammar, pList, lp, pos, &righthand)) != 0) {
		if (!righthand) {
			*col += mp->lefthand.len;
		}
		return (br_findMatching(pGrammar, !righthand, lp, mp, ln, col)) ? mp : 0;
	}
	for ( j = pos; --j >= 0; )
		if ((mp = uc_findMatchinBracketRule(pGrammar, pList, lp, j, &righthand)) != 0)
			goto matched;
	for ( j = pos; ++j < lp->len; )
		if ((mp = uc_findMatchinBracketRule(pGrammar, pList, lp, j, &righthand)) != 0)
			goto matched;

	return 0;

matched:
	*col = j; return mp;
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
	GRAMMAR* pGrammar = fp->documentDescriptor->grammar;
	UCLIST* up = grammar_getUndercursorActions(pGrammar);
	if (up && uc_findBracketMatchInLine(pGrammar, up, wp->caret.linePointer,&ln,&col)) {
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
	GRAMMAR* pGrammar = fp->documentDescriptor->grammar;

	if ((up = uc_find(pGrammar,lp,col)) != 0 && up->action == UA_SHOWMATCH) {
		int nOffs = up->p.uc_bracket->righthand.len;
		col -= nOffs;
		if (nOffs <= 0) {
			lp = lp->prev;
			if (lp) {
				col = lp->len;
			}
		}
		if (lp && br_findMatching(pGrammar, 0,lp, up->p.uc_bracket,&ln,&col)) {
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
{	
	WINFO* wp = ww_getCurrentEditorWindow();
	FTABLE* fp = wp->fp;
	GRAMMAR* pGrammar = fp->documentDescriptor->grammar;
	BRACKET_RULE* mp;
	long ln2;
	long ln,col;
	
	ln = wp->caret.ln, col = wp->caret.offset;
	UCLIST* up = uc_find(pGrammar, wp->caret.linePointer, col);
	if ((mp = uc_findBracketMatchInLine(pGrammar, up, wp->caret.linePointer,&ln,&col)) == 0) {
		return 0;
	}

	if (ln > wp->caret.ln) {
		ln2 = ln;
		ln  = wp->caret.ln;
	} else {
		ln2 = wp->caret.ln;
	}
	
	/* care indentation style */
	if (mp) {
		ln  += mp->ci1[1];
		ln2 -= mp->ci2[1];
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
	ln1 = ll_indexOf((LINKED_LIST*)fp->firstl, (LINKED_LIST*)mps->m_linePointer);
	ln2 = ll_indexOf((LINKED_LIST*)mps->m_linePointer, (LINKED_LIST*)mpe->m_linePointer)+1;
	return uc_shiftLinesByIndent(wp,ln1,ln2,dir);
}

/*
 * Either insert a tab or shift the selected lines back and forth.
 */
EXPORT int edit_shiftSelection(int aDirection) {
	return uc_shiftRange(RNG_BLOCK_LINES, aDirection);
}


