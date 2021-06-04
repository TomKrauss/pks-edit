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
#include <windowsx.h>
#include <commdlg.h>
#include <direct.h>
#include <string.h>

#include "alloc.h"
#include "trace.h"
#include "lineoperations.h"
#include "edierror.h"

#include "winfo.h"
#include "winterf.h"

#include "tos.h"
#include "edhist.h"
#include "editorconfiguration.h"

#include "documenttypes.h"
#include "edfuncs.h"
#include "pathname.h"
#include "fileselector.h"
#include "fsel.h"
#include "pksedit.h"
#include "stringutil.h"
#include "fileutil.h"

extern int 		doDocumentTypes(int nDlg);
extern void 	GetStdMenuText(int menunr, char *text);
/*------------------------------------------------------------
 * EdGetActiveWindow()
 */
extern HWND EdGetActiveWindow(int includeicons);

extern int	nCurrentDialog;

extern char *	_datadir;

static char *	sTitleSpec;

char _fseltarget[512];

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

/*------------------------------------------------------------
 * fsel_setDialogTitle
 * setting the idTitle for the next call of fsel_selectFile
 */
void fsel_setDialogTitle(char *title) {
	if (sTitleSpec)
		_free(sTitleSpec);
	sTitleSpec = string_allocate(title);
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
		fsel_setDialogTitle(szTemp);
	}
}

/*---------------------------------*/
/* SelectFile()				*/
/*---------------------------------*/
static int SelectFile(int title, char *baseDirectory, char *filename, char *pattern, BOOL showSavedialog)
{
	int	nSave;
	int 	ret;
	char pathname[512];

	menu_fseltitle(title);
	string_concatPathAndFilename(pathname,baseDirectory,pattern);
	nSave = nCurrentDialog;
	nCurrentDialog = title;
	ret = fsel_selectFile(pathname, filename, _fseltarget, showSavedialog);
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
char *fsel_selectFileWithOptions(FSELINFO *fp, int idTitle, BOOL showSaveDialog)
{
	static ITEMS	_i = { C_STRING1PAR, _fseltarget };
	static PARAMS	_p = { DIM(_i), P_MAYOPEN, _i	};

	if (macro_openDialog(&_p)) {

		if (fp->path[0] == 0) {	
			lstrcpy(fp->path,_datadir);
		}

		if (fp->search[0] == 0) {
			lstrcpy(fp->search, "*.*");
		}

		if (!SelectFile(idTitle,fp->path,fp->fname,fp->search, showSaveDialog))
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
			lstrcpy(fp->path,_datadir);
		lstrcpy(_fseltarget,fn);
		fn = _fseltarget;
	}
	return fn;
}


/*--------------------------------------------------------------------------
 * DoSelectPerCommonDialog()
 */
static BOOL DoSelectPerCommonDialog(HWND hWnd, char szFileName[], char szExt[], char* initialDirectory, BOOL bSaveAs)
{
	OPENFILENAME 	ofn;
	DWORD 		Errval;
	LPSTR		pszFilter;
	LPSTR		pszCustomFilter;
	LPSTR		pszCustomOffset;
	LPSTR		pszRun;
	BOOL		bRet;
	char 		szTemp[128];

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
	GetSelectableDocumentFileTypes(pszFilter, EDMAXPATHLEN);
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
	ofn.lpstrInitialDir = initialDirectory;
	ofn.lpstrCustomFilter = (LPSTR)pszCustomFilter;
	ofn.nMaxCustFilter = EDMAXPATHLEN;
	ofn.lpstrFile = szFileName;	// Stores the result in this variable
	ofn.nMaxFile = EDMAXPATHLEN - 1;
	ofn.lpstrTitle = sTitleSpec;
	ofn.Flags = OFN_PATHMUSTEXIST;

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

	if (ofn.lpfnHook) {
		FreeProcInstance((FARPROC)ofn.lpfnHook);
	}
	_free(pszFilter);
	_free(pszCustomFilter);
	return bRet;
}

/*------------------------------------------------------------
 * fsel_selectFile
 * select a file with a file open dialog. If bSaveAs is true, the
 * dialog is opened for the purpose of saving files.
 */
int fsel_selectFile(char *szFileSpecIn, char *szFileNameIn, char *szFullPathOut, BOOL bSaveAs)
{
 	int  	ret;
	LPSTR 	pszExt;
	LPSTR	pszFileName;
	LPSTR	pszPath;

	pszExt = _alloc(EDMAXPATHLEN);
	pszFileName = _alloc(EDMAXPATHLEN);
	pszPath = _alloc(EDMAXPATHLEN);

	// remember where we started
	string_splitFilename(szFileSpecIn, pszPath, pszExt);
	string_splitFilename(szFileNameIn, (char *)0, pszFileName);

	//fsel_changeDirectory(pszPath);

	if ((ret = DoSelectPerCommonDialog(GetActiveWindow(), 
		pszFileName, pszExt, pszPath, bSaveAs)) == TRUE) {
		lstrcpy(szFullPathOut, pszFileName);
		string_splitFilename(pszFileName, pszPath, szFileNameIn);
		string_concatPathAndFilename(szFileSpecIn, pszPath, pszExt);
		hist_enq(PATHES, szFileSpecIn);
	}

	_free(pszFileName);
	_free(pszExt);
	_free(pszPath);

	return ret;
}



