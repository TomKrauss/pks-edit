/*
 * Clipboard.c
 *
 * Project: PKS Edit for Windows
 *
 * purpose: handle the standard clipboard
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 */

#include <windows.h>
#include "alloc.h"
#include "winterf.h"
#include "documentmodel.h"
#include "pksedit.h"
#include "errordialogs.h"
#include "clipboard.h"
#include "textblocks.h"

#/*------------------------------------------------------------
 * clp_ismine()
 * check, whether we are clipboard owner
 */
EXPORT int clp_ismine(void)
{
	HWND hwndOwner;

	hwndOwner = GetClipboardOwner();
	return hwndOwner == hwndMain;
}
 
/*------------------------------------------------------------
 * clp_setclipboarddata()
 */
static void clp_setclipboarddata(HANDLE hMem)
{
	if (!OpenClipboard(hwndMain)) {
		return;
	}
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hMem);
	CloseClipboard();
}

/*--------------------------------------------------------------------------
 * clp_makebufferhandle()
 */
static HANDLE clp_makebufferhandle(char* whichBuffer) {
	size_t 	size;
	PASTE *	bp;
	HANDLE 	hMem;
	char *	lpMem;
	int  nl = '\n';
	int  cr = '\r';

	if ((bp = bl_addrbyid(whichBuffer,0, whichBuffer == NULL ? PLT_CLIPBOARD : PLT_NAMED_BUFFER)) == 0) {
		return 0;
	}

	if (bp->pln == 0 ||
	    (size = ln_calculateMemorySizeRequired(NULL, bp->pln, nl, cr)) == 0) 
		return 0;

	if ((hMem = GlobalAlloc(GHND, (DWORD)size)) == 0 ||
	    (lpMem = GlobalLock(hMem)) == 0) {
		error_showErrorById(IDS_MSGNOSPCE);
		return 0;
	}
	ft_convertToBuffer(NULL, lpMem, &size, bp->pln);

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
EXPORT int clp_setdata(char* whichBuffer)
{
	HANDLE hMem;

	hMem = clp_makebufferhandle(whichBuffer);

	/* give away the handle ... */
	SetClipboardData(CF_TEXT,hMem);
	return 1;
}

/*------------------------------------------------------------
 * clp_getdata()
 * get contents of the clipboard for PKS Edit
 */
EXPORT int clp_getdata(void)
{	HANDLE 	hClip;
	PASTE *	bp = bl_addrbyid(0,0,PLT_CLIPBOARD);
	LPSTR  	lpClip;
	int    	ret = 0;

	if (clp_ismine()) 
		return 1;

	if (!OpenClipboard(hwndMain))
		return 0;

	if (bp != 0 &&
	    (hClip = GetClipboardData(CF_TEXT)) != 0 &&
	    (lpClip = GlobalLock(hClip)) != 0) {

		size_t size = GlobalSize(hClip);
		LPSTR	lpTemp;
		if ((lpTemp = malloc(size+10)) != 0) {
			memmove(lpTemp, lpClip, (int)size);
			if (size > 0 && lpTemp[size-1] == 0) {
				// Get rid of trailing 0 char.
				size--;
			}
			lpTemp[size++] = '\n';
			lpTemp[size] = '\n';
			ret = bl_convertTextToPasteBuffer(bp, lpTemp, lpTemp + size, '\n', -1, '\r');
            free(lpTemp);
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
EXPORT int EdShowClipboard(char* pszBuffer) {
	bl_showClipboardList(SNCO_LIST);
	return 1;
}

