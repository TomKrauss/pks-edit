/*
 * MarkPositions.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: setting marks 'a-....
 *
 * 										created: 05.02.89
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#include <stdlib.h>
#include <windows.h>
#include "caretmovement.h"
#include "edierror.h"
#include "lineoperations.h"
#include "markpositions.h"
#include "winfo.h"

/*--------------------------------------------------------------------------
 * mark_find()
 * Find a mark given the identifier character.
 */
MARK *mark_find(FTABLE *fp, int c)
{	register MARK *mp = fp->fmark;

	while(mp) {
		if (mp->mchar == c) break;
		mp = mp->next;
	}
	return mp;
}

/*--------------------------------------------------------------------------
 * mark_free()
 */
static void mark_free(FTABLE *fp, MARK *mp)
{	MARK *mprun,*mp1;
	LINE *lp;
	int  unmark;

	if (!mp)
		return;

	for (mp1 = 0, mprun = fp->fmark, unmark = 1, lp = mp->lm; 
		mprun != 0; mp1 = mprun, mprun = mprun->next) {

		/* delete from list */
		if (P_EQ(mprun,mp)) {
			if (mp1)
				mp1->next = mp->next;
			else
				fp->fmark = mp->next;
		} else {
			/* is there another mark for same line ? */
			if (P_EQ(mprun->lm,lp))
				unmark = 0;
		}
	}
	if (unmark)
		lp->lflg &= ~LNMARKED;

	_free(mp);
}

/*--------------------------------------------------------------------------
 * mark_killSelection()
 * release the copy and paste - marks for a file
 */
void mark_killSelection(FTABLE* fp)
{
	mark_free(fp, fp->blstart);
	bl_setBlockMark(fp, NULL, 1);
	mark_free(fp, fp->blend);
	bl_setBlockMark(fp, NULL, 0);
}

/*--------------------------------------------------------------------------
 * mark_alloc()
 */
static MARK *mark_alloc(FTABLE *fp, int c)
{
	mark_free(fp,mark_find(fp,c));
	return (MARK*) ll_insert((LINKED_LIST**)&fp->fmark,sizeof(MARK));
}

/*--------------------------------------------------------------------------
 * mark_set()
 * Sets a mark in the line and offset named "c",
 */
MARK *mark_set(FTABLE *fp, LINE *lp,int offs,int c)
{	MARK *mp;

	if ((mp = mark_alloc(fp,c)) != (MARK *) 0) {
		mp->lm 	= lp;
		mp->lc 	= offs;
		mp->mchar = c;
		lp->lflg |= LNMARKED;
		return mp;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * mark_goto()
 */
LINE *mark_goto(FTABLE *fp, int c, long *ln, long *col)
{	MARK *mp;

	if (fp == 0) 
		return 0;
	if ((mp = mark_find(fp,c)) == 0)
		return 0;
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
