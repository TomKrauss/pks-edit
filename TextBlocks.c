/*
 * TextBlocks.c
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * purpose: textblock basic delta
 *
 * 										created: 04.02.89
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauﬂ
 */

#include <windows.h>
#include <tos.h>

#include "trace.h"
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
#include "desktopicons.h"
#include "edfuncs.h"

/*--------------------------------------------------------------------------
 * GLOBALS
 */

static PASTE 		_ubufs[1],_ubuf2;
static PASTELIST *	_plist;
static int   _curru;

int				_nundo = 1;
PASTE	*		_undobuf = _ubufs;

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

/*--------------------------------------------------------------------------
 * EdGetSelectedText()
 * PKS Edit macro commad which gets the selected text and makes it available
 * to the macro interpreter.
 */
void EdGetSelectedText(void) {
	char		buf[80];	/* searchPattern[] !!!! */
	PASTE *	pp;
	PASTE  pbuf;

	*buf = 0;
	if (EdBlockCut(0, &pbuf)) {
		pp = bl_addrbyid(0, 0);
		bl_convertPasteBufferToText(buf, &buf[sizeof buf -2], pp);
	}
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
 * bl_freePasteList()
 */
EXPORT void bl_freePasteList(PASTELIST **header) {	
	register PASTELIST *pp = *header;
	register PASTELIST* next;

	while (pp) {
		bl_free(&pp->pbuf);
		next = pp->next;
		_free(pp);
		pp = next;
	}
	*header = 0;
}

/*--------------------------------------------------------------------------
 * EdBufferFree()
 * release all buffers
 */
EXPORT int EdBufferFree(void)
{
	if (errorDisplayYesNoConfirmation(IDS_MSGCLEARBUFFERS) == IDYES) {
		bl_free(_undobuf);
		bl_free(&_ubuf2);
		bl_freePasteList(&_plist);
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

	lpnew = lnlast->prev; /* zum Checken ob sich lnlast bei ln_modify Ñndert */
	lpx   = lnlast->next; /* und auch wirklich nicht lnlast->prev !		*/
	if (freeflg) {
		if ((lps = ln_modify(ft_getCurrentDocument(),lnfirst,last,cfirst)) == 0L) 
			return 0;
	} else lps = lnfirst;
	if (P_EQ(lnlast,lnfirst)) return 1;

	/* Au weia, lnlast kînnte die temporÑre Zeile sei und von ln_modify
	   weggeschmissen werden, dann hat sie nach dem Aufruf einen anderen
	   Pointer, dafÅr diese harte Konstruktion, ob lpnew NIL ist muss fÅr
	   den Fall dass lnlast != lnfirst nicht ÅberprÅft werden !!!!
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
	_icdirty = 1;
	if (colflg)
	    return bl_cutBlockInColumnMode(pp,l1,l2,freeflg);
	return bl_cutTextWithOptions(pp,l1,l2,c1,c2,freeflg); 
}

/*--------------------------------------------------------------------------
 * bl_read()
 */
EXPORT int bl_read(char *fn, PASTE *pb, int rs /* Record Seperator */)
{	register	  LINE *ll;
	FTABLE rf;

	rf.firstl = 0;
	pb->pln = 0;
	if (ft_readfileWithOptions(&rf,fn,rs)) {
		ll = rf.lastl->prev;
		if (ll->lflg & LNNOTERM) {
			ll->next = 0;
			_free(rf.lastl);			/* rm -eof- Mark		*/
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
 * bl_write()
 */
EXPORT int bl_write(char *fn, PASTE *pb,int mode)
{	FTABLE rf;
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
EXPORT int bl_paste(PASTE *pb, FTABLE *fp, LINE *lpd, int col, int colflg)
{
	LINE *	lps;
	LINE *	lsav;
	int		bBlkLines;

	if ((lps = pb->pln) == 0L) {
		return 0;
	}

	if (colflg && (_blkflg || (!P_EQ(pb,_undobuf)))) {
		return bl_pastecol(pb,fp,lpd,col);
	}

	bBlkLines = (fp->documentDescriptor->workmode & BLK_LINES);

	if (!bBlkLines) {
		if ((lpd = ln_break(fp,lpd,col)) == 0L) {
			return 0;
		}

		fp->caret.linePointer = lpd->prev;
		lpd = fp->caret.linePointer;
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

	_free(lp);
	_free(lps);
	return 1;
}

/*--------------------------------------------------------------------------
 * bl_delete()
 */
EXPORT int bl_delete(FTABLE *fp, LINE *lnfirst, LINE *lnlast, int cfirst,
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

	if (blkflg && ww_hasColumnSelection(WIPOI(fp))) {
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
EXPORT int bl_undoIntoUnqBuffer(LINE *lnfirst,LINE *lnlast,int cfirst,int clast,int blockflg)
			/* pointer to first and last line to enq	*/
			/* first column and last col to enq		*/
{	char	*fn;
	char tmpfile[512];

	if (_nundo > 1) {
		if (_nundo > 10) {
			_nundo = 10;
		}
		fn = file_getTempFilename(tmpfile,_curru+'0');
		bl_write(fn, _undobuf, 0);
		_curru++;
		if (_curru >= _nundo) {
			_curru = 0;
		}
	}

	bl_free(_undobuf);
	_blkflg = blockflg & 1;
	
	return bl_delete(ft_getCurrentDocument(), lnfirst, lnlast, cfirst, clast, _blkflg, 1);
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

	num = _curru-num;
	if (num < 0)
		num += _nundo;

	fn = file_getTempFilename(tmpfile,num+'0');
	bl_free(&_ubuf2);

	if (file_exists(fn) || bl_read(fn,&_ubuf2,-1) == 0)
		return 0;

	return &_ubuf2;
}

/*--------------------------------------------------------------------------
 * bl_validateTrashCanName()
 * Validate / generate the name of a "logical" trash can in PKS edit, which may contain
 * data accessible under that name.
 */
void bl_validateTrashCanName(char *pszValid) {
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
	while (pl != 0 && pl->id != id)
		pl = pl->next;

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
 */
EXPORT PASTE *bl_lookupPasteBuffer(int id,int insert,PASTELIST **header)
{	PASTELIST *pl;
	PASTE *pp;

	if ((pp = bl_getTextBlock(id, *header)) != (PASTE *) 0) {
		if (insert)
			bl_free(pp);
		return pp;
	}

	if ((pl = (PASTELIST *) ll_insert((LINKED_LIST**)header,sizeof *pl)) == 0L) 
		return 0;
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

