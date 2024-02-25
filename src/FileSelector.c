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
#include "pksmacro.h"
#include "pksmacrocvm.h"
#include "pathname.h"
#include "fileselector.h"
#include "fsel.h"
#include "pksedit.h"
#include "stringutil.h"
#include "fileutil.h"
#include "pksmacro.h"

extern int 		doDocumentTypes(int nDlg);

extern int	nCurrentDialog;

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

static DWORD _codepages[] = {
	-1,
	CP_ACP,
	CP_OEMCP,
	CP_UTF8,
	37,
	437,
	500,
	708,
	709,
	710,
	720,
	737,
	775,
	850,
	852,
	855,
	857,
	858,
	860,
	861,
	862,
	863,
	864,
	865,
	866,
	869,
	870,
	874,
	875,
	932,
	936,
	949,
	950,
	1026,
	1047,
	1140,
	1141,
	1142,
	1143,
	1144,
	1145,
	1146,
	1147,
	1148,
	1149,
	1200,
	1201,
	1252,
	1253,
	1254,
	1255,
	1256,
	1257,
	1258,
	1361,
	10000,
	10001,
	10002,
	10003,
	10004,
	10005,
	10006,
	10007,
	10008,
	12000,
	12001,
	65000,
	65001
};

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
static void menu_fseltitle(int nCommand, char* szTemp)
{	char *d,*s;

	if (nCommand >= 0) {
		char szTooltip[256];
		command_getTooltipAndLabel((MACROREF) {.typ = CMD_CMDSEQ, .index = nCommand}, szTooltip, szTemp);
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
static int SelectFile(int nCommand, char *baseDirectory, char *filename, char *pattern, FILE_SELECT_PARAMS* pFSP) {
	int	nSave;
	int 	ret;
	char pathname[EDMAXPATHLEN];
	char szTemp[512];

	menu_fseltitle(nCommand, szTemp);
	string_concatPathAndFilename(pathname,baseDirectory,filename);
	nSave = nCurrentDialog;
	nCurrentDialog = nCommand;

	pFSP->fsp_resultFile = _fseltarget;
	pFSP->fsp_inputFile = pathname;
	pFSP->fsp_namePatterns = pattern;
	pFSP->fsp_title = szTemp;

	ret = fsel_selectFile(pFSP);
	nCurrentDialog = nSave;
	if (_fseltarget[0] == 0) {
		return 0;
	}

	return(ret);
}

/*---------------------------------
 * fsel_selectFileWithOptions()
 * Select a file given an info data structure and a resource ID to be used as the nCommand.
 *---------------------------------*/
char *fsel_selectFileWithOptions(FSELINFO *fp, int nCommand, FILE_SELECT_PARAMS* pFSP)
{
	static ITEMS	_i = { C_PUSH_STRING_LITERAL, _fseltarget };
	static PARAMS	_p = { DIM(_i), P_MAYOPEN, _i	};

	if (interpreter_openDialog(&_p)) {

		if (fp->path[0] == 0) {	
			lstrcpy(fp->path, ".");
		}

		if (fp->search[0] == 0) {
			lstrcpy(fp->search, "*.*");
		}

		if (!SelectFile(nCommand,fp->path,fp->fname,fp->search, pFSP))
			return (char *)0;
		recorder_recordOperation(&_p);
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
 * Fill the encoding list (list of possible character encodings). 
 */
static void fsel_fillEncodingList(HWND hwnd, BOOL bAllowAuto, long nSelectedEncoding) {
	SendMessage(hwnd, CB_RESETCONTENT, 0, 0L);
	SendMessage(hwnd, WM_SETREDRAW, FALSE, 0L);
	WPARAM nSelectedIndex = 0;
	for (int i = 0; i < DIM(_codepages); i++) {
		CPINFOEX cpInfo;
		DWORD nCp = _codepages[i];
		char* pszText;
		if (nCp == -1) {
			if (!bAllowAuto) {
				continue;
			}
			pszText = "Auto-detect";
		} else {
			if (!GetCPInfoEx(nCp, 0, &cpInfo)) {
				continue;
			}
			pszText = cpInfo.CodePageName;
		}
		LRESULT nOffset = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)pszText);
		if (nOffset == LB_ERR) {
			continue;
		}
		if (nSelectedEncoding == nCp) {
			nSelectedIndex = (WPARAM)nOffset;
		}
		SendMessage(hwnd, CB_SETITEMDATA, nOffset, nCp);
	}
	SendMessage(hwnd, WM_SETREDRAW, (WPARAM)TRUE, 0L);
	SendMessage(hwnd, CB_SETCURSEL, nSelectedIndex, (LPARAM)0);
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
		fsel_fillEncodingList(GetDlgItem(hwnd, IDC_FILE_ENCONDING), !pFSP->fsp_saveAs, pFSP->fsp_codepage);
		hwndDlg = hwnd;
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_FILE_ENCONDING && HIWORD(wParam) == CBN_SELCHANGE) {
			LRESULT nIndex = SendDlgItemMessage(hwnd, IDC_FILE_ENCONDING, CB_GETCURSEL, 0l, 0L);
			if (nIndex >= 0) {
				pFSP->fsp_codepage = (long)SendDlgItemMessage(hwnd, IDC_FILE_ENCONDING, CB_GETITEMDATA, nIndex, 0L);
			}
		}
		if (LOWORD(wParam) == IDC_CHECK_ENCRYPT && HIWORD(wParam) == BN_CLICKED) {
			pFSP->fsp_encrypted  = (BOOL)SendDlgItemMessage(hwnd, IDC_CHECK_ENCRYPT, BM_GETCHECK, 0, 0);
		}
		break;
	case WM_DESTROY:
		hwndDlg = NULL;
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
	int			nMax = 4096;

	pszFilter = calloc(1, nMax);
	if (!pszFilter) {
		return FALSE;
	}
	pszCustomFilter = calloc(1, nMax);
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

	doctypes_getSelectableDocumentFileTypes(pszFilter, nMax);
	int nFilterIndex = 0;
	int nIndex = 0;
	char* pszPrev = pszFilter;
	for (pszRun = pszFilter; *pszRun; pszRun++) {
		if (*pszRun == '|') {
			*pszRun = (char) 0;
			if (strstr(pszPrev, szExt) != 0) {
				nFilterIndex = 1 + (nIndex / 2);
			}
			nIndex++;
			pszPrev = pszRun + 1;
		}
	}
	*++pszRun = (char) 0;

	memset(&ofn, 0, sizeof ofn);
	ofn.lStructSize = sizeof ofn;
	ofn.hInstance = ui_getResourceModule();
	ofn.hwndOwner = hWnd;			// An invalid hWnd causes non-modality
	ofn.lpstrFilter = (LPSTR)pszFilter;
	ofn.nFilterIndex = nFilterIndex;
	ofn.lpstrInitialDir = initialDirectory;
	ofn.lpstrCustomFilter = (LPSTR)pszCustomFilter;
	ofn.nMaxCustFilter = EDMAXPATHLEN;
	ofn.lpstrFile = szFileName;	// Stores the result in this variable
	ofn.nMaxFile = EDMAXPATHLEN - 1;
	ofn.lpstrTitle = pFSParams->fsp_title;
	ofn.Flags = OFN_PATHMUSTEXIST;
	if (pFSParams->fsp_multiSelect) {
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}
	if (pFSParams->fsp_optionsAvailable) {
		ofn.Flags |= OFN_ENABLETEMPLATE|OFN_EXPLORER|OFN_ENABLESIZING|OFN_ENABLEHOOK;
		ofn.lpTemplateName = MAKEINTRESOURCE(pFSParams->fsp_saveAs ? DLG_SAVEAS_OPTIONS : DLG_OPEN_OPTIONS);
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
			// TODO: I18N
			wsprintf(szTemp, "GetOpenFileName returned Error # 0x%lx", Errval);
			error_displayAlertBoxWithOptions(MB_OK | MB_ICONSTOP, szTemp);
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
	string_splitFilename(szFileNameIn, pszPath, pszFileName);
	strcpy(pszExt, pFSParams->fsp_namePatterns);
	//fsel_changeDirectory(pszPath);

	if ((ret = DoSelectPerCommonDialog(GetActiveWindow(),
		pFSParams, pszFileName, pszExt, pszPath)) == TRUE) {
		memcpy(szFullPathOut, pszFileName, EDMAXPATHLEN);
		if (pszFileName[strlen(pszFileName) + 1]) {
			// special case: multi-result returned by GetFilename Filedialog API.
			strcpy(pszPath, pszFileName);
		} else {
			string_splitFilename(pszFileName, pszPath, szFileNameIn);
		}
	}

	free(pszFileName);
	free(pszExt);
	free(pszPath);

	return ret;
}



