/*
 * publicapi.c
 *
 * PROJEKT: PKS-EDIT for Windows
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

#define	QUERY_CLIPBOARDAVAIL		0
#define	QUERY_HAS_SELECTION_START			1
#define	QUERY_HAS_SELECTION_END			2
#define	QUERY_HAS_SELECTION			3
#define	QUERY_CURRENTFILE			4
#define	QUERY_OPTIONS				5
#define	QUERY_LAYOUTOPTIONS			6
#define	QUERY_WORKMODE				7
#define	QUERY_DISPLAYMODE			8
#define QUERY_FILEMODIFIED			9
#define QUERY_BLOCKXTNDMODE			10

 /*--------------------------------------------------------------------------
  * EdCursorLeft()
  */
extern long long EdCursorLeft(int mtype);

/*--------------------------------------------------------------------------
 * EdCursorRight()
 */
extern long long EdCursorRight(int mtype);

/*--------------------------------------------------------------------------
 * EdCursorUp()
 */
extern long long EdCursorUp(int mtype);

/*--------------------------------------------------------------------------
 * EdCursorDown()
 */
extern long long EdCursorDown(int mtype);

/*--------------------------------------------------------------------------
 * EdChapterGotoBegin()
 */
extern long long EdChapterGotoBegin(int dir);

/*--------------------------------------------------------------------------
 * EdChapterGotoEnd()
 */
extern long long EdChapterGotoEnd(int dir);

#endif