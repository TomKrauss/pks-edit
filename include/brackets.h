/*
 * brackets.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: definition of brackets structure
 *
 * 										created: 28.10.91
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

# ifndef 	BRACKETS_H

#define	BRT_MATCH		0
#define	BRT_INDENT	1

#define	UA_MPLEN		8

/*--------------------------------------------------------------------------
 * uc_shiftRange() 
 * shift text range
 */
extern int uc_shiftRange(int scope, int dir);

/*--------------------------------------------------------------------------
 * uc_showMatchingBracket(s)
 * tries to highlight the matching bracket after a character had been inserted.
 */
extern int uc_showMatchingBracket(WINFO* wp);

/*
 * Find an action descriptor to perform given a line buffer and an offset into that buffer.
 */
#ifdef GRAMMAR_H
extern UCLIST* uc_find(GRAMMAR* pGrammar, LINE* lineBuffer, int column);
#endif

#define	BRACKETS_H
# endif

