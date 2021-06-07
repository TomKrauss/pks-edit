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
 * (c) Pahlen & Krauﬂ
 */

#ifndef MARKPOSITIONS_H

/* should ideally extend CARET */
typedef struct tagMARK MARK;

/*--------------------------------------------------------------------------
 * mark_find()
 * Find a mark given the identifier character.
 */
extern MARK* mark_find(FTABLE* fp, int c);

/*--------------------------------------------------------------------------
 * mark_killSelection()
 * release the copy and paste - marks for a file
 */
extern void mark_killSelection(FTABLE* fp);

/*--------------------------------------------------------------------------
 * mark_set()
 * Sets a mark in the line and offset named "c",
 */
extern MARK* mark_set(FTABLE* fp, LINE* lp, int offs, int c);

/*--------------------------------------------------------------------------
 * mark_goto()
 */
extern LINE* mark_goto(FTABLE* fp, int c, long* ln, long* col);

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
