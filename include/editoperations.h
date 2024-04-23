/*
 * EditOperations.c
 *
 * Project: PKS Edit for MS-WINDOWS
 *
 * purpose: inserting and deleting characters, words, lines ...
 * 		  autoformat, wrap and indent
 *
 * 										created: 04.02.90
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef EDIT_OPERATIONS_H

 /*
  * Comment in/out the selected lines in the current editor.
  */
extern long long edit_toggleComment(WINFO* wp);

/*
 * Insert a string into the current file at the current cursor position.
 */
extern long long edit_insertString(WINFO* wp, const char* pszString);

/*--------------------------------------------------------------------------
 * Perform the given operation on the lines of the specified edior. Some of
 * the given operations require the line being marked before with "mark line".
 */
 /* Flags for marked line delta */
typedef enum { MLN_DELETE = 0, MLN_JOIN = 1, MLN_TOGGLE = 2, MLN_FINDSOFT = 3, MLN_MAKESOFT = 4, MLN_MAKEHARD = 5, MLN_CUT = 6 } MARKED_LINE_OPERATION;
extern long long edit_performLineFlagOperation(WINFO* wp, MARKED_LINE_OPERATION op);

/*
 * Replace a range of lines text with a list of other lines. The algorithm tries to modify only lines
 * really affected and will insert new lines / delete lines if the size of the affected lines and the
 * lines to insert differs.
 */
#ifdef ARRAYLIST_H
long long edit_replaceSelectedRange(WINFO* wp, long nLineFrom, long nLineTo, ARRAY_LIST* pLines);
#endif

/*
 * Types of operations to convert the spellling of characters
 * in a range of text.
 */
typedef enum { 
	// Toggle upper/lower case spelling of all selected characters 
	CC_TOGGLE,
	// Make all selected characters upper case
	CC_UPPER, 
	// Make all selected characters lower case
	CC_LOWER }
CC_OPERATION;

 /*--------------------------------------------------------------------------
  * edit_convertCharacterCase()
  * Map lower characters to upper, upper to lowers and toggle case depending on
  * the passed operation flag.
  */
extern long long edit_convertCharacterCase(WINFO* wp, CC_OPERATION operation);

/*--------------------------------------------------------------------------
 * edit_insertLine()
 * create newline and insert it
 */
/*
 * Options for inserting and deleting lines 
 */
typedef enum {
	// Caret does not change during insert
	ELO_CARET_STAYS = 0, 
	// Caret follows the inserted line
	ELO_CARET_FOLLOWS = 1,
	// Line is appended
	ELO_APPEND = 0x200
} EDIT_LINE_OPTIONS;
extern long long edit_insertLine(WINFO* wp, EDIT_LINE_OPTIONS control);

#define EDIT_OPERATIONS_H
#endif
