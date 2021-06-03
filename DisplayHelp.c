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
 * (c) Pahlen & Krauss
 *
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
int EdCallWinHelp(char *szFile, UINT hType, DWORD_PTR param)
{
	static int initialized = FALSE;
	HWND		ret;
	LPSTR	pszFound;
	DWORD m_dwCookie;

	if (!szFile)
		szFile = szHelpFile;
	
	if (!(pszFound = file_searchFileInPKSEditLocation(szFile))) {
		pszFound = szFile;
	}

	if (!initialized) {
		HtmlHelp(NULL, szHelpFile, HH_INITIALIZE, (DWORD_PTR)&m_dwCookie);
		initialized = TRUE;
	}
	if (hType == HH_DISPLAY_SEARCH) {
		hType = HH_DISPLAY_TOC;
	}
	ret = HtmlHelp(hwndFrame, pszFound, hType, (DWORD_PTR)&m_dwCookie);
	hwndHelpRequested = hwndFrame;
	return ret == NULL ? 0 : 1;
}

/*--------------------------------------------------------------------------
 * HelpQuit()
 */
void HelpQuit(void)
{
	if (hwndHelpRequested) {
		HtmlHelp(hwndHelpRequested, szHelpFile, HH_CLOSE_ALL, 0);
	}
	hwndHelpRequested = NULL;
}

/*--------------------------------------------------------------------------
 * HelpKey()
 */
int HelpKey(LPSTR szFile, LPSTR szKey)
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
int EdHelpContext(WORD nCtx)
{
	return EdHelp(HH_DISPLAY_TOC,(DWORD)nCtx);
}

#if 0
/*--------------------------------------------------------------------------
 * HelpMultiKey()
 */
int HelpMultiKey(LPSTR szFile, char nC, LPSTR szKey)
{	int ret;
	char schlunz[512];
	MULTIKEYHELP *mk;

	mk = (MULTIKEYHELP*)schlunz;
	mk->mkSize = sizeof *mk + (WORD)lstrlen(szKey);
	lstrcpy(mk->szKeyphrase,szKey);
	mk->mkKeylist = nC;
	if (!szFile)
		szFile = _szHelpFile;
	ret = WinHelp(hwndFrame,szFile,HELP_MULTIKEY,(DWORD)mk);
	hwndHelpRequested = hwndFrame;
	return ret;
}
# endif

