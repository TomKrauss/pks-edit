/*
 * SyntaxHighlighting.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: syntax - highlighting
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "alloc.h"

#include "winfo.h"
#include "grammar.h"
#include "syntaxhighlighting.h"
#include "editorfont.h"

#define WINDOW_SIZE		100

typedef unsigned char* (*HIGHLIGHT_CALCULATE)(HIGHLIGHTER* pData, WINFO* wp, LINE* lp, long nLine);

typedef struct tagTOKEN_LINE_CACHE {
	long s_lineNumber;
	LINE* s_linePointer;
	LEXICAL_STATE s_lineEndType;
} TOKEN_LINE_CACHE;

typedef struct tagHIGHLIGHTER {
	HIGHLIGHT_CALCULATE h_calculate;
	unsigned char h_tokenTypeToStyleTable[MAX_TOKEN_TYPE];
	unsigned char* h_styles;
	long h_styleCapacity;
	long h_minLine;
	long h_lastLine;
	GRAMMAR* h_grammar;
	LINE* h_lastLinePointer;
	TOKEN_LINE_CACHE h_lines[WINDOW_SIZE];
} HIGHLIGHTER;

/*
 * Destroy the caches of the highlighter.
 */
static void highlight_destroyCaches(HIGHLIGHTER* pHighlighter) {
	if (pHighlighter->h_styles) {
		free(pHighlighter->h_styles);
		pHighlighter->h_styles = 0;
		pHighlighter->h_styleCapacity = 0;
	}
	memset(pHighlighter->h_lines, 0, sizeof(pHighlighter->h_lines));
}

/*
 * Destroy the syntax highlighter.
 */
void highlight_destroy(HIGHLIGHTER* pHighlighter) {
	highlight_destroyCaches(pHighlighter);
	free(pHighlighter);
}

static void highlight_adjustCachedLineWindow(HIGHLIGHTER* pHighlighter, FTABLE* fp, long nLine) {
	int nDelta = nLine - pHighlighter->h_minLine;
	if (nDelta < 10 || nDelta >= WINDOW_SIZE-10) {
		nLine -= WINDOW_SIZE / 2;
		if (nLine >= fp->nlines) {
			nLine = fp->nlines - WINDOW_SIZE;
		}
		if (nLine < 0) {
			nLine = 0;
		}
		int nDelta = nLine- pHighlighter->h_minLine;
		if (nDelta <= -WINDOW_SIZE || nDelta >= WINDOW_SIZE) {
			highlight_destroyCaches(pHighlighter);
		}
		else if (nDelta < 0) {
			memmove(&pHighlighter->h_lines[-nDelta], pHighlighter->h_lines, (WINDOW_SIZE + nDelta) * sizeof(TOKEN_LINE_CACHE));
			memset(pHighlighter->h_lines, 0, -nDelta * sizeof(TOKEN_LINE_CACHE));
		}
		else if (nDelta > 0) {
			memmove(pHighlighter->h_lines, &pHighlighter->h_lines[nDelta], (WINDOW_SIZE - nDelta) * sizeof(TOKEN_LINE_CACHE));
			memset(&pHighlighter->h_lines[WINDOW_SIZE - nDelta], 0, nDelta * sizeof(TOKEN_LINE_CACHE));
		}
		pHighlighter->h_minLine = nLine;
	}
}

static void highlight_calculateMissingLineEndStates(HIGHLIGHTER* pHighlighter, int startIndex, FTABLE* fp, LINE* lp) {
	LEXICAL_ELEMENT lexicalElements[MAX_LEXICAL_ELEMENT];
	int detectedEnd = 0;
	LINE* lpCurrent = lp;
	
	for (int i = startIndex; i > 0 && lpCurrent->prev; i--) {
		lpCurrent = lpCurrent->prev;
	}
	LEXICAL_STATE lastState = UNKNOWN;
	for (int i = 0; i <= startIndex; i++) {
		int nElements = grammar_parse(pHighlighter->h_grammar, lexicalElements, lastState, lpCurrent->lbuf, lpCurrent->len, &detectedEnd);
		if (nElements > 0) {
			lastState = lexicalElements[nElements - 1].le_state;
		}
		pHighlighter->h_lines[i].s_lineEndType = lastState;
		pHighlighter->h_lines[i].s_lineNumber = i + pHighlighter->h_minLine;
		pHighlighter->h_lines[i].s_linePointer = lpCurrent;
		lpCurrent = lpCurrent->next;
	}
}

static LEXICAL_STATE highlight_getPreviousLineTokenType(HIGHLIGHTER* pHighlighter, FTABLE* fp, LINE* lp, long nLine) {
	if (nLine == 0 || pHighlighter->h_grammar == NULL || !grammar_hasLineSpans(pHighlighter->h_grammar)) {
		return INITIAL;
	}
	int nPreviousIdx = nLine - pHighlighter->h_minLine - 1;
	if (nPreviousIdx < 0 || nPreviousIdx >= WINDOW_SIZE) {
		return UNKNOWN;
	}
	LINE* lpPrev = lp->prev;
	TOKEN_LINE_CACHE* tlp = &pHighlighter->h_lines[nPreviousIdx];
	LEXICAL_STATE tType = tlp->s_lineEndType;
	if (tlp->s_linePointer != lpPrev) {
		tType = UNKNOWN;
	}
	if (tType == UNKNOWN) {
		if (lpPrev == NULL) {
			if (nPreviousIdx == 0) {
				lpPrev = fp->firstl;
			} else {
				return UNKNOWN;
			}
		}
		highlight_calculateMissingLineEndStates(pHighlighter, nPreviousIdx, fp, lpPrev);
		return tlp->s_lineEndType;
	}
	return tType;
}

/*
 * A highlighter which passes on tokenization to a generic grammar driven tokenizer.
 */
static unsigned char* highlight_usingGrammar(HIGHLIGHTER* pHighlighter, WINFO* wp, LINE* lp, long nLine) {
	FTABLE* fp = wp->fp;
	highlight_adjustCachedLineWindow(pHighlighter, fp, nLine);
	LEXICAL_STATE lexicalState = highlight_getPreviousLineTokenType(pHighlighter, fp, lp, nLine);
	if (nLine != pHighlighter->h_lastLine || lp != pHighlighter->h_lastLinePointer) {
		int detectedEnd = 0;
		if (pHighlighter->h_styles) {
			if (lp->len >= pHighlighter->h_styleCapacity - 10) {
				pHighlighter->h_styleCapacity = lp->len+30;
				pHighlighter->h_styles = realloc(pHighlighter->h_styles, pHighlighter->h_styleCapacity);
			}
		} else {
			pHighlighter->h_styleCapacity = lp->len+30;
			pHighlighter->h_styles = malloc(pHighlighter->h_styleCapacity);
		}
		unsigned char* pStyles = pHighlighter->h_styles;
		pHighlighter->h_lastLinePointer = lp;
		pHighlighter->h_lastLine = nLine;
		LEXICAL_ELEMENT lexicalElements[MAX_LEXICAL_ELEMENT];
		int nElements = grammar_parse(pHighlighter->h_grammar, lexicalElements, lexicalState, lp->lbuf, lp->len, &detectedEnd);
		int nPreviousOffset = 0;
		int bShowControl = wp->dispmode & SHOWCONTROL;
		if (nElements == 0) {
			for (int i = 0; i < lp->len; i++) {
				unsigned char c = lp->lbuf[i];
				pStyles[i] = (bShowControl && c <= ' ') ? FS_CONTROL_CHARS : FS_NORMAL;
			}
		} else {
			for (int i = 0; i < nElements; i++) {
				int nNextOffset = nPreviousOffset + lexicalElements[i].le_length;
				lexicalState = lexicalElements[i].le_state;
				for (int j = nPreviousOffset; j < nNextOffset && j < lp->len; j++) {
					unsigned char c = lp->lbuf[j];
					pStyles[j] = (bShowControl && c <= ' ') ? FS_CONTROL_CHARS : pHighlighter->h_tokenTypeToStyleTable[lexicalState];
				}
				nPreviousOffset = nNextOffset;
			}
		}
		int nOffset = nLine - pHighlighter->h_minLine;
		if (nOffset >= 0 && nOffset < WINDOW_SIZE) {
			TOKEN_LINE_CACHE* tlp = &pHighlighter->h_lines[nOffset];
			tlp->s_lineEndType = lexicalState;
			tlp->s_linePointer = lp;
			tlp->s_lineNumber = nLine;
		}
	}
	return pHighlighter->h_styles;
}

#ifdef _DEBUG
static void test_highlight(HIGHLIGHTER* pHighlighter, WINFO* wp) {
	FTABLE* fp = wp->fp;
	LINE* lpFirst = fp->firstl;
	for (long ln = 0; lpFirst && lpFirst != fp->lastl; lpFirst = lpFirst->next, ln++) {
		highlight_usingGrammar(pHighlighter, wp, lpFirst, ln);
	}
}
#endif

/*
 * Invalidates the highlighter data for the given line.
 * Note, that lp might be null, in which case all highlight data is discarded.
 */
void highlight_modelChange(HIGHLIGHTER* pHighlighter, MODEL_CHANGE* mp) {
	if (pHighlighter->h_grammar == NULL) {
		pHighlighter->h_lastLine = -1;
		return;
	}
	switch (mp->type) {
	case EVERYTHING_CHANGED:
		highlight_destroyCaches(pHighlighter);
		break;
	case LINE_MODIFIED:
	case LINE_REPLACED:
		//test_highlight(pHighlighter, WIPOI(mp->fp));
		pHighlighter->h_lastLine = -1;
		if (mp->lp == pHighlighter->h_lastLinePointer) {
			pHighlighter->h_lastLinePointer = 0;
		}
		break;
	}
}


/*
 * Calculates the actual styles per character contained in the line passed as an argument.
 */
unsigned char* highlight_calculate(HIGHLIGHTER* pData, WINFO* wp, LINE* lp, long nLine) {
	return pData->h_calculate(pData, wp, lp, nLine);
}

/*
 * Return a syntax highlighter for a given grammar;
 */
HIGHLIGHTER* highlight_getHighlighter(GRAMMAR* pGrammar) {
	HIGHLIGHTER* pHighlighter = calloc(1, sizeof(HIGHLIGHTER));
	pHighlighter->h_grammar = pGrammar;
	pHighlighter->h_calculate = highlight_usingGrammar;
	grammar_initTokenTypeToStyleTable(pGrammar, pHighlighter->h_tokenTypeToStyleTable);
	pHighlighter->h_lastLine = -1;
	return pHighlighter;
}

