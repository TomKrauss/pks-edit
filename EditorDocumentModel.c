/*
 * EditorDocumentModel.c
 *
 * Basic operations on the document model of PKS Edit.
 *
 * PROJECT: PKSEDIT/Command Editor
 *
 * (c) Pahlen & Krauﬂ					Author : TOM
 * All Rights Reserved.					created: 29.12.1990
 *									
 */
#include <windows.h>
#include "lineoperations.h"
#include "winfo.h"

 /*------------------------------
  * ln_listfree()
  * Releases and frees a list of lines.
  *------------------------------*/
EXPORT void ln_listfree(register LINE* lp) {
	LINE* lps;

	while ((lps = lp) != 0) {
		lp = lp->next;
		_free(lps);
	}
}

/*--------------------------------------------------------------------------
  * ln_createAndAddSimple()
  */
EXPORT int ln_createAndAddSimple(FTABLE* fp, char* b)
{
	return ln_createAndAdd(fp, b, (int)strlen(b), 0);
}

/*----------------------------------------------
 * ln_createAndAdd()
 * create a line and add it to the editor model
 */
BOOL ln_createAndAdd(FTABLE* fp, char* q, int len, int flags) {
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
	lp->attr = 0;
	lp->next = 0L;
	lp->lbuf[len] = 0;
	if (q) {
		memmove(lp->lbuf, q, len);
	}
	if ((lp->prev = fp->lpReadPointer) != 0) {
		fp->lpReadPointer->next = lp;
	} else {
		fp->firstl = lp;
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

