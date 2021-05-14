/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * SelectFiles.c
 *
 * maintain file selection
 *
 * 						created: 07.06.91 
 *						Author : TOM
 */

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <direct.h>
#include <string.h>

#include "trace.h"
#include "editab.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"

#include "tos.h"
#include "edhist.h"

#include "edfuncs.h"
#include "pathname.h"
#include "edifsel.h"
#include "fsel.h"
#include "pksedit.h"

extern void 	_free(void *);
extern void 	sfsplit(char *source, char *path, char *fn);
extern unsigned char* stralloc(unsigned char* buf);
extern char *	FullPathName(char *path,char *fn);
extern int  	GetPksProfileString(char *grp, char *ident, char *string, int maxlen);
extern char *	AbbrevName(char *fn);
extern LPSTR 	lstrchr(LPSTR str, char ch);
extern void 	linealdocstrings(LPSTR pszDest, int nMax);
extern int 		DoDocumentTypes(int nDlg);
extern void 	GetStdMenuText(int menunr, char *text);

extern int	nCurrentDialog;

extern char *	_datadir;

static char *	sTitleSpec;
static int	_optionsenabled;

char _fseltarget[512];

/*--------------------------------------------------------------------------
 * ChangeDirectory()
 */
void ChangeDirectory(LPSTR pszPath)
{
	char 	cDrv;
	LPSTR	pszTrailer;

	if (pszPath[1] == ':') {
		cDrv = pszPath[0] - (pszPath[0] > 'Z' ? 'a' : 'A');
		_chdrive(cDrv);
	}

	/* remove trailing \\ */
	for (pszTrailer = pszPath; *pszTrailer; pszTrailer++) {
		if (*pszTrailer == '\\' && pszTrailer[1] == 0) {
			pszTrailer[0] = 0;
			break;
		}
	}

	chdir(pszPath);
}

/*---------------------------------*/
/* cons_fname()				*/
/* Erzeugen eines Filenamens aus	*/
/* den Parametern des AES-Filesel. */
/*---------------------------------*/
void cons_fname(char *fselpath,char *fselname,char *totalname)
{	char fn[256];

	sfsplit(fselpath,totalname,fn);
	lstrcat(totalname,fselname);
}

/*------------------------------------------------------------
 * fsel_optionsenable
 * enable options button for next file selector
 */
void fsel_optionsenable(int mode)
{
	_optionsenabled = mode;
}

/*------------------------------------------------------------
 * fsel_title
 * setting the title for the next call of EdFsel
 */
void fsel_title(char *title)
{
	if (sTitleSpec)
		_free(sTitleSpec);
	sTitleSpec = stralloc(title);
}

/*---------------------------------*/
/* menu_fseltitle()				*/
/*---------------------------------*/
static void menu_fseltitle(int title)
{	char *d,*s,szTemp[512];

	if (title > 0) {
		GetStdMenuText(title,szTemp);
		/* skip & - menu marks and key idents */
		for (s = szTemp, d = s; *s; ) {
			if (*s == '\010' || *s == '\t') {
				break;
			}
			if (*s == '&' && s[1]) {
				s++;
			}
			*d++ = *s++;
		}
		*d = 0;
		fsel_title(szTemp);
	}
}

/*---------------------------------*/
/* file_select()				*/
/*---------------------------------*/
int file_select(int title, char *path, char *fnam, char *pattern)
{
	int	nSave;
	int 	ret;
	char pathname[512];

	menu_fseltitle(title);
	strdcpy(pathname,path,pattern);
	nSave = nCurrentDialog;
	nCurrentDialog = title;
	ret = EdFsel(pathname,fnam,_fseltarget, title == MSAVEAS);
	nCurrentDialog = nSave;
	sfsplit(pathname,path,pattern);
	if (_fseltarget[0] == 0) {
		return 0;
	}

	return(ret);
}

/*---------------------------------*/
/* rw_select()					*/
/*---------------------------------*/
char *rw_select(FSELINFO *fp, int title)
{
	static ITEMS	_i = { C_STRING1PAR, _fseltarget };
	static PARAMS	_p = { DIM(_i), P_MAYOPEN, _i	};

	if (param_dialopen(&_p)) {

		if (fp->path[0] == 0) {	
			lstrcpy(fp->path,_datadir);
		}

		if (fp->search[0] == 0) {
			lstrcpy(fp->search, "*.*");
		}

		if (!file_select(title,fp->path,fp->fname,fp->search))
			return (char *)0;
		param_record(&_p);
	}
	return _fseltarget;
}

/*---------------------------------*/
/* rw_init()					*/
/*---------------------------------*/
char *rw_init(FSELINFO *fp)
{	char *fn;
	extern char *searchfile(char *);

	if ((fn = searchfile(fp->fname)) != 0) {
		sfsplit(fn,fp->path,fp->fname);
		if (fp->path[0] == 0)
			lstrcpy(fp->path,_datadir);
		lstrcpy(_fseltarget,fn);
		fn = _fseltarget;
	}
	return fn;
}

/*------------------------------------------------------------
 * sh_rename()
 */
static BOOL sh_rename(char *fn, WORD strId)
{
	char path[512];
	char fname[512];

	getcwd(path,sizeof path);
	strdcpy(fname,path,fn);
	lstrcpy(path,fn);
	if (PromptString(strId,AbbrevName(fname),path) == IDOK) {
		if (EdIsDir(path)) {
			strdcpy(path,path,fn);
		}
		if (Frename(0,fname,path)) {
			form_error(-1);
		} else {
			return TRUE;
		}
	}
	return FALSE;
}

/*------------------------------------------------------------
 * sh_delmkd()
 */
static BOOL sh_delmkd(char *fn, int strId)
{
	int  ret;
	char path[EDMAXPATHLEN];
	char fname[EDMAXFNLEN];

	if (!_getcwd(path, sizeof path)) {
		return FALSE;
	}
	strdcpy(fname,path,fn);
	if (*fn) {
		if (!(_options & WARNINGS) || 
			PromptString(strId,AbbrevName(fname),(char*)0) == IDOK) {
			char oemname[EDMAXPATHLEN];

			AnsiToOem(fname,oemname);
			if (strId == IDS_QUERYMKDIR) {
				ret = Dcreate(oemname);
			} else {
				if ((ret = Fdelete(oemname)) != 0)
				    ret = Ddelete(oemname);
			} 
			if (ret) {
				form_error(ret);
				return FALSE;
			} else {
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * fill_listwithtokens()
 *
 * fill a list control with an by ',' or ';' sepearated list of string items
*/
static fill_listwithtokens(HWND hDlg, WORD nItem, char *src)
{
	char 	szTemp[EDMAXPATHLEN];
	LPSTR	s;
	LPSTR	pszSeparator;
	int  	n;
	int		nCurr;
	HWND 	hwndList;

	hwndList = GetDlgItem(hDlg, nItem);
	nCurr = SendMessage(hwndList, CB_GETCURSEL, 0, 0L);
	if (nCurr < 0) {
		nCurr = 0;
	}
	SendMessage(hwndList, CB_RESETCONTENT, 0, 0L);
	SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);

	lstrcpy(szTemp,src);
	/*
	 * split a list of [;,] - separated pathnames (optionally containing
	 * extensions) and add them to a combo list
	 * all | - characters are translated to ';', so multiple extensions
	 * for the OpenFile common dialog are possible
	 */
	for (s = strtok(szTemp,",;"), n = 0; s; s = strtok((char*)0,",;")) {
		for (pszSeparator = s; 
			(pszSeparator = lstrchr(pszSeparator, '|')) != 0;
			*pszSeparator++ = ';')
			;
		if (SendMessage(hwndList, CB_ADDSTRING, 0, (LONG)s) < 0) {
			return 0;
		}
	}
	SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
	SendMessage(hwndList, CB_SETCURSEL, nCurr, 0L);
	SendRedraw(hwndList);
	
	return n;
}

/*--------------------------------------------------------------------------
 * SendOpenFileSelected()
 */
static void SendOpenFileSelected(HWND hDlg, LPSTR pszPath, LPSTR pszExt)
{
	char		szSelector[EDMAXPATHLEN];
	char		szSaveFn[512];

	strdcpy(szSelector, pszPath, *pszExt ? pszExt : "*.*");
	SendDlgItemMessage(hDlg, IDD_FSELNAME, WM_SETREDRAW, FALSE, 0L);
	GetDlgItemText(hDlg, IDD_FSELNAME, szSaveFn, sizeof szSaveFn);
	SetDlgItemText(hDlg, IDD_FSELNAME, szSelector);
	SendMessage(hDlg, WM_COMMAND, IDOK, 0L);
	SetDlgItemText(hDlg, IDD_FSELNAME, szSaveFn);
	SendDlgItemMessage(hDlg, IDD_FSELNAME, WM_SETREDRAW, TRUE, 0L);
}

/*--------------------------------------------------------------------------
 * FileOpenHookProc()
 */
UINT CALLBACK FileOpenHookProc(HWND hDlg, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
	char	szTemp[256];
	char	szPath[EDMAXPATHLEN];
	char	szExt[EDMAXFNLEN];
	char *	pszFound;
	BOOL	bRet;
	DWORD	dwIndex;
	int		nPathIndex;
	int		nId;

	switch(msg) {

	case WM_INITDIALOG:
		GetPksProfileString("desk", "FselPath", szTemp, 
			sizeof szTemp);
		fill_listwithtokens(hDlg, IDD_FSELPATHES, szTemp);
		for (nPathIndex = 0; 
			(pszFound = hist_getstring(&_pathhist, nPathIndex)) != 0;
			nPathIndex++) {
			if (SendDlgItemMessage(hDlg, IDD_FSELPATHES, CB_ADDSTRING, 
				0, (LONG)pszFound) < 0) {
				break;
			}
		}
		ShowWindow(GetDlgItem(hDlg, IDD_FSELOPTIONS),
			_optionsenabled ? SW_SHOW : SW_HIDE);
		form_move(hDlg);
     	return TRUE;

     case WM_COMMAND:
		switch(nId = GET_WM_COMMAND_ID(wParam, lParam)) {
	     case IDD_FSELPATHES:
			if (GET_WM_COMMAND_CMD(wParam, lParam) == CBN_SELCHANGE) {
				dwIndex = SendDlgItemMessage(hDlg, 
					nId, CB_GETCURSEL, 0, 0);
				if (dwIndex != CB_ERR) {
 					SendDlgItemMessage(hDlg, nId,
        					CB_GETLBTEXT, (WPARAM) dwIndex, 
						(LPARAM) ((LPSTR) szTemp));
					sfsplit(szTemp, szPath, szExt);
					SendOpenFileSelected(hDlg, szPath, szExt);
				}
			}
     	     return TRUE; /* No standard processing. */
		case IDD_FSELOPTIONS:
			DoDocumentTypes(DLGSELECTDOCTYPE);
			return TRUE;
		case IDD_FSELDELETE:
		case IDD_FSELMKDIR:
		case IDD_FSELRENAME:
			GetDlgItemText(hDlg,IDD_FSELNAME, szTemp,
					sizeof szTemp);
			bRet = FALSE;
			if (nId == IDD_FSELRENAME) {
				bRet = sh_rename(szTemp, IDS_RENAME);
			} else {
				bRet = sh_delmkd(szTemp, (nId == IDD_FSELDELETE) ?
					IDS_QUERYDELETE : IDS_QUERYMKDIR);
			}
			if (bRet) {
				SendOpenFileSelected(hDlg, ".", "");
			}
			return TRUE;
		}
	}

    /* Allow standard processing. */

    return FALSE;
}

/*--------------------------------------------------------------------------
 * DoSelectPerCommonDialog()
 */
static BOOL DoSelectPerCommonDialog(HWND hWnd, char szFileName[], char szExt[], BOOL bSaveAs)
{
	OPENFILENAME 	ofn;
	DWORD 		Errval;
	LPSTR		pszFilter;
	LPSTR		pszCustomFilter;
	LPSTR		pszCustomOffset;
	LPSTR		pszRun;
	BOOL			bRet;
	char 		szTemp[512];

	pszFilter = _alloc(EDMAXPATHLEN);
	if (!pszFilter) {
		return FALSE;
	}
	pszCustomFilter = _alloc(EDMAXPATHLEN);
	if (!pszCustomFilter) {
		_free(pszFilter);
		return FALSE;
	}

	pszRun = pszCustomFilter;
	lstrcpy(pszRun, szExt);
	while(*pszRun++) {
		;
	}
	pszCustomOffset = pszRun;
	lstrcpy(pszRun, szExt);
	while(*pszRun++) {
		;
	}
	*pszRun = 0;

	*pszFilter = 0;
	linealdocstrings(pszFilter, EDMAXPATHLEN);
	for (pszRun = pszFilter; *pszRun; pszRun++) {
		if (*pszRun == '|') {
			*pszRun = (char) 0;
		}
	}
	*++pszRun = (char) 0;

	blfill(&ofn, sizeof ofn, 0);

	ofn.hInstance = hInst;
	ofn.lStructSize = sizeof( OPENFILENAME );
	ofn.hwndOwner = hWnd;			// An invalid hWnd causes non-modality
	ofn.lpstrFilter = (LPSTR)pszFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrCustomFilter = (LPSTR)pszCustomFilter;
	ofn.nMaxCustFilter = EDMAXPATHLEN;
	ofn.lpstrFile = (LPSTR)szFileName;	// Stores the result in this variable
	ofn.nMaxFile = EDMAXPATHLEN - 1;
	ofn.lpstrTitle = sTitleSpec;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_HIDEREADONLY;
	// ofn.Flags = OFN_PATHMUSTEXIST | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;
	// ofn.lpTemplateName = MAKEINTRESOURCE(DLGFSEL);
	// ofn.lpfnHook = MakeProcInstance((FARPROC) FileOpenHookProc, hInst);


	if ((bSaveAs && GetSaveFileName( &ofn ) ) || (!bSaveAs && GetOpenFileName( &ofn ))) {
		bRet = TRUE;
		lstrcpy(szExt, pszCustomOffset[0] ? pszCustomOffset : "*.*");
     } else {
		bRet = FALSE;
		Errval=CommDlgExtendedError();
		if(Errval!=0) {
			wsprintf(szTemp, "GetOpenFileName returned Error # %lx", Errval);
			MessageBox(hWnd, szTemp, "WARNING", MB_OK|MB_ICONSTOP);
		}
	}

	FreeProcInstance((FARPROC)ofn.lpfnHook);
	_free(pszFilter);
	_free(pszCustomFilter);
	return bRet;
}

/*------------------------------------------------------------
 * EdFsel
 * selecting a file with a file browser
 */
int EdFsel(char *szFileSpecIn, char *szFileNameIn, char *szFullPathOut, BOOL bSaveAs)
{
 	int  	ret;
	LPSTR 	pszExt;
	LPSTR	pszFileName;
	LPSTR	pszPath;

	pszExt = _alloc(EDMAXPATHLEN);
	pszFileName = _alloc(EDMAXPATHLEN);
	pszPath = _alloc(EDMAXPATHLEN);

	// remember where we started
	sfsplit(szFileSpecIn, pszPath, pszExt);
	sfsplit(szFileNameIn, (char *)0, pszFileName);

	ChangeDirectory(pszPath);

	if ((ret = DoSelectPerCommonDialog(GetActiveWindow(), 
		pszFileName, pszExt, bSaveAs)) == TRUE) {
		lstrcpy(szFullPathOut, pszFileName);
		sfsplit(pszFileName, pszPath, szFileNameIn);
		strdcpy(szFileSpecIn, pszPath, pszExt);
		hist_enq(&_pathhist, szFileSpecIn);
	}

	fsel_optionsenable(0);
	_free(pszFileName);
	_free(pszExt);
	_free(pszPath);

	return ret;
}



