/*
 * DisplayHelp.c
 *
 * PROJEKT: PKS-EDIT for MS - WINDOWS 3.0.1
 *
 * purpose: Handle help requests
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

# include	<windows.h>
# include	<HtmlHelp.h>
# include	"winterf.h"

extern char *searchfile(char *s);

static HWND hwndHelpRequested;
static char szHelpFile[] = "pksedit.chm";

/*--------------------------------------------------------------------------
 * EdCallWinHelp()
 */
int EdCallWinHelp(char *szFile, WORD hType, DWORD param)
{
	int		ret;
	LPSTR	pszFound;

	if (!szFile)
		szFile = szHelpFile;
	
	if (!(pszFound = searchfile(szFile))) {
		pszFound = szFile;
	}

	ret = HtmlHelp(hwndFrame, pszFound, HH_DISPLAY_TOC, 0);
	hwndHelpRequested = hwndFrame;
	return ret;
}

/*--------------------------------------------------------------------------
 * HelpQuit()
 */
void HelpQuit(void)
{
	if (hwndHelpRequested) {
		WinHelp(hwndHelpRequested, NULL, HELP_QUIT, 0);
	}
	hwndHelpRequested = NULL;
}

/*--------------------------------------------------------------------------
 * HelpKey()
 */
int HelpKey(LPSTR szFile, LPSTR szKey)
{
	return EdCallWinHelp(szFile,HELP_KEY,(DWORD)szKey);
}

/*--------------------------------------------------------------------------
 * EdHelp()
 */
int EdHelp(WORD hType, DWORD p)
{
	return EdCallWinHelp((char *)0,hType,p);
}

/*--------------------------------------------------------------------------
 * EdHelpContext()
 */
int EdHelpContext(WORD nCtx)
{
	return EdHelp(HELP_CONTEXT,(DWORD)nCtx);
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

