/*
 * CompareFiles.c
 *
 * Project: PKS Edit for MS-WINDOWS
 *
 * purpose: sort_compareRecords the lines of two windows
 * 		  diffs are marked in both windows, last equal line is positioned
 * 		  corresponding to the current setting of "cursor after search"
 *
 * 										created: 01.01.90
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include "alloc.h"

#include "trace.h"

#include "winterf.h"
#include "caretmovement.h"
#include "pksedit.h"
#include "edierror.h"
#include "winfo.h"
#include "linkedlist.h"
#include "mainframe.h"
#include "comparefiles.h"

/**
 * compare_areDifferent
 * Return TRUE; if two lines are different.
 */
static BOOL compare_areDifferent(LINE* lp1, LINE* lp2) {
	return lp1->len != lp2->len || strncmp(lp1->lbuf, lp2->lbuf, lp1->len) != 0;
}

/*--------------------------------------------------------------------------
 * compare_findGroupOfMatchingLines()
 */
/*
	Simple strategy applied so far:

	MINIMUM_MATCH_LINES is the minimum # of lines which should be equal
   	otherwise they are not matched against each other.
   	This trick helps keeping track, if a few equal lines 
	are detected at the cost of loosing some frags of equal
	(less then MINIMUM_MATCH_LINES) lines.

	Example:

	a b c d e d e f g h i
	a e f g h i

	matching chunks

	straight forward:	<a>-<a>,<e>-<e>
	with MINIMUM_MATCH_LINES trick: <a>-<a>,<f g h i> - <f g h i>
*/
static int MINIMUM_MATCH_LINES = 5;		
static LINE *compare_findGroupOfMatchingLines(LINE *lp1,LINE *lp2)
{	int n;
	LINE *l1,*l2;

	while(lp2) {
		if (!compare_areDifferent(lp1,lp2)) {
			n  = MINIMUM_MATCH_LINES;
			l1 = lp1; l2 = lp2;
			do {
				if ((l1 = l1->next) == 0 ||
				    (l2 = l2->next) == 0 ||
				    --n < 0) goto out;
			} while(!compare_areDifferent(l1,l2));
		}
		lp2 = lp2->next;
	}
out:
	return lp2;
}

/*--------------------------------------------------------------------------
 * compare_diffFiles()
 */
static long compare_diffFiles(WINFO* wp1, WINFO* wp2) {	
	FTABLE* fp1;
	FTABLE* fp2;
	LINE *lp1,*lp2,*lp,*lps1,*lps2,*lnlast1,*lnlast2;
	long bDiffFound = 0;
	LINE* lpFirstDifferent = NULL;

	fp1 = wp1->fp;
	fp2 = wp2->fp;
	lp1 = fp1->firstl; lp2 = fp2->firstl;
	lnlast1 = 0;
	lnlast2 = 0;
	ln_removeFlag(lp1, (LINE*)0L, LN_COMPARE_DIFFERENT);
	ln_removeFlag(lp2, (LINE*)0L, LN_COMPARE_DIFFERENT);
	while (lp1 && lp2) {
		if (!compare_areDifferent(lp1, lp2)) {
			lp1 = lp1->next;
			lp2 = lp2->next;
			continue;
		}
		lps1 = lp1;
		lps2 = lp2;
		bDiffFound = 1;
		do {
			if ((lp = compare_findGroupOfMatchingLines(lp1,lp2)) != (LINE *)0) {
				while (lp2 != lp) {
					lp2->lflg |= LN_COMPARE_DELETED;
					lp2 = lp2->next;
				}
				goto advance;
			}
			lp1->lflg |= LN_COMPARE_ADDED;
			lp1 = lp1->next;
		} while(lp1);
	advance:
		if (lpFirstDifferent == NULL) {
			lpFirstDifferent = lp1;
		}
		bDiffFound++;
		while (lps1 != lp1 && lps2 != lp2) {
			lps1->lflg &= ~LN_COMPARE_DIFFERENT;
			lps2->lflg &= ~LN_COMPARE_DIFFERENT;
			lps1->lflg |= LN_COMPARE_MODIFIED;
			lps2->lflg |= LN_COMPARE_MODIFIED;
			lps1 = lps1->next;
			lps2 = lps2->next;
		}
		lp1 = lp1->next;
		lp2 = lp2->next;
	}
	while (lp1 && lp1 != fp1->lastl) {
		bDiffFound = 1;
		lp1->lflg |= LN_COMPARE_ADDED;
		lp1 = lp1->next;
	}
	while (lp2 && lp2 != fp1->lastl) {
		bDiffFound = 1;
		lp2->lflg |= LN_COMPARE_DELETED;
		lp2 = lp2->next;
	}
	if (bDiffFound == 0) {
		error_showErrorById(IDS_MSGNOMOREDIFFS);
		return 0;
	}
	ww_connectWithComparisonLink(wp1, wp2);
	if (lpFirstDifferent != NULL) {
		long l1 = ln_cnt(fp1->firstl, lpFirstDifferent) - 1;
		caret_placeCursorAndMakevisibleWithSpace(wp1, l1, 0L);
		caret_placeCursorAndMakevisibleWithSpace(wp2, l1, 0L);
	}
	render_repaintWindow(wp1);
	render_repaintWindow(wp2);
	return bDiffFound;
}

/*
 * Clear the current comparison link.
 */
EXPORT long long compare_clear() {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (wp != NULL && wp->comparisonLink) {
		ww_releaseComparisonLink(wp, TRUE);
		return TRUE;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * compare_files()
 * Compare two files identified by their WINFO pointers.
 */
EXPORT int compare_files(WINFO* wp0, WINFO* wp1) {

	if (wp0 == NULL ||
	    wp1 == NULL) {
		error_showErrorById(IDS_MSGDIFFTWOWINDOWS);
		return 0;
	}
	mainframe_moveWindowAndActivate(wp1->edwin_handle, DOCK_NAME_RIGHT, TRUE);
	mainframe_moveWindowAndActivate(wp0->edwin_handle, DOCK_NAME_DEFAULT, TRUE);
	ww_setZoom(wp1, wp0->zoomMagnifier);
	return compare_diffFiles(wp0,wp1);
}

/*
 * Used to navigate the differences found during a comparison. wp points to the first
 * file which must be linked to file to compare to. aDirection might be 1 or -1 to navigate
 * to the next or previous difference 
 *
 * If navigation is successful, 1 is returned.
 */
EXPORT long long compare_navigate(int aDirection) {
	WINFO* wp = ww_getCurrentEditorWindow();
	if (wp == NULL) {
		return 0;
	}
	COMPARISON_LINK* pLink = wp->comparisonLink;
	if (pLink == NULL) {
		return 0;
	}
	WINFO* wpOther = pLink->cl_wpLeft == wp ? pLink->cl_wpRight : pLink->cl_wpLeft;
	LINE* lp1 = wp->caret.linePointer;
	LINE* lp2 = wpOther->caret.linePointer;
	if (!lp1 || !lp2) {
		return 0;
	}
	long ln1 = wp->caret.ln;
	long ln2 = wpOther->caret.ln;
	int m1Flag = lp1->lflg & LN_COMPARE_DIFFERENT;
	int m2Flag = lp2->lflg & LN_COMPARE_DIFFERENT;
	while (lp1 && lp2) {
		int m1 = lp1->lflg & LN_COMPARE_DIFFERENT;
		if (m1 && m1 != m1Flag) {
			caret_placeCursorForFile(wp, ln1, 0,0,0);
			return 1;
		}
		int m2 = lp2->lflg & LN_COMPARE_DIFFERENT;
		if (m2 && m2 != m2Flag) {
			caret_placeCursorForFile(wpOther, ln2, 0,0,0);
			return 1;
		}
		if (aDirection < 0) {
			lp1 = lp1->prev;
			lp2 = lp2->prev;
			ln1--;
			ln2--;
		} else {
			lp1 = lp1->next;
			lp2 = lp2->next;
			ln1++;
			ln2++;
		}
	}
	error_showErrorById(IDS_MSGNOMOREDIFFS);
	return 0;
}
