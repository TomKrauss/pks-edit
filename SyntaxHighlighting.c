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

typedef unsigned char* (*HIGHLIGHT_CALCULATE)(HIGHLIGHTER* pData, FTABLE* fp, LINE* lp, long nLine);

typedef struct tagHIGHLIGHTER {
	HIGHLIGHT_CALCULATE h_calculate;
	long h_lastLine;
	LINE* h_lastLinePointer;
	unsigned char* h_data;
} HIGHLIGHTER;


/*
 * Destroy the syntax highlighter. 
 */
void highlight_destroy(HIGHLIGHTER* pHighlighter) {
	if (pHighlighter->h_data) {
		free(pHighlighter->h_data);
	}
	free(pHighlighter);
}

/*
 * A simple demo highlighter.
 */
static unsigned char* highlight_demo(HIGHLIGHTER* pData, FTABLE* fp, LINE* lp, long nLine) {
	if (nLine != pData->h_lastLine || lp != pData->h_lastLinePointer) {
		pData->h_data = pData->h_data ? realloc(pData->h_data, lp->len) : malloc(lp->len);
		unsigned char* pStyles = pData->h_data;
		int state = FS_NORMAL;
		pData->h_lastLinePointer = lp;
		pData->h_lastLine = nLine;
		for (int i = 0; i < lp->len; i++) {
			unsigned char c = lp->lbuf[i];
			if (c <= ' ') {
				*pStyles++ = FS_CONTROL_CHARS;
				continue;
			} else if (state == FS_NORMAL) {
				if (c == '/' && i + 1 < lp->len && lp->lbuf[i + 1] == '/') {
					i++;
					state = FS_COMMENT;
					*pStyles++ = FS_COMMENT;
					*pStyles++ = FS_COMMENT;
					continue;
				}
				if (c == '"') {
					state = FS_STRING;
				}
			} else if (state == FS_STRING) {
				if (c == '\\' && i+1 < lp->len) {
					i++;
					*pStyles++ = state;
					*pStyles++ = state;
					continue;
				} else if (c == '"') {
					*pStyles++ = state;
					state = FS_NORMAL;
					continue;
				}
			}
			*pStyles++ = state;
		}
	}
	return pData->h_data;
}

/*
 * Invalidates the highlighter data for the given line.
 * Note, that lp might be null, in which case all highlight data is discarded.
 */
void highlight_invalidate(HIGHLIGHTER* pData, LINE* lp) {
	pData->h_lastLine = -1;
	pData->h_lastLinePointer = 0;
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
	highlight_invalidate(pResult, 0);
	return pResult;
}

