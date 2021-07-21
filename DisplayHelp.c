/*
 * DisplayHelp.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: Handle help requests
 *
 * 										created: 
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <windows.h>
#include <HtmlHelp.h>
#include "winterf.h"
#include "fileutil.h"

static HWND hwndHelpRequested;
static char szHelpFile[] = "pksedit.chm";

/*--------------------------------------------------------------------------
 * EdCallWinHelp()
 */
int EdCallWinHelp(char *szFile, UINT hType, DWORD_PTR param) {
	HWND		ret;
	LPSTR	pszFound;
	DWORD m_dwCookie;
	DWORD_PTR requestParam = (DWORD_PTR) &m_dwCookie;

	if (!szFile) {
		szFile = szHelpFile;
	}
	
	if (!(pszFound = file_searchFileInPKSEditLocation(szFile))) {
		pszFound = szFile;
	}

	ret = HtmlHelp(NULL, szHelpFile, HH_INITIALIZE, (DWORD_PTR)&m_dwCookie);
	if (!ret) {
		return 0;
	}
	if (hType == HH_DISPLAY_SEARCH) {
		hType = HH_DISPLAY_TOC;
	} else {
		requestParam = param;
	}
	ret = HtmlHelp(hwndMDIFrameWindow, pszFound, hType, requestParam);
	hwndHelpRequested = hwndMDIFrameWindow;
	return ret == NULL ? 0 : 1;
}

/*--------------------------------------------------------------------------
 * help_quitHelpSystem()
 */
void help_quitHelpSystem(void)
{
	if (hwndHelpRequested) {
		HtmlHelp(hwndHelpRequested, szHelpFile, HH_CLOSE_ALL, 0);
	}
	hwndHelpRequested = NULL;
}

/*--------------------------------------------------------------------------
 * help_showHelpForKey()
 */
int help_showHelpForKey(LPSTR szFile, LPSTR szKey)
{
	return EdCallWinHelp(szFile, HH_KEYWORD_LOOKUP, (DWORD_PTR)szKey);
}

/*--------------------------------------------------------------------------
 * EdHelp()
 */
int EdHelp(UINT hType, DWORD p)
{
	return EdCallWinHelp((char *)0,hType,p);
}

/*--------------------------------------------------------------------------
 * EdHelpContext()
 */
int EdHelpContext(DWORD nCtx)
{
	return EdHelp(HH_DISPLAY_TOC,(DWORD)nCtx);
}

