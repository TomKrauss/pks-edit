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
#include <stdlib.h>

#include "syntaxhighlighting.h"
#include "editorfont.h"

#define MAX_TOKEN_TYPE	64
#define WINDOW_SIZE		100

typedef unsigned char* (*HIGHLIGHT_CALCULATE)(HIGHLIGHTER* pData, FTABLE* fp, LINE* lp, long nLine);

typedef enum {
	UNKNOWN,
	OTHER,
	STRING,
	IDENTIFIER,
	OPERATOR,
	KEYWORD,
	SL_COMMENT,
	ML_COMMENT
} TOKEN_TYPE;

typedef struct tagTOKEN_LINE_CACHE {
	long s_lineNumber;
	LINE* s_linePointer;
	TOKEN_TYPE s_lineEndType;
} TOKEN_LINE_CACHE;

typedef struct tagHIGHLIGHTER {
	HIGHLIGHT_CALCULATE h_calculate;
	unsigned char h_tokenTypeToStyleTable[MAX_TOKEN_TYPE];
	unsigned char* h_styles;
	long h_minLine;
	long h_lastLine;
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
	}
	for (int i = 0; i < WINDOW_SIZE; i++) {
		pHighlighter->h_lines[i].s_linePointer = 0;
		pHighlighter->h_lines[i].s_lineEndType = UNKNOWN;
	}
}

/*
 * Destroy the syntax highlighter.
 */
void highlight_destroy(HIGHLIGHTER* pHighlighter) {
	highlight_destroyCaches(pHighlighter);
	free(pHighlighter);
}

static TOKEN_TYPE highlight_getPreviousLineTokenType(HIGHLIGHTER* pHighlighter, FTABLE* fp, LINE* lp, long nLine) {
	if (nLine == 0) {
		return OTHER;
	}
	int nPreviousIdx = nLine - pHighlighter->h_minLine - 1;
	if (nPreviousIdx < 0 || nPreviousIdx >= WINDOW_SIZE) {
		highlight_destroyCaches(pHighlighter);
		for (int i = 0; i < WINDOW_SIZE / 2 && lp && nLine >= 0; i++) {
			nLine--;
			lp = lp->prev;
		}
		pHighlighter->h_minLine = nLine;
		// TODO: determine line-spanning tokens from previous lines, when the cache is invalidated.
		return OTHER;
	}
	TOKEN_TYPE tType = pHighlighter->h_lines[nPreviousIdx].s_lineEndType;
	return tType == UNKNOWN ? OTHER : tType;
}

/*
 * A simple demo highlighter.
 */
static unsigned char* highlight_demo(HIGHLIGHTER* pHighlighter, FTABLE* fp, LINE* lp, long nLine) {
	TOKEN_TYPE tokenType = highlight_getPreviousLineTokenType(pHighlighter, fp, lp, nLine);
	if (tokenType == SL_COMMENT) {
		tokenType = OTHER;
	}
	if (nLine != pHighlighter->h_lastLine || lp != pHighlighter->h_lastLinePointer) {
		pHighlighter->h_styles = pHighlighter->h_styles ? realloc(pHighlighter->h_styles, lp->len) : malloc(lp->len);
		unsigned char* pStyles = pHighlighter->h_styles;
		pHighlighter->h_lastLinePointer = lp;
		pHighlighter->h_lastLine = nLine;
		for (int i = 0; i < lp->len; i++) {
			unsigned char c = lp->lbuf[i];
			if (c <= ' ') {
				*pStyles++ = FS_CONTROL_CHARS;
				continue;
			} else if (tokenType == ML_COMMENT) {
				if (c == '*' && i + 1 < lp->len && lp->lbuf[i + 1] == '/') {
					i++;
					tokenType = OTHER;
					*pStyles++ = FS_COMMENT;
					*pStyles++ = FS_COMMENT;
					continue;
				}
			} else if (tokenType == STRING) {
				if (c == '\\' && i + 1 < lp->len) {
					i++;
					*pStyles++ = FS_STRING;
					*pStyles++ = FS_STRING;
					continue;
				}
				else if (c == '"') {
					*pStyles++ = FS_STRING;
					tokenType = OTHER;
					continue;
				}
			} else if (tokenType != SL_COMMENT) {
				TOKEN_TYPE hOld = tokenType;
				if (c == '/' && i + 1 < lp->len) {
					if (lp->lbuf[i + 1] == '/') {
						tokenType = SL_COMMENT;
					} else if (lp->lbuf[i + 1] == '*') {
						tokenType = ML_COMMENT;
					}
					if (tokenType != hOld) {
						i++;
						*pStyles++ = FS_COMMENT;
						*pStyles++ = FS_COMMENT;
						continue;
					}
				} else if (c == '"') {
					tokenType = STRING;
				} else if (c == '*' || c == '?' || c == '/' || c == '~' || c == '-' || c == '=' || c == '!' || c == '|' || c == '&') {
					tokenType = OPERATOR;
				} else {
					tokenType = OTHER;
				}
			}
			*pStyles++ = pHighlighter->h_tokenTypeToStyleTable[tokenType];
		}
		pHighlighter->h_lines[nLine - pHighlighter->h_minLine].s_lineEndType = tokenType;
	}
	return pHighlighter->h_styles;
}

/*
 * Invalidates the highlighter data for the given line.
 * Note, that lp might be null, in which case all highlight data is discarded.
 */
void highlight_invalidate(HIGHLIGHTER* pHighlighter, LINE* lp) {
	pHighlighter->h_lastLine = -1;
	pHighlighter->h_lastLinePointer = 0;
	for (int i = 0; i < WINDOW_SIZE; i++) {
		if (lp == pHighlighter->h_lines[i].s_linePointer) {
			pHighlighter->h_lines[i].s_lineEndType = UNKNOWN;
			break;
		}
	}
}


/*
 * Calculates the actual styles per character contained in the line passed as an argument.
 */
unsigned char* highlight_calculate(HIGHLIGHTER* pData, FTABLE* fp, LINE* lp, long nLine) {
	return pData->h_calculate(pData, fp, lp, nLine);
}

/*
 * Return a syntax highlighter for a given grammar;
 */
HIGHLIGHTER* highlight_getHighlighter(GRAMMAR* pszGrammar) {
	HIGHLIGHTER* pResult = calloc(1, sizeof(HIGHLIGHTER));
	pResult->h_calculate = highlight_demo;
	pResult->h_tokenTypeToStyleTable[SL_COMMENT] = FS_COMMENT;
	pResult->h_tokenTypeToStyleTable[ML_COMMENT] = FS_COMMENT;
	pResult->h_tokenTypeToStyleTable[OPERATOR] = FS_OPERATOR;
	pResult->h_tokenTypeToStyleTable[STRING] = FS_STRING;
	pResult->h_tokenTypeToStyleTable[IDENTIFIER] = FS_IDENTIFIER;
	pResult->h_tokenTypeToStyleTable[KEYWORD] = FS_KEYWORD;
	pResult->h_tokenTypeToStyleTable[OTHER] = FS_NORMAL;
	pResult->h_tokenTypeToStyleTable[UNKNOWN] = FS_NORMAL;
	highlight_invalidate(pResult, 0);
	return pResult;
}

