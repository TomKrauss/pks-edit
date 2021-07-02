/*
 * MarkPositions.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: setting marks 'a-....
 *
 * 										created: 05.02.89
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdlib.h>
#include <windows.h>
#include "linkedlist.h"
#include "caretmovement.h"
#include "edierror.h"
#include "lineoperations.h"
#include "markpositions.h"
#include "winfo.h"

/*--------------------------------------------------------------------------
 * mark_find()
 * Find a mark given the identifier character.
 */
MARK *mark_find(WINFO *wp, int c)
{	register MARK *mp = wp->fmark;

	while(mp) {
		if (mp->mchar == c) break;
		mp = mp->next;
	}
	return mp;
}

/*--------------------------------------------------------------------------
 * mark_free()
 */
static void mark_free(WINFO *wp, MARK *mp)
{	MARK *mprun,*mp1;
	LINE *lp;
	int  unmark;

	if (!mp)
		return;

	for (mp1 = 0, mprun = wp->fmark, unmark = 1, lp = mp->lm; 
		mprun != 0; mp1 = mprun, mprun = mprun->next) {

		/* delete from list */
		if (P_EQ(mprun,mp)) {
			if (mp1)
				mp1->next = mp->next;
			else
				wp->fmark = mp->next;
		} else {
			/* is there another mark for same line ? */
			if (P_EQ(mprun->lm,lp))
				unmark = 0;
		}
	}

	_free(mp);
}

/*--------------------------------------------------------------------------
 * mark_killSelection()
 * release the copy and paste - marks for a file
 */
void mark_killSelection(WINFO* wp)
{
	mark_free(wp, wp->blstart);
	bl_setBlockMark(wp, NULL, 1);
	mark_free(wp, wp->blend);
	bl_setBlockMark(wp, NULL, 0);
}

/*--------------------------------------------------------------------------
 * mark_alloc()
 */
static MARK *mark_alloc(WINFO *wp, int c)
{
	mark_free(wp,mark_find(wp,c));
	return (MARK*) ll_insert((LINKED_LIST**)&wp->fmark,sizeof(MARK));
}

/*--------------------------------------------------------------------------
 * mark_set()
 * Sets a mark in the line and offset named "c",
 */
MARK *mark_set(WINFO *wp, LINE *lp,int offs,int c)
{	MARK *mp;

	if ((mp = mark_alloc(wp,c)) != (MARK *) 0) {
		mp->lm 	= lp;
		mp->lc 	= offs;
		mp->mchar = c;
		return mp;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * mark_goto()
 */
LINE *mark_goto(WINFO *wp, int c, long *ln, long *col) {	
	MARK *mp;

	if (wp == 0) 
		return 0;
	if ((mp = mark_find(wp,c)) == 0)
		return 0;
	FTABLE* fp = wp->fp;
	*ln  = ln_cnt(fp->firstl,mp->lm)-1;
	*col = (long)mp->lc;
	return mp->lm;
}

/*--------------------------------------------------------------------------
 * EdGotoLastPos()
 */
int EdGotoLastPos(int type)
{	register FTABLE *fp;
	long ln,col;
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp == NULL) {
		return 0;
	}
	fp = wp->fp;
	/*
		* should do different things: last insert, last search,...
		*/
	switch(type) {
		case TM_LASTINSERT:
			if ((ln = ln_indexOf(fp,fp->tln)) < 0)
				return 0;
			col = fp->lastmodoffs;
			break;
		default:
			ln = fp->lastln;
			col = fp->lastcol;
	}
	return caret_placeCursorMakeVisibleAndSaveLocation(wp, ln,col);	   
}
