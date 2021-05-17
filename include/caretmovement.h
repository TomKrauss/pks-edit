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

extern int cphys2scr(char* lbuf, int lnoffs);

extern int cscr2phys(LINE* lp, int col);

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