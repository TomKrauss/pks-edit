/*
 * EditorDocumentModel.c
 *
 * Basic operations on the document model of PKS Edit.
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * author: Tom
 * created: 29.12.1990
 */

#include <windows.h>
#include "alloc.h"
#include "linkedlist.h"
#include "documentmodel.h"
#include "winfo.h"

 /*------------------------------
  * ln_listfree()
  * Releases and frees a list of lines.
  *------------------------------*/
EXPORT void ln_listfree(register LINE* lp) {
	LINE* lps;

	while ((lps = lp) != 0) {
		lp = lp->next;
		free(lps);
	}
}

/*----------------------------------------------
 * ln_createAndAdd()
 * create a line and add it to the editor model
 */
BOOL ln_createAndAdd(FTABLE* fp, const char* q, int len, int flags) {
	LINE* lp;

	if (len < 0) {
		len = (int)strlen(q);
	}
	if ((lp = malloc(len + sizeof(LINE))) == 0L) {
		ln_listfree(fp->firstl);
		fp->firstl = 0;
		return FALSE;
	}
	lp->len = len;
	lp->lflg = flags;
	lp->next = 0L;
	lp->lbuf[len] = 0;
	if (q) {
		memmove(lp->lbuf, q, len);
	}
	LINE* lpCurr = fp->lpReadPointer;
	if (lpCurr != 0) {
		lp->prev = lpCurr;
		lp->next = lpCurr->next;
		if (lpCurr->next) {
			lpCurr->next->prev = lp;
		}
		lpCurr->next = lp;
		fp->lpReadPointer->next = lp;
	} else {
		fp->firstl = lp;
		lp->prev = NULL;
	}
	fp->lpReadPointer = lp;
	return TRUE;
}

/*----------------------------*/
/* ln_create() 			*/
/* create a new line. Pass the number of bytes required to hold the character content */
/*----------------------------*/
LINE* ln_create(int size) {
	LINE* pNew;

	if ((pNew = (LINE*)calloc(1, size + sizeof(LINE))) == 0L) {
		return(0);
	}
	pNew->len = size;
	return pNew;
}

/*----------------------------
 * ln_indexOf()
 * Find the index of a line relative to the beginning or 
 * return -1 if not found.
 */
long ln_indexOf(FTABLE* fp, LINE* lp) {
	return ll_indexOf((LINKED_LIST*)fp->firstl, (LINKED_LIST*)lp);
}

/*----------------------------*/
/* ln_cut()				*/
/* cut a line to linebuffer	*/
/*----------------------------*/
LINE* ln_cut(LINE* lp, int physize, int start, int end)
{
	LINE* new;
	int		len = end - start;

	if ((new = ln_create(physize)) == 0L) {
		return 0;
	}

	memmove(new->lbuf, &lp->lbuf[start], len);
	new->len = len;
	new->lbuf[len] = 0;
	new->lflg = (lp->lflg & (LNNOTERM | LNNOCR));
	return new;
}

