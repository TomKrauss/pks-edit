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

#define	NFILEMARKS		32

typedef struct tagMARK_LIST {
	MARK* ml_head;
	int ml_currentIdx;
	MARK_TYPE_ENUM ml_lastType;
} MARK_LIST;

static MARK_LIST _autoMarks;

/*
 * Destroy all file marks.
 */
void fm_destroyAll() {
	ll_destroy((LINKED_LIST**)  & _autoMarks.ml_head, mark_destroy);
}

/*--------------------------------------------------------------------------
 * fm_getSavedMark()
 * Return a mark to a previously saved position. If offset is positive a mark previous to the "current file mark" 
 * is returned (goto previous position), if the offset is negative, a "forward" mark is returned (reverts a goto previous
 * position).
 */
static MARK * fm_getSavedMark(MARK_TYPE_ENUM type, int offset, BOOL bUpdateIndex) {
	MARK* mp;
	MARK_LIST* mlp = &_autoMarks;

	int nIdxOfMark;
	if (offset > 0) {
		nIdxOfMark = mlp->ml_currentIdx - 1;
		if (nIdxOfMark < 0) {
			return NULL;
		}
	} else {
		nIdxOfMark = mlp->ml_currentIdx + 1;
	}
	if (type == MTE_AUTO_LAST_INSERT) {
		mp = NULL;
		MARK* pMark = _autoMarks.ml_head;
		int nCurrentIdx = 0;
		while (pMark && nCurrentIdx <= nIdxOfMark) {
			if (pMark->m_identifier == type) {
				mp = pMark;
			}
			pMark = pMark->m_next;
			nCurrentIdx++;
		}
	} else {
		mp = (MARK*)ll_at((LINKED_LIST*)mlp->ml_head, nIdxOfMark);
	}
	if (mp && bUpdateIndex) {
		mlp->ml_currentIdx = nIdxOfMark;
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
	MARK_LIST* mlp = &_autoMarks;
	MARK* mp = (MARK*)ll_at((LINKED_LIST*)mlp->ml_head, mlp->ml_currentIdx);
	if (mp) {
		if (strcmp(mp->m_fname, fp->fname) == 0 && mp->m_linePointer == wp->caret.linePointer) {
			if (type == MTE_AUTO_LAST_INSERT) {
				mp->m_identifier = type;
			}
			// nothing to do.
			return 1;
		}
	}

	if (ll_size((LINKED_LIST*)mlp->ml_head) >= NFILEMARKS) {
		MARK* pDestroy = mlp->ml_head;
		mlp->ml_head = pDestroy->m_next;
		mark_destroy(pDestroy);
		if (mlp->ml_currentIdx > 0) {
			mlp->ml_currentIdx--;
		}
	}
	MARK* pMark = (MARK*)ll_append((LINKED_LIST**) & mlp->ml_head, sizeof * pMark);
	if (!pMark) {
		return 0;
	}
	pMark->m_column = wp->caret.offset;
	pMark->m_fname = _strdup(fp->fname);
	pMark->m_line = wp->caret.ln;
	pMark->m_linePointer = wp->caret.linePointer;
	pMark->m_identifier = type;
	mlp->ml_currentIdx = ll_size((LINKED_LIST*)mlp->ml_head);
	return 1;
}

/*--------------------------------------------------------------------------
 * fm_gotoLastPosition()
 */
static int fm_gotoLastPositionDir(MARK_TYPE_ENUM type, int dir) {
	MARK*	mp;

	if (dir < 0) {
		type = _autoMarks.ml_lastType;
	}
	mp = fm_getSavedMark(type, dir, TRUE);
	if (mp == 0) {
		error_showErrorById(IDS_MSGNOLASTPOS);
		return 0;
	}
	if (dir > 0) {
		_autoMarks.ml_lastType = type;
	}
	if (!mp->m_next && dir > 0) {
		int nIdx = _autoMarks.ml_currentIdx;
		fm_savepos(MTE_AUTO_LAST_SEARCH);
		_autoMarks.ml_currentIdx = nIdx;
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
long long fm_gotoLastPosition(MARK_TYPE_ENUM type) {
	return fm_gotoLastPositionDir(type, 1);
}

/*--------------------------------------------------------------------------
 * fm_gotoMextPosition()
 * Goto to the next navigation/insertion position depending on the mark type.
 */
long long fm_gotoNextPosition(MARK_TYPE_ENUM type) {
	return fm_gotoLastPositionDir(type, -1);
}

/*--------------------------------------------------------------------------
 * fm_canGotoNext()
 * Is a goto next for the given mark type supported?
 */
int fm_canGotoNext(long long llParam) {
	MARK* mp = fm_getSavedMark(_autoMarks.ml_lastType, -1, FALSE);
	return (mp != 0);
}

/*--------------------------------------------------------------------------
 * fm_canGotoLast()
 * Is a goto last for the given mark type supported?
 */
int fm_canGotoLast(long long llParam) {
	MARK* mp = fm_getSavedMark((MARK_TYPE_ENUM) llParam, 1, FALSE);
	return (mp != 0);
}


