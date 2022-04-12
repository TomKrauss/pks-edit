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
#include <stdio.h>
#include <direct.h>
#include <io.h>

#include "trace.h"
#include "linkedlist.h"
#include "textblocks.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"
#include "arraylist.h"

#include "winfo.h"
#include "winterf.h"
#include "stringutil.h"
#include "dial2.h"
#include "pksrc.h"

#include "pksedit.h"
#include "fileutil.h"
#include "clipboard.h"
#include "pksmacro.h"
#include "ftw.h"
#include "xdialog.h"

/*--------------------------------------------------------------------------
 * GLOBALS
 */

static PASTELIST *	_plist;
static char* _pszAutosaveDir = "clipboards";
static char* _pszAutosavePrefix = "clip_";
static char* _pszAutosaveExtension = ".clipboard";

/*--------------------------------------------------------------------------
 * bl_lookupPasteList()
 * Lookup a paste list element given an id. If bInsert is 1, space occupied by a possibly existing paste
 * buffer is destroyed before the paste buffer is returned.
 */
static PASTELIST* bl_lookupPasteList(char* pszId, int bInsert, PASTE_LIST_TYPE tType, PASTELIST** header);

/*
 * In the clipboard directory execute a function for every auto-saved clipboard file.
 */
static void bl_withClipboardDirDo(FTWFUNC pFunc) {
	char szDirName[EDMAXDIRLEN];
	char szPattern[20];
	string_concatPathAndFilename(szDirName, config_getPKSEditTempPath(), _pszAutosaveDir);
	sprintf(szPattern, "*%s", _pszAutosaveExtension);
	_ftw(szDirName, pFunc, 1, szPattern, 0xFF);
}

/*
 * A clipboard buffer file was found during startup - restore it.
 */
static int bl_deleteClipboardFile(char* pszFile, DTA* pDta) {
	_unlink(pszFile);
	return 0;
}

/*
 * Calculate the name of a file in which the clipboard with the given name is stored. 
 */
static void bl_clipboardFilename(char* pszId, char *pszFileName, BOOL bEnsureDir) {
	char szDirName[EDMAXDIRLEN];
	char szFile[20 + sizeof(((PASTELIST*) 0)->pl_id)];

	string_concatPathAndFilename(szDirName, config_getPKSEditTempPath(), _pszAutosaveDir);
	if (bEnsureDir) {
		_mkdir(szDirName);
	}
	sprintf(szFile, "%s%s%s", _pszAutosavePrefix, pszId, _pszAutosaveExtension);
	string_concatPathAndFilename(pszFileName, szDirName, szFile);
}

/*
 * Whether a pastelist will be possibly saved on an external file automatically.
 */
static BOOL bl_hasBackingFile(PASTELIST* pl) {
	return pl->pl_type == PLT_NAMED_BUFFER;
}

/*
 * Save all named clipboards to external files. Invoked on exit.
 */
void bl_autosavePasteBuffers() {
	char szFileName[EDMAXPATHLEN];
	PASTELIST* pl = _plist;
	bl_withClipboardDirDo(bl_deleteClipboardFile);
	if (GetConfiguration()->options & O_SAVE_CLIPBOARDS_ON_EXIT) {
		while (pl != 0) {
			if (bl_hasBackingFile(pl)) {
				bl_clipboardFilename(pl->pl_id, szFileName, TRUE);
				bl_writePasteBufToFile(&pl->pl_pasteBuffer, szFileName, F_NORMOPEN);
			}
			pl = pl->pl_next;
		}
	}
}

/*
 * A clipboard buffer file was found during startup - restore it. 
 */
static int bl_clipboardFileFound(char* pszFile, DTA* pDta) {
	char szName[50];
	char szFileOnly[128];
	strcpy(szFileOnly, string_getBaseFilename(pszFile));
	int nPrefix = (int)strlen(_pszAutosavePrefix);
	int nPostfix = (int)strlen(_pszAutosaveExtension);
	long nLen = (long)strlen(szFileOnly) - nPrefix - nPostfix;
	if (nLen <= 0) {
		return 0;
	}
	strncpy(szName, szFileOnly + nPrefix, nLen);
	szName[nLen] = 0;
	PASTE* pBuffer = bl_lookupPasteBuffer(szName, 1, PLT_NAMED_BUFFER, &_plist);
	if (pBuffer) {
		FILE_READ_OPTIONS fro;
		memset(&fro, 0, sizeof fro);
		fro.fro_fileName = pszFile;
		if (!bl_readFileIntoPasteBuf(pBuffer, &fro)) {
			bl_free(pBuffer);
		}
	}
	return 0;
}

/*
 * Restore all paste buffers from external files.
 */
void bl_restorePasteBuffers() {
	if (GetConfiguration()->options & O_SAVE_CLIPBOARDS_ON_EXIT) {
		bl_withClipboardDirDo(bl_clipboardFileFound);
	}
}

/*--------------------------------------------------------------------------
 * bl_convertPasteBufferToText()
 * Convert a string buffer to a paste buffer. Note, that pText must be terminated by a cSeparator1 character.
 * TODO: fix the necessity of the trailing newline.
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
int bl_getSelectedText(WINFO* wp, char* pszBuf, size_t nCapacity) {
	PASTE* pp;
	PASTE  pbuf;

	*pszBuf = 0;
	if (!ww_hasSelection(wp)) {
		return 0;
	}
	if (bl_cutOrCopy(wp, 0, &pbuf)) {
		pp = bl_addrbyid(0, 0, PLT_CLIPBOARD);
		bl_convertPasteBufferToText(pszBuf, &pszBuf[nCapacity - 2], pp);
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * macro_getSelectedText()
 * PKS Edit macro commad which gets the selected text and makes it available
 * to the macro interpreter.
 */
char* macro_getSelectedText(WINFO* wp) {
	static char	buf[256];

	bl_getSelectedText(wp, buf, sizeof buf);
	return buf;
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
 * bl_deleteBufferNamed()
 * Delete a buffer with a given name and return 1 if successful and 0 otherwise.
 */
int bl_deleteBufferNamed(char* pszName) {
	PASTELIST* pl = ll_find(_plist, pszName);
	char szFilename[EDMAXPATHLEN];

	if (pl) {
		if (bl_hasBackingFile(pl)) {
			bl_clipboardFilename(pl->pl_id, szFilename, FALSE);
			_unlink(szFilename);
			bl_free(&pl->pl_pasteBuffer);
		}
		ll_delete(&_plist, pl);
	}
	return 0;
}


/*--------------------------------------------------------------------------
 * bl_destroyPasteList()
 */
void bl_destroyPasteList(BOOL bUnlinkFiles) {
	char szFilename[EDMAXPATHLEN];
	PASTELIST *pp = _plist;
	PASTELIST* next;

	while (pp) {
		bl_free(&pp->pl_pasteBuffer);
		bl_clipboardFilename(pp->pl_id, szFilename, FALSE);
		if (bUnlinkFiles) {
			_unlink(szFilename);
		}
		next = pp->pl_next;
		free(pp);
		pp = next;
	}
	_plist = 0;
}

/*--------------------------------------------------------------------------
 * bl_destroyAll()
 * release all buffers
 */
EXPORT int bl_destroyAll(void)
{
	if (error_displayYesNoConfirmation(IDS_MSGCLEARBUFFERS) == IDYES) {
		bl_destroyPasteList(TRUE);
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
EXPORT int bl_cutTextWithOptions(WINFO* wp, PASTE *pp,LINE *lnfirst,LINE *lnlast,
		int cfirst,int clast,int bDelete)
{	register LINE	*lpd,*lps,*lpnew;
	register int	last;
	FTABLE* fp = wp->fp;
	last = P_EQ(lnfirst,lnlast) ? clast : lnfirst->len;	

	if (pp) {
		if ((lpd = ln_cut(lnfirst,last-cfirst,cfirst,last)) == 0L) 
			return 0;
		pp->size = last - cfirst;
		pp->pln = lpd;
		pp->nlines = 1L;
	}

	lpnew = lnlast->prev; /* zum Checken ob sich lnlast bei ln_modify „ndert */
	BOOL bLastEliminated = fp->tln == lnlast;
	if (bDelete) {
		if ((lps = ln_modify(fp,lnfirst,last,cfirst)) == 0L) 
			return 0;
	} else lps = lnfirst;
	if (P_EQ(lnlast,lnfirst)) return 1;

	// This kludge works around the case that the ln_modify above "changes" the lnlast pointer, as
	// that had been our "temporary" line currently edited. (fp->tln).
	if (bLastEliminated)
		lnlast = lpnew->next;

	LINE* lpsNext = lps->next;
	while((lps = lpsNext) != 0) {
		if ( lps == lnlast) last = clast;
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
		// Save pointer to next line, as ln_delete may invalidate pointer.
		lpsNext = lps->next;
		if (bDelete) 
			ln_delete(fp,lps);
	}
	if (bDelete) {
		if ((lps = ln_modify(fp,lps,clast,0)) == 0L) return 0;
		if (lps->prev && !(ln_join(fp,lps->prev,lps,1))) return 0;
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * bl_cut()
 */
EXPORT int bl_cut(WINFO* wp, PASTE *pp,LINE *l1,LINE *l2,int c1,int c2,int freeflg,int colflg)
{
	if (colflg)
	    return bl_cutBlockInColumnMode(wp, pp,l1,l2,freeflg);
	return bl_cutTextWithOptions(wp, pp,l1,l2,c1,c2,freeflg); 
}

/*--------------------------------------------------------------------------
 * bl_readFileIntoPasteBuf()
 * Read the file 'fileName' and convert it to a PASTE buf data structure
 * given some read options.
 */
EXPORT int bl_readFileIntoPasteBuf(PASTE *pb, FILE_READ_OPTIONS *pOptions) {	
	LINE *ll;
	FTABLE rf;

	rf.firstl = 0;
	pb->pln = 0;
	if (ft_readfileWithOptions(&rf, pOptions)) {
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
		free(rf.documentDescriptor);
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

	memset(&rf, 0, sizeof rf);
	if ((rf.firstl = lp = pb->pln) != 0) {
		while(lp->next)
			lp  = lp->next;
		lp->lflg = LNNOTERM;
		rf.lastl = 0;
		rf.documentDescriptor = doctypes_createDefaultDocumentTypeDescriptor();
		ret = ft_writefileAsWithFlags(&rf,fn,mode,TRUE);
		free(rf.documentDescriptor);
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * bl_paste()
 * paste a textblock
 */
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
	if (colflg) {
		return bl_pastecol(pb,wp,lpd,col);
	}

	bBlkLines = (wp->workmode & WM_LINE_SELECTION);

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
	int clast, int blkflg, int bSaveOnClip)
{
	PASTE	*	ppTrash;
	PASTE		ppDummy;

	ppTrash = &ppDummy;
	memset(ppTrash, 0, sizeof *ppTrash);

	if (blkflg && ww_isColumnSelectionMode(wp)) {
		if (bSaveOnClip) {
			if (!bl_cutBlockInColumnMode(wp, bl_addrbyid(0,0, PLT_CLIPBOARD), lnfirst, lnlast,0)) {
				return 0;
			}
		}
		if (!bl_cutBlockInColumnMode(wp, ppTrash,lnfirst,lnlast,1)) {
			return 0;
		}
	} else {
		if (bSaveOnClip) {
			if (!bl_cutTextWithOptions (wp, bl_addrbyid(0,0, PLT_CLIPBOARD) ,lnfirst,lnlast,cfirst,clast,0)) {
				return 0;
			}
		}
		if (!bl_cutTextWithOptions (wp, ppTrash,lnfirst,lnlast,cfirst,clast,1)) {
			return 0;
		}
	}

	bl_free(ppTrash);

	if (bSaveOnClip) {
		clp_setmine();
	}
	return 1;
}

/*--------------------------------------------------------------------------
 * bl_append()
 */
EXPORT int bl_append(WINFO* wp, PASTE *pb,LINE *lnfirst,LINE *lnlast,int cfirst,int clast)
{	PASTE _p;

	if (!bl_cutTextWithOptions(wp, &_p,lnfirst,lnlast,cfirst,clast,0)) 
		return 0;
	return bl_join(pb,&_p);
}

/*--------------------------------------------------------------------------
 * bl_findPasteList()
 * find a textblock in the linked list if named clipboards.
 */
static PASTELIST* bl_findPasteList(char* pszId, PASTELIST* pl) {
	if (pszId == 0) {
		pszId = "";
	}
	return ll_find(pl, pszId);
}


static BOOL bl_isEqualBuffer(PASTE* pb1, PASTE* pb2) {
	LINE* lp1 = pb1->pln;
	LINE* lp2 = pb2->pln;

	while (lp1 && lp2) {
		if (lp1->len != lp2->len || (lp1->lflg & (LNNOCR | LNNOTERM)) != (lp2->lflg & (LNNOCR | LNNOTERM))) {
			return FALSE;
		}
		if (memcmp(lp1->lbuf, lp2->lbuf, lp1->len) != 0) {
			return FALSE;
		}
		lp1 = lp1->next;
		lp2 = lp2->next;
	}
	return lp1 == NULL && lp2 == NULL;
}

/*
 * bl_makeHistoryEntry()
 * Create a new paste list element to store a history buffer.
 * A history entry will only be created, if no identical buffer 
 * already exists (containing the same contents).
 */
static PASTELIST* bl_makeHistoryEntry(PASTE* pBuf) {
	int nHistory = 1;
	char szName[32];

	for (PASTELIST* pl = _plist; pl; pl = pl->pl_next) {
		if (pl->pl_type == PLT_HISTORY) {
			if (bl_isEqualBuffer(pBuf, &pl->pl_pasteBuffer)) {
				return NULL;
			}
			nHistory++;
		}
	}
	sprintf(szName, "#clip-%d", nHistory);
	PASTELIST* pl = bl_lookupPasteList(szName, TRUE, PLT_HISTORY, &_plist);
	if (pl) {
		memcpy(&pl->pl_pasteBuffer, pBuf, sizeof * pBuf);
		memset(pBuf, 0, sizeof * pBuf);
	}
	return pl;
}

/*--------------------------------------------------------------------------
 * bl_lookupPasteList()
 * Lookup a paste list element given an id. If bInsert is 1, space occupied by a possibly existing paste
 * buffer is destroyed before the paste buffer is returned.
 */
static PASTELIST* bl_lookupPasteList(char* pszId, int bInsert, PASTE_LIST_TYPE tType, PASTELIST** header) {
	PASTELIST* pl;
	PASTE* pp;

	if ((pl = bl_findPasteList(pszId, *header)) != (PASTELIST*)0) {
		pp = &pl->pl_pasteBuffer;
		if (bInsert) {
			if (tType == PLT_CLIPBOARD) {
				if (GetConfiguration()->options & O_SAVE_CLIPBOARD_HISTORY) {
					PASTELIST* plHistory = bl_makeHistoryEntry(pp);
					if (plHistory) {
						return pl;
					}
				}
			}
			// make space for storing clipboard data.
			bl_free(pp);
		}
		return pl;
	}

	if ((pl = ll_insert(header, sizeof * pl)) == 0L) {
		return 0;
	}
	if (!pszId) {
		pl->pl_id[0] = 0;
	} else {
		strcpy(pl->pl_id, pszId);
	}
	pl->pl_type = tType;
	return  pl;
}

/*--------------------------------------------------------------------------
 * bl_lookupPasteBuffer()
 * Lookup a paste buffer given an id. If bInsert is 1, space occupied by a possibly existing paste
 * buffer is destroyed before the paste buffer is returned.
 */
EXPORT PASTE *bl_lookupPasteBuffer(char* pszId, int bInsert, PASTE_LIST_TYPE tType, PASTELIST **header)
{
	PASTELIST* pl = bl_lookupPasteList(pszId, bInsert, tType, header);
	return pl == NULL ? NULL : &pl->pl_pasteBuffer;
}

/*--------------------------------------------------------------------------
 * bl_addrbyid()
 */
EXPORT PASTE *bl_addrbyid(char* pszId, int insert, PASTE_LIST_TYPE tType)
{
	return bl_lookupPasteBuffer(pszId,insert,tType, &_plist);
}

/*
 * Return the pastelist element selected in the dialog. 
 */
static PASTELIST* bl_getSelectedPastelist(HWND hDlg) {
	LRESULT nIndex = SendDlgItemMessage(hDlg, IDD_ICONLIST, LB_GETCURSEL, 0, 0);
	if (nIndex == LB_ERR) {
		return NULL;
	}
	LRESULT result = SendDlgItemMessage(hDlg, IDD_ICONLIST, LB_GETITEMDATA, nIndex, 0);
	if (result == LB_ERR) {
		return NULL;
	}
	return (PASTELIST*)result;
}

/*
 * An item in the template list was selected - update enablement etc...
 */
static SELECT_NAMED_CLIPBOARD_ACTION bl_namedClipboardDialogAction;
static void bl_templateSelected(HWND hdlg) {
	PASTELIST* pl = bl_getSelectedPastelist(hdlg);
	BOOL bEnable = pl != NULL && pl->pl_type != PLT_CLIPBOARD;
	EnableWindow(GetDlgItem(hdlg, IDD_BUT3), bEnable);
	EnableWindow(GetDlgItem(hdlg, IDOK), bl_namedClipboardDialogAction != SNCO_SELECT || pl != NULL);
	if (bl_namedClipboardDialogAction == SNCO_SELECT && !pl) {
		SetWindowText(GetDlgItem(hdlg, IDD_RO1), "");
	}
}

/*--------------------------------------------------------------------------
 * bl_namedBuffersOnSelectionChange()
 */
static char* bl_defaultClipboardName = "#default";
static void bl_namedBuffersOnSelectionChange(HWND hDlg, int nItem, int nNotify, void* unused) {
	char* pszTemplateContents;

	if (nNotify != LBN_SELCHANGE) {
		return;
	}

	PASTELIST* pl = bl_getSelectedPastelist(hDlg);
	if (pl != NULL) {
		pszTemplateContents = bl_getTextForClipboardNamed(&pl->pl_pasteBuffer, pl->pl_type == PLT_CLIPBOARD);
		SendDlgItemMessage(hDlg, IDD_RO1, EM_SETREADONLY, (WPARAM)TRUE, 0L);
		SetWindowText(GetDlgItem(hDlg, IDD_RO1), pszTemplateContents);
		SetWindowText(GetDlgItem(hDlg, IDD_STRING1), pl->pl_id);
	}
	bl_templateSelected(hDlg);
}

/*------------------------------------------------------------
 * bl_namedBuffersFillListbox()
 */
static void bl_namedBuffersFillListbox(HWND hwnd, int nItem, void* unused) {
	PASTELIST* pl;
	char* pszSelect;

	pl = _plist;
	SendDlgItemMessage(hwnd, nItem, LB_RESETCONTENT, 0, 0L);
	if (!pl) {
		return;
	}
	char szText[32];
	GetWindowText(GetDlgItem(hwnd, IDD_STRING1), szText, sizeof szText);
	pszSelect = szText;
	if (!pszSelect[0]) {
		pszSelect = bl_defaultClipboardName;
	}
	BOOL bOnlyNamed = SendDlgItemMessage(hwnd, IDD_OPT1, BM_GETCHECK, 0, 0) == BST_CHECKED;
	while (pl) {
		if (!bOnlyNamed || pl->pl_type == PLT_NAMED_BUFFER) {
			char* pszName = pl->pl_id;
			if (pszName[0] == 0) {
				pszName = bl_defaultClipboardName;
			}
			int nIndex = (int)SendDlgItemMessage(hwnd, nItem, LB_ADDSTRING, 0, (LPARAM)(LPSTR)pszName);
			SendDlgItemMessage(hwnd, nItem, LB_SETITEMDATA, nIndex, (LPARAM)pl);
		}
		pl = pl->pl_next;
	}
	if (pszSelect) {
		SendDlgItemMessage(hwnd, nItem, LB_SELECTSTRING, -1, (LPARAM)pszSelect);
	}
	bl_namedBuffersOnSelectionChange(hwnd, nItem, LBN_SELCHANGE, (void*)0);
}

/*
 * Custom dialog procedure for clipboard selector.
 */
static INT_PTR bl_namedClipboardDialogProc(HWND hdlg, UINT wMessage, WPARAM wParam, LPARAM lParam) {
	switch (wMessage) {
	case WM_INITDIALOG: {
		HWND hwndControl = GetDlgItem(hdlg, IDD_STRING1);
		if (bl_namedClipboardDialogAction == SNCO_LIST) {
			EnableWindow(GetDlgItem(hdlg, IDD_BUT3), FALSE);
		}
		if (bl_namedClipboardDialogAction == SNCO_CREATE) {
			PostMessage(hdlg, WM_NEXTDLGCTL, (WPARAM)hwndControl, TRUE);
			SetWindowText(GetDlgItem(hdlg, IDOK), dlg_getResourceString(IDS_CREATE_BUFFER));
		} else {
			EnableWindow(hwndControl, FALSE);
			if (bl_namedClipboardDialogAction == SNCO_SELECT) {
				SetWindowText(GetDlgItem(hdlg, IDOK), dlg_getResourceString(IDS_INSERT));
			}
		}
		break;
	}
	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDD_BUT3) {
			LRESULT nItem = (LRESULT)SendDlgItemMessage(hdlg, IDD_ICONLIST, LB_GETCURSEL, 0, 0);
			if (nItem >= 0) {
				char szText[32];
				SendDlgItemMessage(hdlg, IDD_ICONLIST, LB_GETTEXT, nItem, (LPARAM)szText);
				bl_deleteBufferNamed(szText);
			}
			bl_namedBuffersFillListbox(hdlg, IDD_ICONLIST, 0);
			return 0;
		} else if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDD_OPT1) {
			bl_namedBuffersFillListbox(hdlg, IDD_ICONLIST, 0);
			return 0;
		}
		break;
	case WM_KEYDOWN:
		break;
	}
	return dlg_standardDialogProcedure(hdlg, wMessage, wParam, lParam);
}

/*--------------------------------------------------------------------------
 * bl_showClipboardList()
 * displays the clipboard list for the purpose of viewing or selecting or adding a named item
 * to the clipboard.
 */
char* bl_showClipboardList(SELECT_NAMED_CLIPBOARD_ACTION bOption) {
	static char selectedTemplate[32];
	DIALLIST tmplatelist = {
		NULL, bl_namedBuffersFillListbox, dlg_getListboxText, 0, 0, bl_namedBuffersOnSelectionChange };
	DIALPARS _d[] = {
		IDD_POSITIONTCURS,	0,			0,
		IDD_ICONLIST,		0,			&tmplatelist,
		IDD_STRING1,		sizeof selectedTemplate,	selectedTemplate,
		0
	};

	bl_namedClipboardDialogAction = bOption;
	int ret = win_callDialogCB(DLGSELTMPLATE, NULL, _d, NULL, bl_namedClipboardDialogProc);
	if (!ret) {
		return 0;
	}
	if (strcmp(selectedTemplate, bl_defaultClipboardName) == 0) {
		selectedTemplate[0] = 0;
	}
	return selectedTemplate;
}

