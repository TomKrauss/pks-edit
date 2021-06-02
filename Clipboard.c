/*
 * Clipboard.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: handle the standard clipboard
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * (c) Pahlen & Krauss
 *
 */

#include <windows.h>
#include "winterf.h"
#include "lineoperations.h"
#include "pksedit.h"
#include "errordialogs.h"
#include "clipboard.h"
#include "textblocks.h"

/*------------------------------------------------------------
 * clp_ismine()
 * check, whether we are clipboard owner
 */
EXPORT int clp_ismine(void)
{
	HWND hwndOwner;

	hwndOwner = GetClipboardOwner();
	return hwndOwner == hwndFrame;
}
 
/*------------------------------------------------------------
 * clp_setclipboarddata()
 */
static void clp_setclipboarddata(HANDLE hMem)
{
	if (!OpenClipboard(hwndFrame)) {
		return;
	}
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}

/*--------------------------------------------------------------------------
 * clp_makebufferhandle()
 */
static HANDLE clp_makebufferhandle(int whichBuffer)
{
	DWORD 	size;
	PASTE *	bp;
	LINE  *	lp;
	HANDLE 	hMem;
	char *	lpMem;
	int  nl = '\n';
	int  cr = '\r';

	if ((bp = bl_addrbyid(whichBuffer,0)) == 0) {
		return 0;
	}

	if (bp->pln == 0 ||
	    (size = ln_calculateMemorySizeRequired(bp->pln, nl, cr)) == 0) 
		return 0;

	if ((hMem = GlobalAlloc(GHND, (DWORD)size)) == 0 ||
	    (lpMem = GlobalLock(hMem)) == 0) {
		ed_error(IDS_MSGNOSPCE);
		return 0;
	}
	
	lp = bp->pln;

	while(size > (DWORD)lp->len) {
		memmove(lpMem,lp->lbuf,lp->len);
		lpMem += lp->len;
		size  -= lp->len;
		if ((lp = lp->next) == 0)
			break;
		if (cr >= 0 && size > 0) {
			*lpMem++ = cr;
			size --;
		}
		if (nl >= 0 && size > 0) {
			*lpMem++ = nl;
			size--;
		}
	}
	GlobalUnlock(hMem);

	return hMem;
}

/*------------------------------------------------------------
 * clp_setmine()
 * we are clipboard owner !
 */
EXPORT void clp_setmine(void) {
	clp_setclipboarddata((HANDLE)0);
}

/*------------------------------------------------------------
 * clp_setdata()
 * deliver contents of the clipboard to a caller
 * is called after a RENDER... message
 */
EXPORT int clp_setdata(int whichBuffer)
{
	HANDLE hMem;

	hMem = clp_makebufferhandle(whichBuffer);

	/* give away the handle ... */
	SetClipboardData(CF_TEXT,hMem);
	return 1;
}

/*------------------------------------------------------------
 * clp_getdata()
 * get contents of the clipboard for PKS-EDIT
 */
EXPORT int clp_getdata(void)
{	HANDLE 	hClip;
	PASTE *	bp = bl_addrbyid(0,0);
	LPSTR  	lpClip;
	LPSTR	lpTemp;
	FTABLE 	ft;
	SIZE_T 	size;
	int    	ret = 0;

	if (clp_ismine()) 
		return 1;

	if (!OpenClipboard(hwndFrame))
		return 0;

	if (bp != 0 &&
	    (hClip = GetClipboardData(CF_TEXT)) != 0 &&
	    (lpClip = GlobalLock(hClip)) != 0) {

		blfill(&ft,sizeof ft,0);
		size = GlobalSize(hClip);
		if ((lpTemp = _alloc(size+10)) != 0) {
			memmove(lpTemp, lpClip, (int)size);
			if (size > 0 && lpTemp[size-1] == 0) {
				// Get rid of trailing 0 char.
				size--;
			}
			lpTemp[size++] = '\n';
			lpTemp[size] = '\n';
			if (ln_createMultipleLinesUsingSeparators(&ft, lpTemp, lpTemp+size, '\n', -1, '\r')) {
				bl_free(bp);
				bp->pln = ft.firstl;
				bp->nlines = ft.nlines;
				ret = 1;
			}
            _free(lpTemp);
		}
		GlobalUnlock(hClip);
	}
	CloseClipboard();
	if (ret) {
		clp_setmine();
	}
	return ret;
}

/*--------------------------------------------------------------------------
 * EdShowClipboard()
 */
EXPORT int EdShowClipboard(int whichBuffer)
{
	HANDLE hMem;
	
	if (whichBuffer == 0 && !clp_ismine()) {
		;
	} else {
		hMem = clp_makebufferhandle(whichBuffer);
		clp_setclipboarddata(hMem);
	}
	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;

	if (!CreateProcess("CLIPBRD.EXE", NULL, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo)) {
		alert("Clipbrd.exe konnte nicht gestartet werden. Eventuell ist die Anwendung unter dieser Windows Version nicht mehr verfügbar.");
		return 0;
	}
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
	return 1;
}

