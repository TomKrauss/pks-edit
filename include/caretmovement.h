/*
 * CaretMovement.h
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: caret positioning
 *
 * 										created: 15.03.91
 * 										last modified:
 *										author: Tom
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
 * caret_advanceParagraph()
 * Advance the cursor starting from a line by one paragraph.
 */
extern long caret_advanceParagraph(long ln, int dir, int start);

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
 * caret_placeCursorInCurrentFile()
 * cursor absolut positioning
 */
extern int caret_placeCursorInCurrentFile(long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorAndSavePosition()
 * Places the cursor in the current document in a given row and column and save the current
 * position.
 */
extern int caret_placeCursorAndSavePosition(long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorForFile()
 * cursor absolut positioning for the given file.
 */
extern int caret_placeCursorForFile(FTABLE* fp, long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorAndMakevisibleWithSpace()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible. Leave some rows space at top / bottom of the screen.
 */
extern int caret_placeCursorAndMakevisibleWithSpace(FTABLE* fp, long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorMakeVisibleAndSaveLocation()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible - save the last cursor position, if we needed to scroll.
 */
extern int caret_placeCursorMakeVisibleAndSaveLocation(long ln, long col);

/*---------------------------------*
 * caret_calculateOffsetFromScreen()
 * Calculate the offset in the file (line and column) given the X and Y coordinates
 * on which a mouse was clicked. No validation regarding the validity of the column
 * is performed at that point in time.
 */
extern void caret_calculateOffsetFromScreen(WINFO* wp, int x, int y, long* line, long* column);

/*--------------------------------------------------------------------------
 * caret_placeCursorMakeVisible()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible.
 */
extern int caret_placeCursorMakeVisible(long ln, long col);

/*--------------------------------------------------------------------------
 * cphypos()
 * cursoraddressierung with slider and mouse
 */
extern int caret_updateLineColumn(FTABLE* fp, long* ln, long* col, int newcol);

/*--------------------------------------------------------------------------
 * caret_updateLineColumn()
 * Invoked, when the cursor is positioned using slider or mouse to update the
 * caret position.
 */
extern int caret_updateLineColumn(FTABLE* fp, long* ln, long* col, int newcol);

/*--------------------------------------------------------------------------
 * caret_placeCursorAndMakevisibleWithSpace()
 */
extern int caret_placeCursorAndMakevisibleWithSpace(FTABLE* fp, long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorAndValidate()
 */
extern int caret_placeCursorAndValidate(FTABLE* fp, long* ln, long* col, int newcol);

/*--------------------------------------------------------------------------
 * caret_advancePage()
 * cursor advance one screen
 */
extern int caret_advancePage(long* ln, int dir);

/*--------------------------------------------------------------------------
 * caret_advanceSection()
 * Advances the cursor by one "section".
 */
extern int caret_advanceSection(int dir, int start);

/*--------------------------------------------------------------------------
 * caret_moveUpOrDown()
 * general cursor advancing in
 * vertical direction
 */
extern int caret_moveUpOrDown(int dir, int mtype);

#define CARETMOVEMENT_H
#endif