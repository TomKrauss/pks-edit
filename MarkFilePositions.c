/*
 * MarkFilePositions.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: maintain a list of the positions + corresponding active files.
 *
 * 										created: Jan. 1990
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "caretmovement.h"
#include "linkedlist.h"
#include "winfo.h"
#include "pksedit.h"
#include "edierror.h"
#include "stringutil.h"
#include "markpositions.h"
#include "crossreferencelinks.h"

#define	NFILEMARKS		16

typedef struct tagMARK_LIST {
	int	  ml_lastDir;
	MARK* ml_head;
	MARK* ml_current;
} MARK_LIST;

static MARK_LIST _searchMarks;
static MARK_LIST _insertMarks;

/*
 * Destroy all file marks.
 */
void fm_destroyAll() {
	ll_destroy((LINKED_LIST**)  & _searchMarks.ml_head, mark_destroy);
	ll_destroy((LINKED_LIST**)&_insertMarks.ml_head, mark_destroy);
}

/*--------------------------------------------------------------------------
 * fm_getSavedMark()
 * Return a mark to a previously saved position. If offset is positive a mark previous to the "current file mark" 
 * is returned (goto previous position), if the offset is negative, a "forward" mark is returned (reverts a goto previous
 * position).
 */
static MARK * fm_getSavedMark(MARK_TYPE_ENUM type, int offset) {
	MARK* mp;
	MARK_LIST* mlp = type == TM_LASTSEARCH ? &_searchMarks : &_insertMarks;

	if (mlp->ml_lastDir != offset) {
		mlp->ml_lastDir = offset;
		offset *= 2;
	}
	if (offset < 0) {
		mp = mlp->ml_head;
		while (mp) {
			if (mp->m_next == mlp->ml_current || mlp->ml_current == NULL) {
				mlp->ml_current = mp;
				if (offset == -2 && mp->m_next) {
					mlp->ml_current = mp->m_next;
				}
				break;
			}
			mp = mp->m_next;
		}
		return mlp->ml_current;
	} else {
		mp = mlp->ml_current;
		while (--offset > 0 && mp) {
			mp = mp->m_next;
		}
		if (mp) {
			mlp->ml_current = mp->m_next;
		}

	}
	return mp;
}

/*--------------------------------------------------------------------------
 * fm_savepos()
 * Save a filemark position, so one can later navigate back to the position.
 * Return 1 if successful.
 */
int fm_savepos(MARK_TYPE_ENUM type) {	
	WINFO* wp = ww_getCurrentEditorWindow();

	if (wp == NULL) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	MARK_LIST* mlp = type == TM_LASTSEARCH ? &_searchMarks : &_insertMarks;
	if (mlp->ml_current) {
		if (strcmp(mlp->ml_current->m_fname, fp->fname) == 0 && mlp->ml_current->m_linePointer == wp->caret.linePointer) {
			// nothing to do.
			return 1;
		}
	}
	MARK* pEndOfHistory = (MARK*) ll_at((LINKED_LIST*)mlp->ml_head, NFILEMARKS);
	if (pEndOfHistory) {
		MARK* pDestroy = pEndOfHistory->m_next;
		pEndOfHistory->m_next = NULL;
		while (pDestroy) {
			if (pDestroy == mlp->ml_current) {
				mlp->ml_current = NULL;
			}
			MARK* mpNext = pDestroy->m_next;
			mark_destroy(pDestroy);
			pDestroy = mpNext;
		}
	}
	MARK* pMark = calloc(1, sizeof * pMark);
	if (!pMark) {
		return 0;
	}
	pMark->m_column = wp->caret.offset;
	pMark->m_fname = _strdup(fp->fname);
	pMark->m_line = wp->caret.ln;
	pMark->m_linePointer = wp->caret.linePointer;
	pMark->m_next = mlp->ml_head;
	mlp->ml_head = pMark;
	mlp->ml_current = pMark;
	return 1;
}

/*--------------------------------------------------------------------------
 * fm_gotoLastPosition()
 */
static int fm_gotoLastPositionDir(MARK_TYPE_ENUM type, int dir) {
	MARK*	mp;

	mp = fm_getSavedMark(type, dir);
	if (mp == 0) {
		error_showErrorById(IDS_MSGNOLASTPOS);
		return 0;
	}
	FTABLE* fp = ft_fpbyname(mp->m_fname);
	WINFO* wp;
	if (fp == NULL) {
		if (!ft_openFileWithoutFileselector(mp->m_fname, -1, &(FT_OPEN_OPTIONS) { NULL, CP_ACP })) {
			return 0;
		}
		wp = ww_getCurrentEditorWindow();
	} else {
		wp = WIPOI(fp);
		if (wp) {
			ft_selectWindowWithId(wp->win_id, 1);
		}
	}
	if (wp) {
		bl_hideSelection(wp, TRUE);
		caret_placeCursorInCurrentFile(wp, mp->m_line, mp->m_column);
		return  1;
	}

	return  0;

}

/*--------------------------------------------------------------------------
 * fm_gotoLastPosition()
 * Goto to the last navigation/insertion position depending on the mark type.
 */
int fm_gotoLastPosition(MARK_TYPE_ENUM type) {
	return fm_gotoLastPositionDir(type, 1);
}

/*--------------------------------------------------------------------------
 * fm_gotoMextPosition()
 * Goto to the next navigation/insertion position depending on the mark type.
 */
int fm_gotoNextPosition(MARK_TYPE_ENUM type) {
	return fm_gotoLastPositionDir(type, -1);
}

