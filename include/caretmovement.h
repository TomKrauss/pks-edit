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
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef CARETMOVEMENT_H

#include "documentmodel.h"

/*--------------------------------------------------------------------------
 * caret_moveLeftRight()
 * Move the caret to the left or right. If motionFlags contains MOT_XTNDBLOCK
 * the selection is extended.
 */
extern int caret_moveLeftRight(WINFO* wp, int direction, int motionFlags);

/**
 * Utility function to place the caret according to screen coordinates.
 */
extern void caret_placeToXY(WINFO* wp, int x, int y);

/*--------------------------------------------------------------------------
 * caret_getPreviousColumnInLine()
 * Got back one screen column in a line and return the new column considering internal
 * structure of the data and return the new offset into the line buffer.
 */
extern int caret_getPreviousColumnInLine(WINFO* wp, LINE* lp, int col);

/*
 * Update the current selection in a window. It is assumed that the caret has moved with an "extend selection"
 * move (e.g. by mouse drag or by Shift+Arrow Key)
 */
extern void caret_updateSelection(WINFO* wp, CARET* c1, CARET* c2, BOOL bC1BeforeC2);

/**
 * caret_isBeforeOther()
 * Check, whether one caret is located before another one. If bMarkEnd is true, the logic for determining the end is reversed.
 */
extern int caret_isBeforeOther(LINE* lpFirst, int col1, const LINE* fpcl, int col, int bMarkEnd);

/*--------------------------------------------------------------------------
 * caret_advanceParagraph()
 * Advance the cursor starting from a line by one paragraph.
 */
extern long caret_advanceParagraph(WINFO* wp, long ln, int dir, int start);

/*--------------------------------------------------------------------------
 * caret_advanceParagraphFromCurrentLine()
 */
extern int caret_advanceParagraphFromCurrentLine(WINFO* wp, int dir, int start);

/*
 * Calculate an offset of character buffer positions to the corresponding number
 * of columns as displayed on the screen.
 */
extern int caret_bufferOffset2screen(WINFO* wp, const char* lbuf, int lnoffset);

/*--------------------------------------------------------------------------
  * caret_lineOffset2screen()
  * the following stuff is calculating the cursor screen position, out of
  * the internal offset to the line buffer
  */
extern int caret_lineOffset2screen(WINFO* wp, CARET* cp);

/*--------------------------------------------------------------------------
 * caret_screen2lineOffset()
 * Calculate the internal offset in the line buffer
 * for a given cursor screen position
 */
extern int caret_screen2lineOffset(WINFO* wp, CARET* pCaret);

/*--------------------------------------------------------------------------
 * caret_moveToXY()
 * move the caret to follow the mouse pressed on the screen coordinates x and y.
 */
extern int caret_moveToXY(WINFO* wp, int x, int y);

/*--------------------------------------------------------------------------
 * caret_placeCursorInCurrentFile()
 * cursor absolut positioning
 */
extern int caret_placeCursorInCurrentFile(WINFO* wp, long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorAndSavePosition()
 * Places the cursor in the current document in a given row and column and save the current
 * position.
 */
extern int caret_placeCursorAndSavePosition(WINFO* wp, long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorForFile()
 * cursor absolut positioning for the given file.
 */
extern int caret_placeCursorForFile(WINFO* wp, long ln, long col, long screencol, int xDelta);

/*--------------------------------------------------------------------------
 * caret_placeCursorAndMakevisibleWithSpace()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible. Leave some rows space at top / bottom of the screen.
 */
extern int caret_placeCursorAndMakevisibleWithSpace(WINFO* wp, long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorMakeVisibleAndSaveLocation()
 * Place a cursor to the new line number and column and scroll to make the cursor
 * visible - save the last cursor position, if we needed to scroll.
 */
extern int caret_placeCursorMakeVisibleAndSaveLocation(WINFO* wp, long ln, long col);

/*---------------------------------*
 * caret_calculateOffsetFromScreen()
 * Calculate the offset in the file (line and column) given the X and Y coordinates
 * on which a mouse was clicked. No validation regarding the validity of the column
 * is performed at that point in time.
 */
extern void caret_calculateOffsetFromScreen(WINFO* wp, int x, int y, long* line, long* column);

/*--------------------------------------------------------------------------
 * cphypos()
 * cursoraddressierung with slider and mouse
 */
extern int caret_updateDueToMouseClick(WINFO* wp, long* ln, long* col, int newcol);

/*
 * Move the caret to the given line.
 */
extern void caret_moveToLine(WINFO* wp, long ln);

/*--------------------------------------------------------------------------
 * caret_updateDueToMouseClick()
 * Invoked, when the cursor is positioned using slider or mouse to update the
 * caret position.
 */
extern int caret_updateDueToMouseClick(WINFO* wp, long* ln, long* col, int newcol);

/*--------------------------------------------------------------------------
 * caret_placeCursorAndMakevisibleWithSpace()
 */
extern int caret_placeCursorAndMakevisibleWithSpace(WINFO* wp, long ln, long col);

/*--------------------------------------------------------------------------
 * caret_placeCursorAndValidate()
 */
extern int caret_placeCursorAndValidate(WINFO* wp, long* ln, long offset, long* screenCol, int updateVirtualOffset, int xDelta);

/*--------------------------------------------------------------------------
 * caret_advancePage()
 * cursor advance one screen
 */
extern int caret_advancePage(WINFO* wp, long* ln, int dir);

/*--------------------------------------------------------------------------
 * caret_advanceSection()
 * Advances the cursor by one "section".
 */
extern int caret_advanceSection(WINFO* wp, int dir, int start);

/*--------------------------------------------------------------------------
 * caret_moveUpOrDown()
 * general cursor advancing in
 * vertical direction
 */
extern int caret_moveUpOrDown(WINFO* wp, int dir, int mtype);

/*--------------------------------------------------------------------------
 * caret_moveToCurrentMousePosition()
 */
extern int caret_moveToCurrentMousePosition(WINFO* wp, long bAsk);

/*--------------------------------------------------------------------------
 * caret_saveLastPosition()
 * Store the last cursor position so one can navigate "back" for the current
 * editor window.
 */
extern int caret_saveLastPosition(void);

/*--------------------------------------------------------------------------
 * caret_positionCloseToMouseWithConfirmation()
 * Position the caret in the current window potentially asking the user before doing so.
 */
extern int caret_positionCloseToMouseWithConfirmation(long bAsk);

extern LINE* caret_gotoIdentifierEnd(LINE* lp, long* ln, long* col, int dir);
extern LINE* caret_gotoIdentifierSkipSpace(LINE* lp, long* ln, long* col, int dir);
extern LINE* caret_advanceWordOnly(LINE* lp, long* ln, long* col, int dir);

#define CARETMOVEMENT_H
#endif