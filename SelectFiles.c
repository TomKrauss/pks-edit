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
#include "edifsel.h"
#include "fsel.h"
#include "pksedit.h"
#include "stringutil.h"
#include "fileutil.h"

extern int 		DoDocumentTypes(int nDlg);
extern void 	GetStdMenuText(int menunr, char *text);
/*------------------------------------------------------------
 * EdGetActiveWindow()
 */
extern HWND EdGetActiveWindow(int includeicons);

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

	_chdir(pszPath);
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
/* SelectFile()				*/
/*---------------------------------*/
static int SelectFile(int title, char *baseDirectory, char *filename, char *pattern, BOOL showSavedialog)
{
	int	nSave;
	int 	ret;
	char pathname[512];

	menu_fseltitle(title);
	strdcpy(pathname,baseDirectory,pattern);
	nSave = nCurrentDialog;
	nCurrentDialog = title;
	ret = EdFsel(pathname, filename, _fseltarget, showSavedialog);
	nCurrentDialog = nSave;
	sfsplit(pathname,baseDirectory,pattern);
	if (_fseltarget[0] == 0) {
		return 0;
	}

	return(ret);
}

/*---------------------------------*/
/* rw_select()					*/
/*---------------------------------*/
char *rw_select(FSELINFO *fp, int title, BOOL showSaveDialog)
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

		if (!SelectFile(title,fp->path,fp->fname,fp->search, showSaveDialog))
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

	if ((fn = searchfile(fp->fname)) != 0) {
		sfsplit(fn,fp->path,fp->fname);
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

	//ChangeDirectory(pszPath);

	if ((ret = DoSelectPerCommonDialog(GetActiveWindow(), 
		pszFileName, pszExt, pszPath, bSaveAs)) == TRUE) {
		lstrcpy(szFullPathOut, pszFileName);
		sfsplit(pszFileName, pszPath, szFileNameIn);
		strdcpy(szFileSpecIn, pszPath, pszExt);
		hist_enq(PATHES, szFileSpecIn);
	}

	fsel_optionsenable(0);
	_free(pszFileName);
	_free(pszExt);
	_free(pszPath);

	return ret;
}



