/*
 * markpositions.c
 *
 * Project: PKS Edit for Windows
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

/*
 * Destroy all file marks.
 */
void fm_destroyAll();

/*--------------------------------------------------------------------------
 * fm_savepos()
 * Save a filemark position, so one can later navigate back to the position.
 */
extern int fm_savepos(MARK_TYPE_ENUM type);

/**
 * Destroy a mark and related resources.
 */
extern int mark_destroy(MARK* mp);

/*--------------------------------------------------------------------------
 * fm_gotoLastPosition()
 * Goto to the last navigation/insertion position depending on the mark type.
 */
extern long long fm_gotoLastPosition(MARK_TYPE_ENUM type);

/*--------------------------------------------------------------------------
 * fm_gotoMextPosition()
 * Goto to the next navigation/insertion position depending on the mark type.
 */
extern long long fm_gotoNextPosition(MARK_TYPE_ENUM type);

/*--------------------------------------------------------------------------
 * fm_canGotoNext()
 * Is a goto next for the given mark type supported?
 */
extern int fm_canGotoNext(intptr_t* pStack);

/*--------------------------------------------------------------------------
 * fm_canGotoLast()
 * Is a goto last for the given mark type supported?
 */
extern int fm_canGotoLast(intptr_t* pStack);

#define MARKPOSITIONS_H

#endif // !MARKPOSITIONS_H
