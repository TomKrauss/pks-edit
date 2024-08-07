/*
 * Project: PKS Edit for Windows
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

/*
 * Show an open file selector with a title and a pointer to hold the selected result.
 * If a file was selected successfully 1 is returned.
 */
extern int file_open_vista_version(HWND hwnd, FILE_SELECT_PARAMS* pParams);

char _fseltarget[EDMAXPATHLEN];
static char* _currentDirectory;

static INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) {
	switch (uMsg) {
	case BFFM_INITIALIZED:
		if (_currentDirectory != NULL && *_currentDirectory) {
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)_currentDirectory);
		}
		break;
	}

	return 0;
}

/*
 * Select a folder using a browse for folder dialog. Return TRUE,
 * if the folder was selected. pResult will contain the resulting folder name.
 */
BOOL fsel_selectFolder(HWND hwndParent, char* pTitle, char* pResult) {
	BROWSEINFO browseinfo = {
		.hwndOwner = hwndParent,
		.lpszTitle = pTitle,
		.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON,
		.lpfn = BrowseCallbackProc
	};
	_currentDirectory = pResult;
	HWND hwndOld = GetFocus();
	PIDLIST_ABSOLUTE pPids = SHBrowseForFolder(&browseinfo);
	if (hwndOld) {
		SetFocus(hwndOld);
	}
	if (pPids == NULL) {
		EdTRACE(log_lastWindowsError("Select Folder"));
		_currentDirectory = NULL;
		return FALSE;
	}
	SHGetPathFromIDList(pPids, pResult);
	CoTaskMemFree(pPids);
	_currentDirectory = NULL;
	return TRUE;
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
	char* pathname;
	char* szTemp;

	pathname = calloc(1, EDMAXPATHLEN);
	szTemp = calloc(1, EDMAXDIRLEN);
	if (szTemp == NULL) {
		free(pathname);
		return 0;
	}
	menu_fseltitle(nCommand, szTemp);
	string_concatPathAndFilename(pathname,baseDirectory,filename);
	nSave = nCurrentDialog;
	nCurrentDialog = nCommand;

	pFSP->fsp_resultFile = _fseltarget;
	pFSP->fsp_inputFile = pathname;
	pFSP->fsp_namePatterns = pattern;
	pFSP->fsp_title = szTemp;

	ret = fsel_selectFile(pFSP);
	free(pathname);
	free(szTemp);

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
	static RECORDED_FORM_ITEM_DESCRIPTOR _i[] = {C_PUSH_STRING_LITERAL, _fseltarget};
	static RECORDED_FORM_DESCRIPTOR	_p = { DIM(_i), P_MAYOPEN, _i	};

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
		string_splitFilename(fn,fp->path,fp->fname, sizeof fp->fname);
		if (fp->path[0] == 0)
			lstrcpy(fp->path,_pksSysFolder);
		lstrcpy(_fseltarget,fn);
		fn = _fseltarget;
	}
	return fn;
}

/*------------------------------------------------------------
 * fsel_selectFile
 * select a file with a file open dialog. If bSaveAs is true, the
 * dialog is opened for the purpose of saving files.
 */
int fsel_selectFile(FILE_SELECT_PARAMS* pFSParams) {
	char* szFileNameIn = pFSParams->fsp_inputFile;
	int  	ret;
	LPSTR 	pszExt;
	LPSTR	pszFileName;
	LPSTR	pszPath;
	int		nMax = 4096;

	pFSParams->fsp_filters = calloc(1, nMax);
	if (!pFSParams->fsp_filters) {
		return FALSE;
	}
	doctypes_getSelectableDocumentFileTypes(pFSParams->fsp_filters, nMax);

	pszExt = malloc(EDMAXPATHLEN);
	if (pszExt == NULL) {
		return FALSE;
	}
	pszFileName = malloc(EDMAXPATHLEN);
	pszPath = malloc(EDMAXPATHLEN);
	if (pszPath == NULL) {
		free(pszExt);
		free(pszFileName);
		return FALSE;
	}

	// remember where we started
	string_splitFilename(szFileNameIn, pszPath, pszFileName, EDMAXPATHLEN);
	strcpy(pszExt, pFSParams->fsp_namePatterns);
	//fsel_changeDirectory(pszPath);

	ret = file_open_vista_version(hwndMain, pFSParams);

	free(pFSParams->fsp_filters);
	free(pszFileName);
	free(pszExt);
	free(pszPath);

	return ret;
}



