/*
 * syntaxhighlighting.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * purpose: as the name suggests....
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef SYNTAXHIGHLIGHTING_H
#include <lineoperations.h>

typedef struct tagHIGHLIGHTER HIGHLIGHTER;

/*
 * Return a syntax highlighter for a given grammar;
 */
extern HIGHLIGHTER* highlight_getHighlighter(GRAMMAR* pszGrammar);

/*
 * Invalidates the highlighter data for the given line.
 */
extern void highlight_invalidate(HIGHLIGHTER* pData, LINE* lp);

/*
 * Destroy a syntax highlighter, when it not used any more.
 */
extern void highlight_destroy(HIGHLIGHTER *pHighlighter);

/*
 * Calculates the actual styles per character contained in the line passed as an argument.
 */
extern unsigned char* highlight_calculate(HIGHLIGHTER* pData, FTABLE* fp, LINE* lp, long nLine);

#define SYNTAXHIGHLIGHTING_H
#endif // !SYNTAXHIGHLIGHTING_H

