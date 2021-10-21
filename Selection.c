/*
 * Selection.c
 *
 * PROJEKT: PKS-EDIT for Windows
 *
 * purpose: text selection handling
 *
 * 										created: 04.02.89
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <tos.h>

#include "trace.h"
#include "linkedlist.h"
#include "textblocks.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"

#include "winfo.h"
#include "winterf.h"

# pragma hdrstop

#include "pksedit.h"
#include "fileutil.h"
#include "clipboard.h"
#include "edfuncs.h"

/*--------------------------------------------------------------------------
 * GLOBALS
 */

static PASTE 		_ubufs[1],_ubuf2;
static PASTELIST *	_plist;
static int   _curentUndoBufferIndex;

int				_nundo = 1;
PASTE	*		_undobuf = _ubufs;

/*--------------------------------------------------------------------------
 * bl_convertPasteBufferToText()
 * Convert a string buffer to a paste buffer.
 */
int bl_convertTextToPasteBuffer(PASTE* bp, unsigned char* pText, unsigned char* pEnd, char cSeparator1, char cSeparator2, char cCr) {
	FTABLE 	ft;
	memset(&ft, 0, sizeof ft);
	if (ln_createMultipleLinesUsingSeparators(&ft, pText, pEnd, cSeparator1, cSeparator2, cCr)) {
		bl_free(bp);
		bp->pln = ft.firstl;
		bp->nlines = ft.nlines;
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * bl_convertPasteBufferToText()
 * Convert a paste buffer to a regular string buffer.
 */
unsigned char *bl_convertPasteBufferToText(unsigned char *pDestination, unsigned char *pDestinationEnd, 
	PASTE *pPasteBuffer) {
	unsigned char *	s;
	unsigned char *	d = pDestination;
	LINE *			lp;

	pDestinationEnd--;
	lp = (pPasteBuffer) ? pPasteBuffer->pln : 0;
	while(lp) {
		s = lp->lbuf;
		while(d < pDestinationEnd && s < &lp->lbuf[lp->len]) {
			*d++ = *s++;
		}
		lp = lp->next;
		if (lp && d < pDestinationEnd) {
			*d++ = '\r';
			*d++ = '\n';
		}
	}
	*d = 0;
	return pDestination;
}

/*
 * Tries to return the text from the current selection in the passed buffer, assuming a maximum
 * of nCapacity characters to return.
 */
int bl_getSelectedText(char* pszBuf, size_t nCapacity) {
	PASTE* pp;
	PASTE  pbuf;

	*pszBuf = 0;
	WINFO* wp = ww_getCurrentEditorWindow();
	if (!ww_hasSelection(wp)) {
		return 0;
	}
	if (EdBlockCut(0, &pbuf)) {
		pp = bl_addrbyid(0, 0);
		bl_convertPasteBufferToText(pszBuf, &pszBuf[nCapacity - 2], pp);
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdGetSelectedText()
 * PKS Edit macro commad which gets the selected text and makes it available
 * to the macro interpreter.
 */
void EdGetSelectedText(void) {
	char		buf[80];	/* searchPattern[] !!!! */

	bl_getSelectedText(buf, sizeof buf);
	macro_returnString(buf);
}

/*--------------------------------------------------------------------------
 * bl_free()
 * free a pastebuffer
 */
EXPORT void bl_free(PASTE *buf)
{
	ln_listfree(buf->pln);
	memset((unsigned char*) buf,0,sizeof *buf);
}

/*--------------------------------------------------------------------------
 * bl_destroyPasteList()
 */
void bl_destroyPasteList() {	
	register PASTELIST *pp = _plist;
	register PASTELIST* next;

	while (pp) {
		bl_free(&pp->pbuf);
		next = pp->next;
		free(pp);
		pp = next;
	}
	_plist = 0;
}

/*--------------------------------------------------------------------------
 * EdBufferFree()
 * release all buffers
 */
EXPORT int EdBufferFree(void)
{
	if (error_displayYesNoConfirmation(IDS_MSGCLEARBUFFERS) == IDYES) {
		bl_free(_undobuf);
		bl_free(&_ubuf2);
		bl_destroyPasteList();
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * bl_cutTextWithOptions()
 * cut out a block of text
 * and (opt) delete it
 * pPasteBuffer == 0: only delete block
 */
EXPORT int bl_cutTextWithOptions(PASTE *pp,LINE *lnfirst,LINE *lnlast,
		int cfirst,int clast,int freeflg)
{	register LINE	*lpd,*lps,*lpnew,*lpx;
	register int	last;

	last = P_EQ(lnfirst,lnlast) ? clast : lnfirst->len;	

	if (pp) {
		if ((lpd = ln_cut(lnfirst,last-cfirst,cfirst,last)) == 0L) 
			return 0;
		pp->size = last - cfirst;
		pp->pln = lpd;
		pp->nlines = 1L;
	}

	lpnew = lnlast->prev; /* zum Checken ob sich lnlast bei ln_modify „ndert */
	lpx   = lnlast->next; /* und auch wirklich nicht lnlast->prev !		*/
	if (freeflg) {
		if ((lps = ln_modify(ft_getCurrentDocument(),lnfirst,last,cfirst)) == 0L) 
			return 0;
	} else lps = lnfirst;
	if (P_EQ(lnlast,lnfirst)) return 1;

	/* Au weia, lnlast k”nnte die tempor„re Zeile sei und von ln_modify
	   weggeschmissen werden, dann hat sie nach dem Aufruf einen anderen
	   Pointer, dafr diese harte Konstruktion, ob lpnew NIL ist muss fr
	   den Fall dass lnlast != lnfirst nicht berprft werden !!!!
	*/
	if (P_NE(lpx->prev,lnlast) && P_NE(lpnew->next,lnlast))
		lnlast = lpnew->next;

	for(;;) {
		lps = lps->next;
		if (P_EQ(lps,lnlast)) last = clast;
		else last = lps->len; 
		
		if (pp) {
			pp->nlines++;
			pp->size += (last+2);
			if ((lpnew = ln_cut(lps,last,0,last)) == 0L) 
				return 0;
			lpd->next = lpnew; 
			lpnew->prev = lpd;
		}
		if (P_EQ(lps,lnlast)) break;
		lpd = lpnew;
		if (freeflg) 
			ln_delete(ft_getCurrentDocument(),lps);
	}
	if (freeflg) {
		if ((lps = ln_modify(ft_getCurrentDocument(),lps,clast,0)) == 0L) return 0;
		if (!(ln_join(ft_getCurrentDocument(),lps->prev,lps,1))) return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * bl_cut()
 */
EXPORT int bl_cut(PASTE *pp,LINE *l1,LINE *l2,int c1,int c2,int freeflg,int colflg)
{
	if (colflg)
	    return bl_cutBlockInColumnMode(pp,l1,l2,freeflg);
	return bl_cutTextWithOptions(pp,l1,l2,c1,c2,freeflg); 
}

/*--------------------------------------------------------------------------
 * bl_readFileIntoPasteBuf()
 * Read the file 'fileName' and convert it to a PASTE buf data structure
 * given the line / record separator 'rs'.
 */
EXPORT int bl_readFileIntoPasteBuf(PASTE *pb, char* fileName, int rs /* Record Seperator */)
{	register	  LINE *ll;
	FTABLE rf;

	rf.firstl = 0;
	pb->pln = 0;
	if (ft_readfileWithOptions(&rf,fileName,rs)) {
		ll = rf.lastl->prev;
		if (ll->lflg & LNNOTERM) {
			ll->next = 0;
			free(rf.lastl);			/* rm -eof- Mark		*/
		} else {
			rf.lastl->len = 0;
			rf.nlines++;
		}
		pb->pln = rf.firstl;
		pb->nlines = rf.nlines;
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * bl_writePasteBufToFile()
 * Write a paste buffer to the file named 'fn'.
 */
EXPORT int bl_writePasteBufToFile(PASTE *pb, char* fn,  int mode) {	
	FTABLE rf;
	LINE	  *lp;
	int    ret = 0;

	if ((rf.firstl = lp = pb->pln) != 0) {
		while(lp->next)
			lp  = lp->next;
		lp->lflg = LNNOTERM;
		rf.lastl = 0;
		rf.documentDescriptor = doctypes_createDefaultDocumentTypeDescriptor();
		ret = ft_writefileAsWithFlags(&rf,fn,mode);
		free(rf.documentDescriptor);
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * bl_paste()
 * paste a textblock
 */
static _blkflg = 0;
EXPORT int bl_paste(PASTE *pb, WINFO *wp, LINE *lpd, int col, int colflg)
{
	LINE *	lps;
	LINE *	lsav;
	int		bBlkLines;
	FTABLE* fp;

	if ((lps = pb->pln) == 0L) {
		return 0;
	}
	fp = wp->fp;
	if (colflg && (_blkflg || (!P_EQ(pb,_undobuf)))) {
		return bl_pastecol(pb,wp,lpd,col);
	}

	bBlkLines = (wp->workmode & BLK_LINES);

	if (!bBlkLines) {
		if ((lpd = ln_break(fp,lpd,col)) == 0L) {
			return 0;
		}

		wp->caret.linePointer = lpd->prev;
		lpd = wp->caret.linePointer;
		if ((lpd = ln_join(fp,lpd,lps,0)) == 0L) {
			return 0;
		}
		lps = lps->next;
	}
	lpd = lpd->next;
	while (lps != 0L) {
		if ((lsav=ln_cut(lps,lps->len,0,lps->len)) == 0L) {
			return 0; 
		}
		lsav->lflg |= LNMODIFIED;
		ln_insert(fp,lpd,lsav);
		lps = lps->next;
	}
	if (bBlkLines) {
		return 1;
	}
	lpd = lpd->prev;
	return ((ln_join(fp,lpd,lpd->next,1)) ? 1 : 0);
}

/*--------------------------------------------------------------------------
 * bl_join()
 */
EXPORT int bl_join(PASTE *pd,PASTE *p2)
{	LINE *lp,*lptmp,*lps;
	int  len;

	if (!pd->pln) {
		memmove(pd,p2,sizeof(*pd));
		return 1;
	}

	pd->nlines += (p2->nlines-1);
	pd->size   += p2->size;

	lp = pd->pln;
	while (lp->next)
		lp = lp->next;
	lps = p2->pln;
	len = lps->len+lp->len;

	if ((lptmp = ln_cut(lp,len,0,lp->len)) == 0) {
		bl_free(p2);
		return 0;
	}

	memmove(&lptmp->lbuf[lptmp->len],lps->lbuf,lps->len);
	if (P_EQ(lp,pd->pln))
		pd->pln = lptmp;
	else lp->prev->next = lptmp;

	lptmp->len       = len;
	lptmp->lbuf[len] = 0;
	lptmp->next      = lps->next;
	if (lps->next)
		lps->next->prev = lptmp;

	free(lp);
	free(lps);
	return 1;
}

/*--------------------------------------------------------------------------
 * bl_delete()
 */
EXPORT int bl_delete(WINFO *wp, LINE *lnfirst, LINE *lnlast, int cfirst,
	int clast, int blkflg, int saveintrash)
{
	PASTE	*	ppTrash;
	PASTE		ppDummy;
	int			bSaveOnClip;

	if (saveintrash) {
		ppTrash = _undobuf;
		bSaveOnClip = 1;
	} else {
		ppTrash = &ppDummy;
		memset(ppTrash, 0, sizeof *ppTrash);
		bSaveOnClip = 0;
	}

	if (blkflg && ww_isColumnSelectionMode(wp)) {
		if (bSaveOnClip) {
			if (!bl_cutBlockInColumnMode(bl_addrbyid(0,0), lnfirst, lnlast,0)) {
				return 0;
			}
		}
		if (!bl_cutBlockInColumnMode(ppTrash,lnfirst,lnlast,1)) {
			return 0;
		}
	} else {
		if (bSaveOnClip) {
			if (!bl_cutTextWithOptions (bl_addrbyid(0,0) ,lnfirst,lnlast,cfirst,clast,0)) {
				return 0;
			}
		}
		if (!bl_cutTextWithOptions (ppTrash,lnfirst,lnlast,cfirst,clast,1)) {
			return 0;
		}
	}

	if (!saveintrash) {
		bl_free(ppTrash);
	}

	if (bSaveOnClip) {
		clp_setmine();
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * bl_undoIntoUnqBuffer()
 * enqueue next Pastebuffer to undolist
 */
EXPORT int bl_undoIntoUnqBuffer(WINFO* wp, LINE *lnfirst,LINE *lnlast,int cfirst,int clast,int blockflg)
			/* pointer to first and last line to enq	*/
			/* first column and last col to enq		*/
{	char	*fn;
	char tmpfile[512];

	if (_nundo > 1) {
		if (_nundo > 10) {
			_nundo = 10;
		}
		fn = config_getPKSEditTempPath(tmpfile,_curentUndoBufferIndex+'0');
		bl_writePasteBufToFile(_undobuf, fn, 0);
		_curentUndoBufferIndex++;
		if (_curentUndoBufferIndex >= _nundo) {
			_curentUndoBufferIndex = 0;
		}
	}

	bl_free(_undobuf);
	_blkflg = blockflg & 1;
	
	return bl_delete(wp, lnfirst, lnlast, cfirst, clast, _blkflg, 1);
}

/*--------------------------------------------------------------------------
 * bl_getBlockFromUndoBuffer()
 */
EXPORT PASTE *bl_getBlockFromUndoBuffer(int num)
{
	char	*fn;
	char	tmpfile[512];
	
	if (num < 0 || num >= _nundo)
		return 0;
	if (!num)
		return _undobuf;

	num = _curentUndoBufferIndex-num;
	if (num < 0)
		num += _nundo;

	fn = config_getPKSEditTempPath(tmpfile,num+'0');
	bl_free(&_ubuf2);

	if (file_exists(fn) || bl_readFileIntoPasteBuf(&_ubuf2, fn, -1) == 0) {
		return 0;
	}

	return &_ubuf2;
}

/*--------------------------------------------------------------------------
 * bl_validateTrashcanName()
 * Validate / generate the name of a "logical" trash can in PKS edit, which may contain
 * data accessible under that name.
 */
void bl_validateTrashcanName(char *pszValid) {
	int		i;
	
	for (i = 0; i < _nundo; i++) {
		*pszValid++ = i + '0';
	}
	*pszValid = 0;
}

/*--------------------------------------------------------------------------
 * bl_append()
 */
EXPORT int bl_append(PASTE *pb,LINE *lnfirst,LINE *lnlast,int cfirst,int clast)
{	PASTE _p;

	if (!bl_cutTextWithOptions(&_p,lnfirst,lnlast,cfirst,clast,0)) 
		return 0;
	return bl_join(pb,&_p);
}

/*--------------------------------------------------------------------------
 * bl_getTextBlock()
 * find a textblock in his linked list
 */
EXPORT PASTE *bl_getTextBlock(int id, PASTELIST *pl)
{
	while (pl != 0 && pl->id != id) {
		pl = pl->next;
	}
	return (pl == 0) ? (PASTE *) 0 : &pl->pbuf;
}

/*--------------------------------------------------------------------------
 * bl_collectClipboardIds)
 * Collect all clipboard identifiers in the passed parameter.
 */
EXPORT void bl_collectClipboardIds(char *pszValid)
{
	PASTELIST *pl;

	pl = _plist;
	while (pl != 0) {
		*pszValid++ = pl->id;
		pl = pl->next;
	}
	*pszValid = 0;
}

/*--------------------------------------------------------------------------
 * bl_lookupPasteBuffer()
 * Lookup a paste buffer given an id. If insert is 1, space occupied by a possibly existing paste
 * buffer is destroyed before the paste buffer is returned.
 */
EXPORT PASTE *bl_lookupPasteBuffer(int id,int insert,PASTELIST **header)
{	PASTELIST *pl;
	PASTE *pp;

	if ((pp = bl_getTextBlock(id, *header)) != (PASTE *) 0) {
		if (insert) {
			// make space for storing clipboard data.
			bl_free(pp);
		} 
		return pp;
	}

	if ((pl = ll_insert(header, sizeof * pl)) == 0L) {
		return 0;
	}
	pl->id = id;
	return  &pl->pbuf;
}

/*--------------------------------------------------------------------------
 * bl_addrbyid()
 */
EXPORT PASTE *bl_addrbyid(int id,int insert)
{
	return bl_lookupPasteBuffer(id,insert,&_plist);
}

/*--------------------------------------------------------------------------
 * bl_hasClipboardBlock()
 * Check, whether the cut text block with the given number exists and can be inserted.
 */
BOOL bl_hasClipboardBlock(BOOL isTrashCan, int blockNumber) {
	PASTE *pb;

	pb  = (isTrashCan) ? _undobuf : bl_addrbyid(blockNumber,0);
	if (!isTrashCan && !blockNumber && (!pb || !pb->pln))
		return IsClipboardFormatAvailable(CF_TEXT);
	return (pb && pb->pln) ? 1  : 0;
}

