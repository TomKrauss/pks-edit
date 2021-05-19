/*
 * CaretMovement.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: caret positioning
 *
 * 										created      : 15.03.91
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauﬂ
 */

#ifndef CARETMOVEMENT_H

#include "lineoperations.h"

typedef struct wininfo WINFO;

 /*--------------------------------------------------------------------------
  * caret_lineOffset2screen()
  * the following stuff is calculating the cursor screen position, out of
  * the internal offset to the line buffer
  */
extern int caret_lineOffset2screen(FTABLE* fp, CARET* cp);

/*--------------------------------------------------------------------------
 * caret_screen2lineOffset()
 * Calculate the internal offset in the line buffer
 * for a given cursor screen position
 */
extern int caret_screen2lineOffset(FTABLE* fp, CARET* pCaret);

/*--------------------------------------------------------------------------
 * caret_moveToXY()
 * move the caret to follow the mouse pressed on the screen coordinates x and y.
 */
extern int caret_moveToXY(WINFO* wp, int x, int y);

/*--------------------------------------------------------------------------
 * curpos()
 * cursor absolut positioning
 */
extern int curpos(long ln, long col);

/*--------------------------------------------------------------------------
 * centernewpos()
 */
extern int centernewpos(long ln, long col);

/*--------------------------------------------------------------------------
 * center2cpos()
 */
extern int center2cpos(FTABLE* fp, long ln, long col);

/*--------------------------------------------------------------------------
 * cadv_page()
 * cursor advance one screen
 */
extern int cadv_page(long* ln, int dir);

/*--------------------------------------------------------------------------
 * ln_leadspce(l)
 */
extern int ln_leadspce(LINE* lp);

/*--------------------------------------------------------------------------
 * isempty()
 */
extern int isempty(LINE* lp);

#define CARETMOVEMENT_H
#endif