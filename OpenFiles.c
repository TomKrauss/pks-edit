/*
 * PROJEKT: PKS-EDIT for WINDOWS
 *
 * OpenFiles.c
 *
 * open files
 *
 * 						created: 07.06.91
 *						Author : TOM
 */

#include "tos.h"
#include <shellapi.h>
#include <time.h>
#include "alloc.h"

#include "trace.h"
#include "linkedlist.h"
#include "caretmovement.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"

#include "winfo.h"
#include "winterf.h"
#include "actions.h"
#include "grammar.h"

#include "dos.h"
#include "pathname.h"
#include "fileselector.h"
#include "history.h"
#include "pksedit.h"
#include "stringutil.h"
#include "fileutil.h"
#include "winfo.h"
#include "documenttypes.h"
#include "editorconfiguration.h"
#include "regexp.h"
#include "edfuncs.h"
#include "crossreferencelinks.h"
#include "propertychange.h"
#include "arraylist.h"
#include "mainframe.h"

/*
 * Files bigger than this are not completely re-read on changes, but an attempt is performed
 * to read only the new file contents appended to that file. Smaller files are always completely
 * re-read as the probability, that smaller files are re-created is bigger and the cost for re-reading
 * them completely is acceptable.
 */
#define APPEND_THRESHOLD_SIZE			100000

extern WINFO* ww_findwinid(int winid);
extern int 	EdPromptAutosavePath(char *path);

extern char *	_pksSysFolder;
extern void *	lastSelectedDocType;

static	FTABLE 	*_currentFile;
static FTABLE 	*_filelist;

#define HISTORY_FILE_NAME "pksedit.his"
static char		_historyFileName[EDMAXPATHLEN];
int				_ExSave;

/*--------------------------------------------------------------------------
 * ft_generateAutosavePathname()
 */
static int ft_generateAutosavePathname(char *destinationName, char *fname)
{	char fn[EDMAXPATHLEN];
	char szBuff[EDMAXPATHLEN];

	string_splitFilename(fname,(char *)0,fn);
	string_concatPathAndFilename(szBuff, config_getPKSEditTempPath(),fn);
	string_getFullPathName(destinationName,szBuff);
	return 1;
}

/*
 * Returns the current active document. Should not be used any more to often 
 */
FTABLE* ft_getCurrentDocument() {
	return _currentFile;
}

/*
 * Returns the current error document. Should not be used any more to often
 */
FTABLE* ft_getCurrentErrorDocument() {
	for (FTABLE* fp = _filelist; fp; fp = fp->next) {
		if (fp->navigationPattern) {
			return fp;
		}
	}
	return 0;
}

/*
 * Append the external changes of a file to the passed file in
 * tail -f mode.
 */
static int ft_appendFileChanges(WINFO* wp) {
	int fd;
	FTABLE* fp = wp->fp;
	if ((fd = file_openFile(fp->fname)) < 0) {
		return 0;
	}
	FTABLE ftAppend;
	FILE_READ_OPTIONS fro;
	memset(&ftAppend, 0, sizeof ftAppend);
	memset(&fro, 0, sizeof fro);
	fro.fro_fileName = fp->fname;
	fro.fro_fileReadOffset = fp->fileSize;
	fro.fro_useDefaultDocDescriptor = 1;
	if (ft_readfileWithOptions(&ftAppend, &fro) == 0) {
		return 0;
	}
	fp->fileSize = ftAppend.fileSize;
	LINE* lpd = fp->lastl;
	int oldFlags = fp->flags;
	int ret = ln_pasteLines(wp, ftAppend.firstl, ftAppend.lastl, lpd, 0, 0);
	int nLines = ln_cnt(ftAppend.firstl, ftAppend.lastl)-1;
	LINE* lpPrevious = ln_relative(fp->lastl, -nLines);
	if (lpPrevious) {
		ln_removeFlag(lpPrevious, fp->lastl, LNMODIFIED);
	}
	ln_listfree(ftAppend.firstl);
	ft_setFlags(fp, oldFlags);
	render_repaintAllForFile(fp);
	file_closeFile(&fd);
	return 1;
}

/*
 * Checks whether two file names describe two different files. Returns
 * value != 0, if different
 */
static BOOL areFilenamesDifferent(char* fileName1, char* fileName2) {
	char tempFn1[EDMAXPATHLEN];
	char tempFn2[EDMAXPATHLEN];

	if (strcmp(fileName1, fileName2) == 0) {
		return FALSE;
	}
	GetLongPathName(fileName1, tempFn1, sizeof tempFn1);
	GetLongPathName(fileName2, tempFn2, sizeof tempFn2);
	return lstrcmpi(tempFn1, tempFn2);
}

void ft_checkForChangedFiles(BOOL bActive) {
	FTABLE *	fp;
	EDTIME		lCurrentTime;
	BOOL bAbandoned = FALSE;

	for (fp = _filelist; fp && !bAbandoned; fp = fp->next) {
		if (!file_getAccessTime(fp->fname, 0, &lCurrentTime)) {
			// File was deleted on disk - for now ignore. There are valid cases for this anyways: temporary macro files etc....
			continue;
		}
		if (fp->ti_modified != lCurrentTime) {
			WINFO* wp = WIPOI(fp);
			if (wp) {
				int maxcol = wp->maxcol;
				BOOL bLogMode = fp->flags & F_WATCH_LOGFILE;
				if (bLogMode) {
					if (fp->fileSize < APPEND_THRESHOLD_SIZE || !ft_appendFileChanges(wp)) {
						ft_abandonFile(fp, (EDIT_CONFIGURATION*)0);
						fp->flags |= F_WATCH_LOGFILE;
						// may make the list of files invalid.
						bAbandoned = TRUE;
					}
					// This is a hack to avoid expensive recalculation of maximum line len for big files
					// when files change frequently - e.g. the PKS Edit search list.
					if (fp->nlines > 1000) {
						wp->maxcol = maxcol;
					}
					caret_placeCursorInCurrentFile(wp, fp->nlines - 1, 0);
				} else {
					if (!bActive) {
						continue;
					}
					EdSelectWindow(wp->win_id);
					if (error_displayYesNoConfirmation(IDS_MSGFILESHAVECHANGED, string_abbreviateFileNameOem(fp->fname)) == IDYES) {
						ft_abandonFile(fp, (EDIT_CONFIGURATION*)0);
						// may make the list of files invalid.
						bAbandoned = TRUE;
					}
				}
			}
			fp->ti_modified = lCurrentTime;
		}
	}
}

/*
 * Fire a property change event. 
 */
static void ft_firePropertyChange(FTABLE* fp, PROPERTY_CHANGE* pChange) {
	ft_forAllViews(fp, ww_documentPropertyChanged, pChange);
}

/*---------------------------------*/
/* ft_triggerAutosaveAllFiles()					*/
/* do an autosave				*/
/*---------------------------------*/
#define 	HZ			1000L
int ft_triggerAutosaveAllFiles(void)
{	int	    		ret,flags,saved;
	long     		now,dclicks;
	register FTABLE *fp;
	char     		spath[EDMAXPATHLEN];
	static long 	nchkclick;
	static int 		inAutoSave;

	if (inAutoSave || (dclicks = (GetConfiguration()->autosaveSeconds * HZ)) == 0) {
		/* autosave option is OFF */
		return 0;
	}

	now = GetTickCount();
	if (!nchkclick) {
		nchkclick = now + 5 * HZ;
	}
	if (now < nchkclick) {
		return 0;
	}

	BOOL bAutoaveToTemp = GetConfiguration()->options & O_AUTOSAVE_TO_TEMP;
	inAutoSave = 1;

	/* check every 5 secs */
	nchkclick = now + 5 * HZ;

	saved = 0;
	for (fp = _filelist; fp; fp = fp->next) {

		/* file in unsaved state and needs to be auto-saved ? */
		flags = fp->flags;
		if ((flags & F_NEEDSAUTOSAVE) == 0 || (flags & F_TRANSIENT)) {
			continue;
		}

		/* no "autosave needed"-time: set to current + #of seconds */
		if (fp->as_time == 0) {
			fp->as_time = now + dclicks;
			continue;
		}

		/* save maximum 1 file per check cycle */
		if (saved)
			continue;

		if (now < fp->as_time)
			continue;

	autosave:
		if (!bAutoaveToTemp) {
			fp->flags &= ~F_NEEDSAUTOSAVE;
			ret = ft_writefileMode(fp, WFM_QUIET);
			fp->as_time = now + dclicks;
			saved = ret;
			continue;
		}
		/* "best before" date expired: do autosave */
		strcpy(spath,fp->fname);
		ft_generateAutosavePathname(fp->fname, fp->fname);
		ret = ft_writefileMode(fp, WFM_QUIET | WFM_AUTOSAVING);

		/* restore MODIFIED and ISBACKUPED - Flags */
		fp->flags = flags;

		if (ret > 0) {
			error_showMessageInStatusbar(IDS_MSGAUBE,ft_visibleName(fp));
		} else if (EdPromptAutosavePath(config_getPKSEditTempPath())) {
		/* let the user correct invalid autosave pathes */
			goto autosave;
		}
	
		/* restore file name */
		strcpy(fp->fname, spath);

		/*
		 * maybe autosave failed - reset autosave condition nevertheless
		 * cause autosave probably will fail on next cycle(in 5 sec.)
		 * too
		 */
		flags &= ~F_NEEDSAUTOSAVE;
		ft_setFlags(fp, flags);
		fp->as_time = now + dclicks;
		saved = ret;
	}
	
	inAutoSave = 0;
	return saved;
}

/*---------------------------------
 * ft_saveWindowStates()			
 * Save the state of the currently opened windows in the config for restore.
 ---------------------------------*/
void ft_saveWindowStates(void )
{	int    				s;
	FTABLE 				ft;
	WINFO *				wp;
	char   				szBuff[EDMAXPATHLEN];
	char*				pszFilename;

	memset(&ft, 0, sizeof ft);
	ln_createAndAddSimple(&ft, "[files]");

	int nWindows = ww_getNumberOfOpenWindows();
	WINFO* wpActive = ww_getCurrentEditorWindow();
	for (s = 1; s <= nWindows; s++) {
		if ((wp = ww_findwinid(s)) != 0) {
			FTABLE* fp = wp->fp;
			if (!(fp->flags & (F_TRANSIENT | F_NAME_INPUT_REQUIRED))) {
				int nIndex = arraylist_indexOf(fp->views, wp);
				int nDispmode = wp->dispmode;
				if (nDispmode == fp->documentDescriptor->dispmode) {
					nDispmode = -1;
				}
				xref_addSearchListEntry(&ft, fp->fname, wp->caret.ln, 
					mainframe_getOpenHint(wp->edwin_handle, wp == wpActive, nIndex > 0, nDispmode));
			}
		}
	}

	hist_saveAllEntriesTo(&ft);
	pszFilename = _historyFileName;
	if (pszFilename[0] == 0) {
		string_concatPathAndFilename(szBuff, _pksSysFolder, HISTORY_FILE_NAME);
		pszFilename = szBuff;
	}
	ft_writeFileAndClose(&ft, pszFilename, FA_NORMAL);
}

/*---------------------------------*/
/* ft_restorePreviouslyOpenedWindows() */
/*---------------------------------*/
int ft_restorePreviouslyOpenedWindows(void) {
	FTABLE 	ft;
	char *	pszFound;

	if (GetConfiguration()->options & O_AUTO_OPEN_HISTORY) {
		if ((pszFound = file_searchFileInPKSEditLocation(HISTORY_FILE_NAME)) != 0) {
			FILE_READ_OPTIONS fro;
			memset(&fro, 0, sizeof fro);
			fro.fro_fileName = pszFound;
			fro.fro_useDefaultDocDescriptor = 1;
			memset(&ft, 0, sizeof ft);
			if (ft_readfileWithOptions(&ft, &fro)) {
			// save complete filename of history file.
				GetFullPathName(pszFound, sizeof _historyFileName, _historyFileName, NULL);
				if (_filelist == 0) {
					xref_openSearchListResultFromLine(&ft, ft.firstl);
				}
				hist_readLine(ft.firstl);
				ln_listfree(ft.firstl);
				free(ft.documentDescriptor);
				return 1;
			}
		}
	}
	return 0;
}

/*------------------------------------------------------------
 * ft_settime()
 */
void ft_settime(EDTIME *tp) {	
	time (tp);
}

/*------------------------------------------------------------
 * ft_deleteautosave()
 */
void ft_deleteautosave(FTABLE *fp) {
	char as_name[EDMAXPATHLEN];

	if ((GetConfiguration()->options & O_DELETE_AUTOSAVE_FILES) && 
			ft_generateAutosavePathname(as_name,fp->fname) && 
			areFilenamesDifferent(fp->fname,as_name)) {
		_unlink(as_name);
	}
}

/*------------------------------------------------------------
 * ft_destroy()
 */
void ft_destroy(FTABLE *fp)
{
	EdTRACE(log_errorArgs(DEBUG_TRACE,"ft_destroy File 0x%lx",fp));
	ft_deleteautosave(fp);
	ft_bufdestroy(fp);
	free(fp->title);
	fp->title = 0;

	ll_delete(&_filelist,fp);

	if (!_filelist || P_EQ(fp, ft_getCurrentDocument())) {
		_currentFile = NULL;
	}
}

/*------------------------------------------------------------
 * ft_new()
 */
FTABLE *ft_new(void)
{	FTABLE *fp;

	if ((fp = ll_insert(&_filelist,sizeof *fp)) == 0)
		return 0;

	if (undo_initializeManager(fp) == 0) {
		ll_delete(&_filelist,fp);
		return 0;
	}
	EdTRACE(log_errorArgs(DEBUG_TRACE,"ft_new File 0x%lx",fp));

	return fp;
}

/*------------------------------------------------------------
 * ft_visibleName()
 */
char *ft_visibleName(FTABLE *fp) {	
	static char buf[512];

	if (fp)
		OemToAnsi(fp->fname,buf);
	return buf;
}

/*------------------------------------------------------------
 * ft_size()
 * calculate current file size
 */
long ft_size(FTABLE *fp)
{	
	long fsize = ln_calculateMemorySizeRequired(fp->firstl,fp->documentDescriptor->nl,fp->documentDescriptor->cr);

	if (fp->lastl) 
		fsize -= fp->lastl->len;
	return fsize;
}

/*---------------------------------
 * ww_hasSelection()
 * Check whether a text selection exists.
 *---------------------------------*/
int ww_hasSelection(WINFO* wp) {
	if (wp == 0 || wp->blstart == 0L || wp->blend == 0L)
		return 0;
	return 1;
}

/*
 * Can be used to test, whether a selection contains a caret position given in
 * screen line and column positions.
 */
int bl_selectionContainsLogicalPoint(WINFO* wp, long ln, long col) {
	long ln1;
	long ln2;

	if (!ww_getSelectionLines(wp, &ln1, &ln2)) {
		return 0;
	}
	if (ln1 > ln || ln2 < ln) {
		return 0;
	}
	if (ww_isColumnSelectionMode(wp)) {
		return (col >= wp->blcol1 && col <= wp->blcol2);
	}
	if (ln == ln1) {
		return col >= wp->blstart->m_column;
	}
	if (ln == ln2) {
		return col <= wp->blend->m_column;
	}
	return 1;
}
/*---------------------------------
 * ww_checkSelectionWithError()
 * Check whether a text selection exists. If not
 * report an error to the user.
 *---------------------------------*/
EXPORT int ww_checkSelectionWithError(WINFO* wp) {
	if (ww_hasSelection(wp) == 0) {
		error_showErrorById(IDS_MSGNOBLOCKSELECTED);
		return 0;
	}
	return 1;
}

/*------------------------------------------------------------
 * ft_fpbyname()
 * Find a filebuffer given the name of the file.
 */
FTABLE *ft_fpbyname(char *fn)
{	FTABLE *fp;

	for (fp = _filelist; fp; fp = fp->next) {
		if (!areFilenamesDifferent(fp->fname,fn)) {
		    return fp;
		}
	}
	return 0;
}

/*------------------------------------------------------------
 * ft_editing()
 * Answer true, if we are editing the file named filename.
 */
int ft_editing(char *fn)
{
	return ft_fpbyname(fn) ? 1 : 0;
}

/*------------------------------------------------------------
 * ft_globalediting()
 */
#if !defined(WIN32)
static DWORD ft_globalediting(char *fn)
{
	return SendBrotherMessage(WMBRD_FINDFILE,(LONG)fn);
}
#endif
/*------------------------------------------------------------
 * ft_openwin()
 */
static int ft_openwin(FTABLE *fp, const char* pszHint) {
	HWND hwndNew;
	size_t nOldWindows = fp->views == 0 ? 0 : arraylist_size(fp->views);
	if ((hwndNew = ww_createEditWindow(fp->fname, (LPVOID)(uintptr_t)fp, pszHint)) == 0) {
		return 0;
	}
	if (nOldWindows == 1) {
		WINFO* wp = ww_getWinfoForHwnd(hwndNew);
		if (wp && wp->renderer->r_supportsMode(SHOWWYSIWYG)) {
			ww_changeDisplayMode(wp, wp->dispmode | SHOWWYSIWYG);
		}
	}
	return 1;
}


/*
 * Open a second window for the current open file. 
 */
int ft_cloneWindow() {
	FTABLE* fp = ft_getCurrentDocument();
	if (fp == NULL) {
		return 0;
	}
	return ft_openwin(fp, DOCK_NAME_RIGHT);
}

/*
 * Clone a window given the file name of the window. 
 */
int ft_cloneWindowNamed(char* pszFilename, const char* pszDock) {
	FTABLE* fp = ft_fpbyname(pszFilename);
	if (fp == NULL) {
		return 0;
	}
	return ft_openwin(fp, pszDock);
}

/*------------------------------------------------------------
 * ww_requestToClose()
 * The user requests to close a file (last window of a file). 
 * If the file is modified and cannot be saved or some other error
 * occurs, return 0, otherwise, if the file can be closed return 1.
 */
int ww_requestToClose(WINFO *wp)
{
	FTABLE* fp = wp->fp;
	if (fp->documentDescriptor->closingMacroName[0]) {
		if (!macro_executeByName(fp->documentDescriptor->closingMacroName)) {
			return 0;
		}
	}
	if (ft_isFileModified(fp)) {
		ShowWindow(hwndMain,SW_SHOW);
		EdSelectWindow(wp->win_id);
		if (_ExSave || (GetConfiguration()->options & O_AUTOSAVE_FILES_ON_EXIT)) {
	     	return ft_writeFileWithAlternateName(fp);
		}
		switch(error_displayYesNoCancelConfirmation(IDS_MSGQUITORSAVE,ft_visibleName(fp))) {
			case IDYES:
				if (!ft_writeFileWithAlternateName(fp)) {
					return 0;
				}
				break;
			case IDNO: ft_setFlags(fp, fp->flags & ~F_MODIFIED);  break;
			case IDCANCEL:  return (0);
		}
	}
	hist_saveString(OPEN_FILES, fp->fname);
	hist_updateMenu(OPEN_FILES);
	return 1;
}

/*------------------------------------------------------------
 * ft_selectWindowWithId()
 * Select and activate the window with the given window id.
 */
int ft_selectWindowWithId(int winid, BOOL bPopup) {
	WINFO *	wp;

	wp = ww_findwinid(winid);
	if (wp == 0) {
		error_showErrorById(IDS_MSGWRONGWINDOW);
		return 0;
	}

	SendMessage(hwndMain,WM_MDIACTIVATE,(WPARAM)wp->edwin_handle,(LPARAM)0L);
	SetFocus(wp->edwin_handle);
	return 1;
}

/*------------------------------------------------------------
 * EdSelectWindow()
 */
int EdSelectWindow(int winid)
{
	return ft_selectWindowWithId(winid, TRUE);
}

/*------------------------------------------------------------
 * ft_select()
 * Make the passed filebuffer the "current" edited file in PKS Edit.
 */
int ft_currentFileChanged(FTABLE *fp) {
	if (fp == _currentFile) {
		return 1;
	}
	EdTRACE(log_errorArgs(DEBUG_TRACE,"ft_select File 0x%lx",fp));
	_currentFile = fp;
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ALL);
	macro_selectDefaultBindings();
	if (fp == 0) {
		return 0;
	}
	grammar_documentTypeChanged(fp->documentDescriptor->grammar);
	return 1;
}

/*------------------------------------------------------------
 * fsel_selectFileWithTitle()
 * Select a file given a resource id for the title and return the selected
 * filename in "result" (must be large enough to hold the pathname (EDMAXPATHLEN)).
 */
static FSELINFO _txtfninfo = {"."};
int fsel_selectFileWithTitle(int title, char *result, FILE_SELECT_PARAMS* pFSP)
{
	char *fn;
	BOOL	bRet;

	bRet = FALSE;
	if ((fn = fsel_selectFileWithOptions(&_txtfninfo, title, pFSP)) != 0) {
		strcpy(result,fn);
		bRet = TRUE;
	}
	return (int) bRet;
}


/*------------------------------------------------------------
 * ft_activateWindowOfFileNamed()
 * Activate the window of the file named 'fn'.
 */
int ft_activateWindowOfFileNamed(char *fn) {
	FTABLE 	*fp;
	char 	fullname[EDMAXPATHLEN];

	string_getFullPathName(fullname,fn);
	fp = ft_fpbyname(fullname);
	if (fp == NULL) {
		return 0;
	}
	WINFO* wp = WIPOI(fp);
	if (wp == NULL) {
		return 0;
	}
	return EdSelectWindow(wp->win_id);
}

/*------------------------------------------------------------
 * ft_openFileWithoutFileselector()
 * Open a file with a file name and jump into a line. Place the window to
 * open as defined in the param wsp.
 */
FTABLE* ft_openFileWithoutFileselector(char *fn, long line, FT_OPEN_OPTIONS* pOptions) {   
	char 		szResultFn[EDMAXPATHLEN];
	char		szAsPath[EDMAXPATHLEN];
	FTABLE 		*fp;
	int 		ret;
	int			fileflags = 0;
	const char* pszHint = pOptions->fo_dockName;

	szAsPath[0] = 0;
	lastSelectedDocType = 0;
	if (fn) {
		string_getFullPathName(szResultFn,fn);
		fn = szResultFn;
	}
	if (fn && ft_editing(fn) != 0) {
		ret = error_displayYesNoCancelConfirmation(IDS_MSGINWORK,string_abbreviateFileNameOem(fn));
		if (ret == IDCANCEL) {
			return 0;
		}
		if (ret == IDNO) {
			if (ft_activateWindowOfFileNamed(fn)) {
				return ft_getCurrentDocument();
			}
			return 0;
		}
	}
	if (fn && file_exists(fn) < 0) {
		if (error_displayYesNoConfirmation(IDS_MSGQUERYNEWFILE, string_abbreviateFileNameOem(fn)) == IDNO) {
			return 0;
		}
		fileflags = F_NEWFILE;
	} else {
		char szBuf[80];
		if (fn == NULL) {
			fileflags = F_NEWFILE|F_NAME_INPUT_REQUIRED;
			for (int i = 1; i < 100; i++) {
				sprintf(szBuf, "newfile%d.txt", i);
				if (ft_fpbyname(szBuf) == NULL) {
					break;
				}
			}
			fn = szBuf;
		}
		if (!(fileflags & F_NEWFILE) && 
			(GetConfiguration()->options & O_DELETE_AUTOSAVE_FILES) &&
			ft_generateAutosavePathname(szAsPath, fn) &&
			areFilenamesDifferent(szAsPath, fn) &&
			file_exists(szAsPath) == 0 &&
			error_displayYesNoConfirmation(IDS_MSGRECOVER, string_abbreviateFileName(fn)) == IDYES) {
			;
		} else {
			szAsPath[0] = 0;
		}
	}

	if ((fp = ft_new()) == 0) {
		return 0;
	}
	if (szAsPath[0]) {
		lstrcpy(fp->fname, szAsPath);
	} else {
		lstrcpy(fp->fname, fn);
	}
	fp->flags |= fileflags;
	if (doctypes_assignDocumentTypeDescriptor(fp, doctypes_getDocumentTypeDescriptor(lastSelectedDocType)) == 0 ||
         ft_readfile(fp, fp->documentDescriptor,pOptions->fo_codePage, 0) == 0 || 
	    (lstrcpy(fp->fname, fn), ft_openwin(fp, pszHint) == 0)) {
		ft_destroy(fp);
		return 0;
	}
	if (szAsPath[0]) {
		ft_setFlags(fp, fp->flags | F_MODIFIED);
		// restore the time from the actual file - do not use the timestamp of the auto-saved file.
		file_getAccessTime(fp->fname, &fp->ti_created, &fp->ti_modified);
	}

	caret_placeCursorInCurrentFile(WIPOI(fp), line,0L);

	if (fileflags != 0 && fp->documentDescriptor) {
		macro_executeByName(fp->documentDescriptor->creationMacroName);
	}

	return fp;
}


/*------------------------------------------------------------
 * ft_openBackupfile()
 * Open the backup file of the given file. The backup file is opened in 
 * read-only mode by default. If the file cannot be opened NULL is returned.
 */
FTABLE* ft_openBackupfile(FTABLE* fp) {
	char backupFilename[EDMAXPATHLEN];

	ft_getBackupFilename(fp, backupFilename);
	FTABLE* fpBackup = ft_openFileWithoutFileselector(backupFilename, 0l, &(FT_OPEN_OPTIONS) { NULL, fp->codepage });
	if (fpBackup != NULL) {
		ft_setFlags(fpBackup, fpBackup->flags | F_RDONLY | F_TRANSIENT);
		doctypes_assignDocumentTypeDescriptor(fpBackup, fp->documentDescriptor);
		doctypes_documentTypeChanged(FALSE);
	}
	return fpBackup;
}

/*------------------------------------------------------------
 * EdEditFile()
 * Edit a file with a filename and with potential flags.
 */
int EdEditFile(long editflags, char *filename) {
	int		nEntry;

	if ((editflags & OPEN_DIRGIVEN) && filename) {
		string_splitFilename(filename,_txtfninfo.path,_txtfninfo.search);
		filename = 0;
	}
	if (editflags & OPEN_HISTORY) {
		nEntry = (editflags & 0xF000) >> 12;
		if ((filename = hist_getString(OPEN_FILES, nEntry)) == 0) {
			return 0;
		}
	}
	long codepage = -1;
	if ((editflags & OPEN_NOFN) == 0) {
		FILE_SELECT_PARAMS fsp;
		memset(&fsp, 0, sizeof fsp);
		fsp.fsp_saveAs = FALSE;
		fsp.fsp_codepage = -1;
		fsp.fsp_optionsAvailable = TRUE;
		if (!fsel_selectFileWithTitle(IDS_MSGOPEN, _fseltarget, &fsp)) {
			return 0;
		}
		codepage = fsp.fsp_codepage;
		filename = _fseltarget;
	}
	return ft_openFileWithoutFileselector(filename, 0L, &(FT_OPEN_OPTIONS) { NULL, codepage }) != NULL ? 1 : 0;
}

/*------------------------------------------------------------
 * ft_abandonFile()
 * Discard changes in a file and re-read.
 */
struct tagPOSITION {
	long ln;
	long col;
};
static int ft_abandoned(WINFO* wp, struct tagPOSITION* pPosition) {
	caret_placeCursorInCurrentFile(wp, pPosition->ln, pPosition->col);
	return 1;
}
int ft_abandonFile(FTABLE *fp, EDIT_CONFIGURATION *linp) {
	long   	ln,col;

	if  (fp == 0) {
		return 0;
	}
	WINFO* wp = WIPOI(fp);

	if (fp->flags & F_MODIFIED) {
		if (error_displayYesNoConfirmation(IDS_MSGABANDON) == IDNO)
			return 1;
	}

	ln = wp->caret.ln;
	col = wp->caret.offset;
	caret_moveToLine(wp, 0);
	fp->nlines = 0;
	ft_bufdestroy(fp);

	if (undo_initializeManager(fp) == 0 || 
	    !doctypes_assignDocumentTypeDescriptor(fp, linp) ||
	    !ft_readfile(fp,fp->documentDescriptor, fp->codepage, 0)) {
		fp->flags = 0;
		ww_close(wp);
		return 0;
	}
	ln_removeFlag(fp->firstl, fp->lastl, LNMODIFIED);
	ft_setFlags(fp, fp->flags & ~F_CHANGEMARK);

	/*
	 * uses currfile----
	 */
	if (ln >= fp->nlines) {
		ln = fp->nlines-1;
	}

	ft_forAllViews(fp, ft_abandoned, &(struct tagPOSITION){ln, col});

	doctypes_documentTypeChanged(FALSE);
	render_repaintAllForFile(fp);

	return 1;
}

/*
 *  Answer 1, if the passed file is modified.
 */
int ft_isFileModified(FTABLE* fp) {
	return fp->flags & F_MODIFIED;
}

/*---------------------------------*/
/* ft_checkReadonlyWithError()					*/
/*---------------------------------*/
int ft_checkReadonlyWithError(FTABLE* fp)
{
	if (ft_isReadonly(fp)) {
		error_showErrorById(IDS_MSGRDONLY, string_abbreviateFileName(ft_visibleName(fp)));
		return 1;
	}
	return 0;
}

/*
 * Checks whether the passed file buffer can be modified or whether it is readonly. 
 */
int ft_isReadonly(FTABLE* fp) {
	return fp->flags & (F_RDONLY|F_WATCH_LOGFILE) ? 1 : 0;
}

/*--------------------------------------------------------------------------
 * EdFileAbandon()
 * Cancel all changes in he current file.
 */
int EdFileAbandon(void) {
	return ft_abandonFile(ft_getCurrentDocument(), (EDIT_CONFIGURATION *)0);
}

/**
 * Save all files currently in state modified. 
 */
int EdSaveAllFiles() {
	for (FTABLE* fp = _filelist; fp; fp = fp->next) {
		if ((fp->flags & (F_NAME_INPUT_REQUIRED | F_MODIFIED)) == F_MODIFIED && !ft_checkReadonlyWithError(fp)) {
			if (!ft_writefileMode(fp, WFM_QUIET)) {
				return 0;
			}
		}
	}
	return 1;
}

/*------------------------------------------------------------
 * EdSaveFile()
 * Save the current editor window. Depending on the passed options
 * this may require the user to enter a file name or to do nothing (if the file
 * is unchanged) etc...
 */
int EdSaveFile(int flg) {
	FTABLE *fp;

#ifdef	DEMO
	if (flg & ~SAV_QUIT) {
		error_displayAlertDialog("Demoversion\nSichern nicht möglich");
		return 0;
	}
#else
	if ((fp = ft_getCurrentDocument()) == 0) 
		return(0);	 		/* currently no File open */

	if (flg != SAV_QUIT && ft_checkReadonlyWithError(fp)) {
		return 0;
	}

	if ((flg & SAV_AS) || (fp->flags & (F_NAME_INPUT_REQUIRED|F_MODIFIED)) == (F_NAME_INPUT_REQUIRED | F_MODIFIED)) {
		char newname[512];
		EDIT_CONFIGURATION* pConfig = fp->documentDescriptor;
		string_splitFilename(fp->fname,newname,_txtfninfo.fname);
		if (newname[0]) {
			strcpy(_txtfninfo.path, newname);
		}
		FILE_SELECT_PARAMS fsp;
		fsp.fsp_saveAs = TRUE;
		fsp.fsp_optionsAvailable = TRUE;
		fsp.fsp_codepage = fp->codepage;
		fsp.fsp_encrypted = pConfig->workmode & O_CRYPTED ? TRUE : FALSE;
		if (fsel_selectFileWithTitle(MSAVEAS, newname, &fsp) == 0) {
			return 0;
		}
		if (fsp.fsp_encrypted) {
			pConfig->workmode |= O_CRYPTED;
		} else {
			pConfig->workmode &= ~O_CRYPTED;
		}
		fp->codepage = fsp.fsp_codepage;
		if (areFilenamesDifferent(newname,fp->fname) && file_exists(newname) >= 0) {
			if (error_displayYesNoConfirmation(IDS_MSGOVERWRITE,string_abbreviateFileNameOem(newname)) == IDNO)
				return 0;
			/* if ret == "APPEND".... fp->flags |= F_APPEND */
		}
		strcpy(fp->fname, newname);
		// Document type may have changed as a result of saving under a new name. Recalculate document descriptor.
		doctypes_assignDocumentTypeDescriptor(fp, 0);
		doctypes_documentTypeChanged(FALSE);
		int flags = fp->flags;
		ft_forAllViews(fp, (int(*)(WINFO*,void*))ww_setwindowtitle, (void*)TRUE);
		flags |= F_CHANGEMARK;
		if (!(flags & F_APPEND)) flags |= F_SAVEAS;
		flags &= ~(F_NEWFILE|F_NAME_INPUT_REQUIRED);
		ft_setFlags(fp, flags);
		if (!ft_writeFileWithAlternateName(fp)) {
			return 0;
		}
	}

	if (flg & SAV_FORCED)
		ft_setFlags(fp, fp->flags | F_WFORCED);  /* force to write even if 
							   not edited */

	if ((flg & SAV_SAVE) && !ft_writeFileWithAlternateName(fp)) return 0;

	if (flg & SAV_QUIT) {
		// Close only one of the cloned windows.
		// B.t.w.: do not use ft_forAllWindows(close....) -> won't work, as ft_forAll cannot handle the case windows are added or closed
		// while iterating.
		ww_close(WIPOI(fp));
	}
#endif
	return(1);
}

/*
 * ft_setFlags()  
 * Assign new flags to the file. If the flags have changed, notify all our views of the change.
 */
void ft_setFlags(FTABLE *fp, int newFlags) {
	int oldFlags = fp->flags;
	if (oldFlags != newFlags) {
		PROPERTY_CHANGE change;
		change.prop_type = FT_FLAGS;
		change.prop_oldValue.v_simpleValue = oldFlags;
		change.prop_newValue.v_simpleValue = newFlags;
		fp->flags = newFlags;
		ft_firePropertyChange(fp, &change);
	}
}

/*
 * Assign a new file name. 
 */
void ft_setOutputFilename(FTABLE* fp, char* pNewName) {
	char oldName[sizeof fp->fname];
	if (strcmp(fp->fname, pNewName) == 0) {
		return;
	}
	strcpy(oldName, pNewName);
	PROPERTY_CHANGE change;
	change.prop_type = FT_NAME;
	change.prop_oldValue.v_string = oldName;
	change.prop_newValue.v_string = pNewName;
	strcpy(fp->fname, pNewName);
	ft_firePropertyChange(fp, &change);
}

/*
 * Assign a new title to display for a file.
 */
void ft_setTitle(FTABLE* fp, char* pNewName) {
	char oldName[256];
	if (fp->title && strcmp(fp->title, pNewName) == 0) {
		return;
	}
	strcpy(oldName, pNewName);
	PROPERTY_CHANGE change;
	change.prop_type = FT_NAME;
	change.prop_oldValue.v_string = oldName;
	change.prop_newValue.v_string = pNewName;
	fp->title = _strdup(pNewName);
	ft_firePropertyChange(fp, &change);
}

/*--------------------------------------------------------------------------
 * EditDroppedFiles()
 */
void EditDroppedFiles(HDROP hDrop)
{
	LPSTR	pszFileName;
	int		i;
	int		nTotal;
	int		nFileLength;

	nTotal = DragQueryFile( hDrop , 0xFFFFFFFF, NULL, 0 );

	for ( i = 0; i < nTotal; i++ )
	{
		nFileLength  = DragQueryFile( hDrop , i , NULL, 0 );
		pszFileName = malloc(nFileLength + 1);
		DragQueryFile( hDrop , i, pszFileName, nFileLength + 1 );
		if (!EdEditFile(OPEN_NOFN, pszFileName)) {
			break;
		}
		free(pszFileName);
	}
}

