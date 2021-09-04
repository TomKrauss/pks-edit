/*
 * EditOperations.c
 *
 * PROJEKT: PKS-EDIT for MS-WINDOWS 3.01
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
extern int edit_toggleComment();

/*--------------------------------------------------------------------------
 * Perform the given operation on the lines of the current file. Some of
 * the given operations required the line being marked before with "mark line".
 */
 /* Flags for marked line delta */
typedef enum { MLN_DELETE = 0, MLN_JOIN = 1, MLN_TOGGLE = 2, MLN_FINDSOFT = 3, MLN_MAKESOFT = 4, MLN_MAKEHARD = 5, MLN_CUT = 6 } MARKED_LINE_OPERATION;
extern int edit_performLineFlagOperation(MARKED_LINE_OPERATION op);

 /*--------------------------------------------------------------------------
  * edit_convertCharacterCase()
  * Map lower characters to upper, upper to lowers and toggle case depending on
  * the passed operation flag.
  */
typedef enum { CC_TOGGLE, CC_UPPER, CC_LOWER} CC_OPERATION;
extern int edit_convertCharacterCase(CC_OPERATION operation);


#define EDIT_OPERATIONS_H
#endif
