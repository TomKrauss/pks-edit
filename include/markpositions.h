/*
 * markpositions.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: setting marks 'a-....
 *
 * 										created: 05.02.89
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MARKPOSITIONS_H

/* should ideally extend CARET */
typedef struct tagMARK MARK;

/*--------------------------------------------------------------------------
 * mark_find()
 * Find a mark given the identifier character.
 */
extern MARK* mark_find(WINFO* wp, int c);

/*--------------------------------------------------------------------------
 * mark_killSelection()
 * release the copy and paste - marks for a file
 */
extern void mark_killSelection(WINFO* wp);

/*
 * Sets a block mark - if the "has current selection" property has changed, fire an action change.
 */
extern void bl_setBlockMark(WINFO* wp, MARK* pMark, BOOL bStart);

/*--------------------------------------------------------------------------
 * mark_set()
 * Sets a mark in the line and offset named "c",
 */
extern MARK* mark_set(WINFO* wp, LINE* lp, int offs, int c);

/*--------------------------------------------------------------------------
 * mark_goto()
 */
extern LINE* mark_goto(WINFO* fp, int c, long* ln, long* col);

/*--------------------------------------------------------------------------
 * EdGotoLastPos()
 */
extern int EdGotoLastPos(int type);

/*--------------------------------------------------------------------------
 * fm_savepos()
 * Save a filemark position, so one can later navigate back to the position.
 */
extern int fm_savepos(char* tag);

/*--------------------------------------------------------------------------
 * fm_mktagstring()
 * Create a String for a file mark.
 */
extern void fm_mktagstring(char* buf, char* bufend);

#define MARKPOSITIONS_H

#endif // !MARKPOSITIONS_H
