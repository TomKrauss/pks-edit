/*
 * MarkPositions.c
 *
 * Project: PKS Edit for Windows
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

#include "alloc.h"
#include <windows.h>
#include "linkedlist.h"
#include "caretmovement.h"
#include "edierror.h"
#include "documentmodel.h"
#include "markpositions.h"
#include "winfo.h"
#include "actions.h"

/*--------------------------------------------------------------------------
 * mark_find()
 * Find a mark given the identifier character.
 */
MARK *mark_find(WINFO *wp, int c)
{	register MARK *mp = wp->fmark;

	while(mp) {
		if (mp->m_identifier == c) break;
		mp = mp->m_next;
	}
	return mp;
}

/**
 * Destroy a mark and related resources.
 */
int mark_destroy(MARK* mp) {
	if (mp) {
		free(mp->m_fname);
		free(mp);
	}
	// Return 0 so we can also be used as callback to ll_destroy - we destroy
	// the mark itself already.
	return 0;
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

	for (mp1 = 0, mprun = wp->fmark, unmark = 1, lp = mp->m_linePointer;
		mprun != 0; mp1 = mprun, mprun = mprun->m_next) {

		/* delete from list */
		if (P_EQ(mprun,mp)) {
			if (mp1)
				mp1->m_next = mp->m_next;
			else
				wp->fmark = mp->m_next;
		} else {
			/* is there another mark for same line ? */
			if (P_EQ(mprun->m_linePointer,lp))
				unmark = 0;
		}
	}
	free(mp->m_fname);
	free(mp);
}

/*
 * Sets a block mark - if the "has current selection" property has changed, fire an action change.
 */
void bl_setBlockMark(WINFO* wp, MARK* pMark, BOOL bStart) {
	BOOL bHasSelection = wp->blstart && wp->blend;
	if (bStart) {
		wp->blstart = pMark;
	} else {
		wp->blend = pMark;
	}
	if (bHasSelection != (wp->blstart && wp->blend)) {
		action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ENABLEMENT);
	}
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
	return ll_insert(&wp->fmark,sizeof(MARK));
}

/*--------------------------------------------------------------------------
 * mark_set()
 * Sets a mark in the line and offset named "c",
 */
MARK *mark_set(WINFO *wp, LINE *lp,int offs,int c)
{	MARK *mp;

	if ((mp = mark_alloc(wp,c)) != (MARK *) 0) {
		mp->m_linePointer = lp;
		mp->m_column = offs;
		mp->m_identifier = c;
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
	*ln  = ln_cnt(fp->firstl,mp->m_linePointer)-1;
	*col = (long)mp->m_column;
	return mp->m_linePointer;
}

