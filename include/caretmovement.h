/*
 * CaretMovement.h
 *
 * Project: PKS Edit for Windows
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

typedef struct tagCARET_MOVEMENT_SPEC CARET_MOVEMENT_SPEC;

 /* Describes options for moving the caret */
typedef enum {
    // Move a single character left right or a single line up down
    MOT_SINGLE = 1,
    // Move the caret to the end of the line / file
    MOT_TOEND = 2,
    // Move the caret by a number of words
    MOT_WORD = 3,
    // Move the caret up down page wise
    MOT_PAGE = 3,
    // Move the caret up down to the end of the screen
    MOT_SCREEN = 4,
    // Move the caret left right to a given character
    MOT_UNTILC = 4,
    // Move the caret up down by a paragraph (text)
    MOT_PGRPH = 4,
    // move to begin / end of file
    MOT_FILE = 4,
    // move to next space
    MOT_SPACE = 5,
    // move to center of page
    MOT_CENTER = 5,
    // If added to another option - extend the current selection
    MOT_EXTEND_SELECTION = 0x100
} CARET_MOVEMENT_OPTTION;

/*
 * Enum type to define a direction of a movement etc...
 */
typedef enum {
    DIR_NONE = 0,
    DIR_FORWARD = 1,
    DIR_BACKWARD = -1,
    DIR_LEFT = -1,
    DIR_RIGHT = 1
} DIRECTION_OPTION;

/*--------------------------------------------------------------------------
 * caret_moveLeftRight()
 * Move the caret to the left or right. If motionFlags contains MOT_EXTEND_SELECTION
 * the selection is extended.
 */
extern int caret_moveLeftRight(WINFO* wp, DIRECTION_OPTION direction, CARET_MOVEMENT_OPTTION motionFlags);

/*--------------------------------------------------------------------------
 * EdCursorLeft()
 */
extern long long EdCursorLeft(WINFO* wp, CARET_MOVEMENT_OPTTION mtype);

/*--------------------------------------------------------------------------
 * EdCursorRight()
 */
extern long long EdCursorRight(WINFO* wp, CARET_MOVEMENT_OPTTION mtype);

/*--------------------------------------------------------------------------
 * EdCursorUp()
 */
extern long long EdCursorUp(WINFO* wp, CARET_MOVEMENT_OPTTION mtype);

/*--------------------------------------------------------------------------
 * EdCursorDown()
 */
extern long long EdCursorDown(WINFO* wp, CARET_MOVEMENT_OPTTION mtype);

/*--------------------------------------------------------------------------
 * EdChapterGotoBegin()
 */
extern long long EdChapterGotoBegin(WINFO* wp, DIRECTION_OPTION dir);

/*--------------------------------------------------------------------------
 * EdChapterGotoEnd()
 */
extern long long EdChapterGotoEnd(WINFO* wp, DIRECTION_OPTION dir);

/*
 * Move the caret to add one or more secondary carets.
 */
typedef enum { MSM_UP, MSM_DOWN, MSM_WORDS } MOT_SECONDARY_MOVEMENT;
extern long long caret_moveAndAddSecondary(MOT_SECONDARY_MOVEMENT mType);

/**
 * Utility function to place the caret according to screen coordinates.
 */
extern void caret_placeToXY(WINFO* wp, int x, int y);

/**
 * Add a secondary window caret with the mouse.
 */
extern long long caret_addSecondaryWithMouse();

/*--------------------------------------------------------------------------
 * caret_getPreviousColumnInLine()
 * Got back one screen column in a line and return the new column considering internal
 * structure of the data and return the new offset into the line buffer.
 */
extern int caret_getPreviousColumnInLine(WINFO* wp, LINE* lp, int col);

/*
 * Adds a secondary caret to a window.
 */
extern CARET* caret_addSecondary(WINFO* wp, long ln, long nLineOffset);

/*
 * Removes all secondary carets from the screen.
 */
extern long long caret_removeSecondaryCarets();

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
extern long caret_advanceParagraph(WINFO* wp, long ln, DIRECTION_OPTION dir, int start);

/*--------------------------------------------------------------------------
 * caret_advanceParagraphFromCurrentLine()
 */
extern int caret_advanceParagraphFromCurrentLine(WINFO* wp, DIRECTION_OPTION dir, int start);

/*
 * Calculate an offset of character buffer positions to the corresponding number
 * of columns as displayed on the screen.
 */
extern size_t caret_bufferOffset2Screen(WINFO* wp, const char* lbuf, int lnoffset);

/*
 * Calculate the buffer offset in a line given the corresponding screeen column.
 */
extern size_t caret_screenOffset2Buffer(WINFO* wp, const char* p, size_t lineLen, size_t col);

/*--------------------------------------------------------------------------
  * caret_lineOffset2screen()
  * the following stuff is calculating the cursor screen position, out of
  * the internal offset to the line buffer
  */
extern int caret_lineOffset2screen(WINFO* wp, CARET* cp);

/*--------------------------------------------------------------------------
 * caret_screen2lineOffset()
 * Calculate the internal offset in the line buffer
 * for a given cursor screen position. You must specify the screen column in the
 * field offset of the caret + you must specify either the line pointer to the line,
 * where the offset should be calculated or the line number.
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
extern int caret_placeCursorForFile(WINFO* wp, long ln, long col, long screencol, CARET_MOVEMENT_SPEC* pSpec);

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
 * caret_updateDueToMouseClick()
 * place carets when the mouse is clicked or the slider was moved.
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
extern int caret_placeCursorAndValidate(WINFO* wp, long* ln, long offset, long* screenCol, int updateVirtualOffset, 
    CARET_MOVEMENT_SPEC* xDelta);

/*--------------------------------------------------------------------------
 * caret_advancePage()
 * cursor advance one screen
 */
extern int caret_advancePage(WINFO* wp, long* ln, int dir);

/*--------------------------------------------------------------------------
 * caret_moveUpOrDown()
 * general cursor advancing in
 * vertical direction
 */
extern long long caret_moveUpOrDown(WINFO* wp, DIRECTION_OPTION dir, CARET_MOVEMENT_OPTTION mtype);

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
extern int caret_positionCloseToMouseWithConfirmation(WINFO* wp, long bAsk);

extern LINE* caret_gotoIdentifierEnd(LINE* lp, long* ln, long* col, DIRECTION_OPTION dir, unsigned char unused);
extern LINE* caret_gotoIdentifierSkipSpace(LINE* lp, long* ln, long* col, DIRECTION_OPTION dir, unsigned char unused);
extern LINE* caret_advanceWordOnly(LINE* lp, long* ln, long* col, DIRECTION_OPTION dir, unsigned char unused);

#define CARETMOVEMENT_H
#endif