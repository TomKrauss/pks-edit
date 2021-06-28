/*
 * publicapi.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose:
 * defines all macro callable editor API functions.
 * 
 *
 * 										created: 20.02.87
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef PUBLICAPI_H

#define PUBLICAPI_H

 /*--------------------------------------------------------------------------
  * EdCursorLeft()
  */
extern int EdCursorLeft(int mtype);

/*--------------------------------------------------------------------------
 * EdCursorRight()
 */
extern int EdCursorRight(int mtype);

/*--------------------------------------------------------------------------
 * EdCursorUp()
 */
extern int EdCursorUp(int mtype);

/*--------------------------------------------------------------------------
 * EdCursorDown()
 */
extern int EdCursorDown(int mtype);

/*--------------------------------------------------------------------------
 * EdChapterGotoBegin()
 */
extern int EdChapterGotoBegin(int dir);

/*--------------------------------------------------------------------------
 * EdChapterGotoEnd()
 */
extern int EdChapterGotoEnd(int dir);

#endif