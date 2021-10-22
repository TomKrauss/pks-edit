/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * FileSelector.c
 *
 * all operations related to selecting a file.
 *
 * 						created: 07.06.91 
 *						Author : Tom
 */

#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <direct.h>
#include <string.h>
#include <ShlObj_core.h>

#include "alloc.h"
#include "trace.h"
#include "documentmodel.h"
#include "edierror.h"
#include "errordialogs.h"

#include "winfo.h"
#include "winterf.h"
#include "xdialog.h"
#include "pksrc.h"

#include "tos.h"
#include "history.h"
#include "editorconfiguration.h"

#include "documenttypes.h"
#include "edfuncs.h"
#include "pathname.h"
#include "fileselector.h"
#include "fsel.h"
#include "pksedit.h"
#include "stringutil.h"
#include "fileutil.h"
#include "edfuncs.h"

extern int 		doDocumentTypes(int nDlg);
extern void 	win_getStdMenuText(int menunr, char *text);

extern int	nCurrentDialog;

extern char *	_pksSysFolder;

char _fseltarget[EDMAXPATHLEN];

static INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) {
	char szDir[MAX_PATH];

	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		_getcwd(szDir, MAX_PATH);
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)szDir);
		break;
	}

	return 0;
}

/*
 * Select a folder using a browse for folder dialog. Return TRUE,
 * if the folder was selected. pResult will contain the resulting folder name.
 */
BOOL fsel_selectFolder(HWND hwndParent, char* pTitle, char* pResult) {
	BROWSEINFO browseinfo;
	memset(&browseinfo, 0, sizeof browseinfo);
	browseinfo.hwndOwner = hwndParent;
	browseinfo.lpszTitle = pTitle;
	browseinfo.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
	browseinfo.lpfn = BrowseCallbackProc;
	HWND hwndOld = GetFocus();
	PIDLIST_ABSOLUTE pPids = SHBrowseForFolder(&browseinfo);
	if (hwndOld) {
		SetFocus(hwndOld);
	}
	if (pPids == NULL) {
		EdTRACE(log_lastWindowsError("Select Folder"));
		return FALSE;
	}
	SHGetPathFromIDList(pPids, pResult);
	CoTaskMemFree(pPids);
	return TRUE;
}

/*--------------------------------------------------------------------------
 * fsel_changeDirectory()
 * Change the current directory. Allow for drive specification and trailing slash.
 */
void fsel_changeDirectory(char* pszPath) {
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

	_chdir(pszPath);
}

/*---------------------------------*/
/* menu_fseltitle()				*/
/*---------------------------------*/
static void menu_fseltitle(int title, char* szTemp)
{	char *d,*s;

	if (title > 0) {
		win_getStdMenuText(title,szTemp);
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
	}
}

/*---------------------------------*/
/* SelectFile()				*/
/*---------------------------------*/
static int SelectFile(int title, char *baseDirectory, char *filename, char *pattern, FILE_SELECT_PARAMS* pFSP) {
	int	nSave;
	int 	ret;
	char pathname[EDMAXPATHLEN];
	char szTemp[512];

	menu_fseltitle(title, szTemp);
	string_concatPathAndFilename(pathname,baseDirectory,pattern);
	nSave = nCurrentDialog;
	nCurrentDialog = title;

	pFSP->fsp_resultFile = _fseltarget;
	pFSP->fsp_inputFile = pathname;
	pFSP->fsp_namePatterns = pattern;
	pFSP->fsp_title = szTemp;

	ret = fsel_selectFile(pFSP);
	nCurrentDialog = nSave;
	string_splitFilename(pathname,baseDirectory,pattern);
	if (_fseltarget[0] == 0) {
		return 0;
	}

	return(ret);
}

/*---------------------------------
 * fsel_selectFileWithOptions()
 * Select a file given an info data structure and a resource ID to be used as the title.
 *---------------------------------*/
char *fsel_selectFileWithOptions(FSELINFO *fp, int idTitle, FILE_SELECT_PARAMS* pFSP)
{
	static ITEMS	_i = { C_STRING1PAR, _fseltarget };
	static PARAMS	_p = { DIM(_i), P_MAYOPEN, _i	};

	if (macro_openDialog(&_p)) {

		if (fp->path[0] == 0) {	
			lstrcpy(fp->path,_pksSysFolder);
		}

		if (fp->search[0] == 0) {
			lstrcpy(fp->search, "*.*");
		}

		if (!SelectFile(idTitle,fp->path,fp->fname,fp->search, pFSP))
			return (char *)0;
		macro_recordOperation(&_p);
	}
	return _fseltarget;
}

/*---------------------------------*/
/* fsel_initPathes()					*/
/*---------------------------------*/
char *fsel_initPathes(FSELINFO *fp)
{	char *fn;

	if ((fn = file_searchFileInPKSEditLocation(fp->fname)) != 0) {
		string_splitFilename(fn,fp->path,fp->fname);
		if (fp->path[0] == 0)
			lstrcpy(fp->path,_pksSysFolder);
		lstrcpy(_fseltarget,fn);
		fn = _fseltarget;
	}
	return fn;
}

/* 
 * Callback for the openfile common dialog. 
 */
static UINT_PTR fsel_wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static FILE_SELECT_PARAMS* pFSP;
	OPENFILENAME* pOFN;
	BOOL crypted;

	switch (message) {
	case WM_INITDIALOG:
		pOFN = (OPENFILENAME*)lParam;
		pFSP = (FILE_SELECT_PARAMS*)pOFN->lCustData;
		crypted = pFSP->fsp_encrypted;
		SendDlgItemMessage(hwnd, IDC_CHECK_ENCRYPT, BM_SETCHECK, crypted ? BST_CHECKED : BST_UNCHECKED, 0);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_CHECK_ENCRYPT && HIWORD(wParam) == BN_CLICKED) {
			pFSP->fsp_encrypted  = (BOOL)SendDlgItemMessage(hwnd, IDC_CHECK_ENCRYPT, BM_GETCHECK, 0, 0);
		}
		break;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 * DoSelectPerCommonDialog()
 */
static BOOL DoSelectPerCommonDialog(HWND hWnd, FILE_SELECT_PARAMS* pFSParams, char szFileName[], char szExt[], char* initialDirectory) {
	OPENFILENAME 	ofn;
	DWORD 		Errval;
	LPSTR		pszFilter;
	LPSTR		pszCustomFilter;
	LPSTR		pszCustomOffset;
	LPSTR		pszRun;
	BOOL		bRet;
	char 		szTemp[128];

	pszFilter = calloc(1, EDMAXPATHLEN);
	if (!pszFilter) {
		return FALSE;
	}
	pszCustomFilter = calloc(1, EDMAXPATHLEN);
	if (!pszCustomFilter) {
		free(pszFilter);
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

	doctypes_getSelectableDocumentFileTypes(pszFilter, EDMAXPATHLEN);
	for (pszRun = pszFilter; *pszRun; pszRun++) {
		if (*pszRun == '|') {
			*pszRun = (char) 0;
		}
	}
	*++pszRun = (char) 0;

	memset(&ofn, 0, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hInstance = ui_getResourceModule();
	ofn.hwndOwner = hWnd;			// An invalid hWnd causes non-modality
	ofn.lpstrFilter = (LPSTR)pszFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrInitialDir = initialDirectory;
	ofn.lpstrCustomFilter = (LPSTR)pszCustomFilter;
	ofn.nMaxCustFilter = EDMAXPATHLEN;
	ofn.lpstrFile = szFileName;	// Stores the result in this variable
	ofn.nMaxFile = EDMAXPATHLEN - 1;
	ofn.lpstrTitle = pFSParams->fsp_title;
	ofn.Flags = OFN_PATHMUSTEXIST;
	BOOL bHandleCrypt = pFSParams->fsp_saveAs && pFSParams->fsp_encryptedAvailable;
	if (bHandleCrypt) {
		ofn.Flags |= OFN_ENABLETEMPLATE|OFN_EXPLORER|OFN_ENABLESIZING|OFN_ENABLEHOOK;
		ofn.lpTemplateName = MAKEINTRESOURCE(DLG_SAVEAS_OPTIONS);
		ofn.lpfnHook = MakeProcInstance(fsel_wndProc, hInst);
		ofn.lCustData = (LPARAM) pFSParams;
	}
	if ((pFSParams->fsp_saveAs && GetSaveFileName( &ofn ) ) || (!pFSParams->fsp_saveAs && GetOpenFileName( &ofn ))) {
		bRet = TRUE;
		lstrcpy(szExt, pszCustomOffset[0] ? pszCustomOffset : "*.*");
     } else {
		bRet = FALSE;
		Errval=CommDlgExtendedError();
		if(Errval!=0) {
			wsprintf(szTemp, "GetOpenFileName returned Error # 0x%lx", Errval);
			error_displayAlertBoxWithOptions(0, MB_OK | MB_ICONSTOP, szTemp);
		}
	}
	if (ofn.lpfnHook) {
		FreeProcInstance((FARPROC)ofn.lpfnHook);
	}
	free(pszFilter);
	free(pszCustomFilter);
	return bRet;
}

/*------------------------------------------------------------
 * fsel_selectFile
 * select a file with a file open dialog. If bSaveAs is true, the
 * dialog is opened for the purpose of saving files.
 */
int fsel_selectFile(FILE_SELECT_PARAMS* pFSParams) {
	char* szFileSpecIn = pFSParams->fsp_namePatterns;
	char* szFileNameIn = pFSParams->fsp_inputFile;
	char* szFullPathOut = pFSParams->fsp_resultFile;
	int  	ret;
	LPSTR 	pszExt;
	LPSTR	pszFileName;
	LPSTR	pszPath;

	pszExt = malloc(EDMAXPATHLEN);
	pszFileName = malloc(EDMAXPATHLEN);
	pszPath = malloc(EDMAXPATHLEN);

	// remember where we started
	string_splitFilename(szFileSpecIn, pszPath, pszExt);
	string_splitFilename(szFileNameIn, (char *)0, pszFileName);

	//fsel_changeDirectory(pszPath);

	if ((ret = DoSelectPerCommonDialog(GetActiveWindow(),
		pFSParams, pszFileName, pszExt, pszPath)) == TRUE) {
		lstrcpy(szFullPathOut, pszFileName);
		string_splitFilename(pszFileName, pszPath, szFileNameIn);
		string_concatPathAndFilename(szFileSpecIn, pszPath, pszExt);
		hist_saveString(PATHES, szFileSpecIn);
	}

	free(pszFileName);
	free(pszExt);
	free(pszPath);

	return ret;
}



