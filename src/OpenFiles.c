/*
 * Project: PKS Edit for Windows
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
#include "pksrc.h"
#include "errordialogs.h"
#include "resource.h"
#include "editorconfiguration.h"

#include "winfo.h"
#include "winterf.h"
#include "actions.h"
#include "grammar.h"

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
#include "pksmacro.h"
#include "crossreferencelinks.h"
#include "propertychange.h"
#include "arraylist.h"
#include "mainframe.h"
#include "funcdef.h"

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

static	FTABLE 	*_currentFile;
static	FTABLE* _currentSteplistFile;
static FTABLE 	*_filelist;

#define HISTORY_FILE_NAME "pkssession.json"
int				_ExSave;

/*--------------------------------------------------------------------------
 * ft_generateAutosavePathname()
 */
static int ft_generateAutosavePathname(char *destinationName, const char *fname)
{	char fn[EDMAXPATHLEN];
	char szBuff[EDMAXPATHLEN];

	string_splitFilename(fname,(char *)0,fn,sizeof fn);
	string_concatPathAndFilename(szBuff, config_getPKSEditTempPath(),fn);
	*destinationName = 0;
	string_getFullPathName(destinationName,szBuff,EDMAXPATHLEN);
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
	if (_currentSteplistFile != NULL) {
		return _currentSteplistFile;
	}
	for (FTABLE* fp = _filelist; fp; fp = fp->next) {
		if (fp->navigationPattern) {
			_currentSteplistFile = fp;
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
	if ((fd = file_openFile(fp->fname, FALSE)) < 0) {
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
	ln_pasteLines(wp, ftAppend.firstl, ftAppend.lastl, lpd, 0, 0);
	int nLines = ln_cnt(ftAppend.firstl, ftAppend.lastl)-1;
	LINE* lpPrevious = ln_relative(fp->lastl, -nLines);
	if (lpPrevious) {
		ln_removeFlag(lpPrevious, fp->lastl, LNMODIFIED);
	}
	ln_listfree(ftAppend.firstl);
	free(ftAppend.documentDescriptor);
	ft_setFlags(fp, oldFlags);
	render_repaintAllForFile(fp);
	file_closeFile(&fd);
	return 1;
}

void ft_checkForChangedFiles(BOOL bActive) {
	FTABLE *	fp;
	EDTIME		lCurrentTime;
	BOOL bChangesDiscarded = FALSE;

	for (fp = _filelist; fp && !bChangesDiscarded; fp = fp->next) {
		if (!file_getFileAttributes(fp->fname, 0, &lCurrentTime, NULL)) {
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
						ft_rereadFileAndDiscardChanges(fp, (EDIT_CONFIGURATION*)0);
						fp->flags |= F_WATCH_LOGFILE;
						// may make the list of files invalid.
						bChangesDiscarded = TRUE;
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
					ww_selectWindow(wp);
					BOOL bSilentReload = GetConfiguration()->options & O_SILENTLY_RELOAD_CHANGED_FILES;
					if ((bSilentReload && !ft_isFileModified(fp)) ||
						error_displayYesNoConfirmation(IDS_MSGFILESHAVECHANGED, string_abbreviateFileName(fp->fname)) == IDYES) {
						ft_rereadFileAndDiscardChanges(fp, (EDIT_CONFIGURATION*)0);
						// may make the list of files invalid.
						bChangesDiscarded = TRUE;
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
	long	  		dclicks;
	long long		now;
	register FTABLE *fp;
	char     		spath[EDMAXPATHLEN];
	static long long 	nchkclick;
	static int 		inAutoSave;

	if (inAutoSave || (dclicks = (GetConfiguration()->autosaveSeconds * HZ)) == 0) {
		/* autosave option is OFF */
		return 0;
	}
	now = GetTickCount64();
	if (!nchkclick) {
		nchkclick = now + (long long)(5l * HZ);
	}
	if (now < nchkclick) {
		return 0;
	}

	BOOL bAutoaveToTemp = GetConfiguration()->options & O_AUTOSAVE_TO_TEMP;
	inAutoSave = 1;

	/* check every 5 secs */
	nchkclick = now + (long long)(5 * HZ);

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
void ft_saveWindowStates(void ) {
	int    				s;
	WINFO *				wp;
	char   				szBuff[EDMAXPATHLEN];
	char*				pszFilename;

	int nWindows = ww_getNumberOfOpenWindows();
	WINFO* wpActive = ww_getCurrentEditorWindow();
	hist_resetHistory(OPEN_IN_EDITOR);
	for (s = 1; s <= nWindows; s++) {
		if ((wp = ww_findwinid(s)) != 0) {
			FTABLE* fp = wp->fp;
			if (!(fp->flags & (F_TRANSIENT | F_NAME_INPUT_REQUIRED))) {
				int nIndex = arraylist_indexOf(fp->views, wp);
				int nDispmode = wp->dispmode;
				if (nDispmode == fp->documentDescriptor->dispmode) {
					nDispmode = -1;
				}
				xref_addSearchListEntry(wp, szBuff, fp->fname, wp->caret.ln,
					mainframe_getOpenHint(wp, wp == wpActive, nIndex > 0, nDispmode));
				hist_saveString(OPEN_IN_EDITOR, szBuff);
			}
		}
	}
	pszFilename = file_searchFileInPKSEditLocationFlags(HISTORY_FILE_NAME, CFSF_SEARCH_LOCAL_PKS_SYS | CFSF_SEARCH_APP_PKS_SYS | CFSF_SEARCH_PKS_SYS_OVERRIDE_DIR);
	if (pszFilename == NULL) {
		string_concatPathAndFilename(szBuff, _pksSysOverrideFolder[0] ? _pksSysOverrideFolder : _pksSysFolder, HISTORY_FILE_NAME);
	} else {
		string_getFullPathName(szBuff, pszFilename, sizeof szBuff);
	}
	pszFilename = szBuff;
	hist_saveSession(pszFilename);
}

/*---------------------------------*/
/* ft_restorePreviouslyOpenedWindows() */
/*---------------------------------*/
int ft_restorePreviouslyOpenedWindows(void) {

	if (GetConfiguration()->options & O_AUTO_OPEN_HISTORY) {
		return hist_readSession(HISTORY_FILE_NAME);
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
			file_fileNamesDiffer(fp->fname,as_name)) {
		_unlink(as_name);
	}
}

/*------------------------------------------------------------
 * ft_destroy()
 */
void ft_destroy(FTABLE *fp) {
	EdTRACE(log_message(DEBUG_TRACE, "ft_destroy(fp = 0x%lx, name = %s)", fp, fp->fname));
	ft_deleteautosave(fp);
	ft_bufdestroy(fp);
	free(fp->title);
	fp->title = 0;

	ll_delete((void**) & _filelist, fp);

	if (P_EQ(fp, ft_getCurrentErrorDocument())) {
		_currentSteplistFile = NULL;
	}
	if (!_filelist || P_EQ(fp, ft_getCurrentDocument())) {
		_currentFile = NULL;
	}
}

/*------------------------------------------------------------
 * ft_new()
 */
FTABLE *ft_new(void)
{	FTABLE *fp;

	if ((fp = ll_insert((void**)&_filelist, sizeof * fp)) == 0) {
		EdTRACE(log_message(DEBUG_ERR, "ft_new() failed to create FTABLE pointer."));
		return 0;
	}

	if (undo_initializeManager(fp) == 0) {
		ll_delete((void**)&_filelist,fp);
		return 0;
	}
	EdTRACE(log_message(DEBUG_TRACE,"ft_new()->fp = 0x%lx",fp));

	return fp;
}

/*------------------------------------------------------------
 * ft_visibleName()
 */
char *ft_visibleName(FTABLE *fp) {	
	static char buf[EDMAXPATHLEN];

	if (fp) {
		strcpy(buf, fp->fname);
	}
	return buf;
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

	if (!ww_getSelectionLines(wp, &ln1, &ln2, FALSE)) {
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
FTABLE *ft_fpbyname(const char *fn)
{	FTABLE *fp;

	for (fp = _filelist; fp; fp = fp->next) {
		if (!file_fileNamesDiffer(fp->fname,fn)) {
		    return fp;
		}
	}
	return 0;
}

/*------------------------------------------------------------
 * ft_editing()
 * Answer true, if we are editing the file named filename.
 */
int ft_editing(const char *fn)
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
static int ft_openwin(FTABLE *fp, OPEN_WINDOW_OPTIONS* pOptions) {
	HWND hwndNew;
	size_t nOldWindows = fp->views == 0 ? 0 : arraylist_size(fp->views);
	WINFO* wpOld = nOldWindows == 1 ? arraylist_get(fp->views, 0) : NULL;
	if ((hwndNew = ww_createEditWindow(fp->fname, (LPVOID)(uintptr_t)fp, pOptions->owo_dockName, pOptions->owo_activate)) == 0) {
		return 0;
	}
	WINFO* wp = ww_getWinfoForHwnd(hwndNew);
	if (wp == NULL) {
		return 1;
	}
	if (nOldWindows == 1) {
		if (pOptions->owo_preferredRendererMode) {
			RENDERER_SUPPORTS_MODE pFunc = wp->renderer->r_supportsMode;
			if (pFunc && pFunc((EDIT_MODE) {
				.em_displayMode = TRUE, .em_flag = pOptions->owo_preferredRendererMode
			})) {
				ww_changeDisplayMode(wp, wp->dispmode | pOptions->owo_preferredRendererMode);
			}
		}
		if (pOptions->owo_linkWithExisting && wpOld) {
			ww_linkWindows(wp, wpOld, FALSE);
		}
	} else if (nOldWindows == 0) {
		const char* pszMode = grammar_defaultDisplayMode(fp->documentDescriptor->grammar);
		if (pszMode != NULL) {
			int flag = wp->dispmode;
			if (strcmp(pszMode, "hex") == 0) {
				flag |= SHOW_HEX_DISPLAY;
			} else if (strcmp(pszMode, "wysiwyg") == 0) {
				flag |= SHOW_WYSIWYG_DISPLAY;
			}
			ww_changeDisplayMode(wp, flag);
		}
	}
	return 1;
}


/*
 * Open a second window for the passed file. 
 */
int ft_cloneWindow(WINFO* wp, BOOL forWysiwyg) {
	if (!wp) {
		return 0;
	}
	FTABLE* fp = wp->fp;
	if (fp == NULL) {
		return 0;
	}
	OPEN_WINDOW_OPTIONS options = {
		.owo_dockName = DOCK_NAME_RIGHT,
		.owo_linkWithExisting = forWysiwyg,
		.owo_preferredRendererMode = forWysiwyg ? SHOW_WYSIWYG_DISPLAY : 0,
		.owo_activate = TRUE
	};
	return ft_openwin(fp, &options);
}

/*
 * Clone a window given the file name of the window. 
 */
int ft_cloneWindowNamed(char* pszFilename, OPEN_WINDOW_OPTIONS* pOptions) {
	FTABLE* fp = ft_fpbyname(pszFilename);
	if (fp == NULL) {
		return 0;
	}
	return ft_openwin(fp, pOptions);
}

/*
 * This flag will be set, if the user cancelled the closing of a window, which will then abort
 * the process of enumerating further windows to close.
 */
BOOL ww_requestToCloseCancelled;

/*------------------------------------------------------------
 * ww_requestToClose()
 * The user requests to close a file (last window of a file). 
 * If the file is modified and cannot be saved or some other error
 * occurs, return 0, otherwise, if the file can be closed return 1.
 */
int ww_requestToClose(WINFO *wp) {
	FTABLE* fp = wp->fp;
	ww_requestToCloseCancelled = FALSE;
	if (ft_isFileModified(fp)) {
		ShowWindow(hwndMain,SW_SHOW);
		ww_selectWindow(wp);
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
			case IDCANCEL:
				ww_requestToCloseCancelled = TRUE;
				return (0);
		}
	}
	if (!(fp->flags & (F_TRANSIENT|F_NEWFILE))) {
		hist_saveString(OPEN_FILES, fp->fname);
	}
	return 1;
}

/*
 * Select the window with the given WINFO pointer.
 */
int ww_selectWindow(WINFO* wp) {
	SendMessage(hwndMain, WM_MDIACTIVATE, (WPARAM)wp->edwin_handle, (LPARAM)0L);
	SetFocus(wp->edwin_handle);
	return 1;
}

/*------------------------------------------------------------
 * ft_selectWindowWithId()
 * Select and activate the window with the given window id.
 */
int ft_selectWindowWithId(int winid) {
	WINFO *	wp;

	wp = ww_findwinid(winid);
	if (wp == 0) {
		error_showErrorById(IDS_MSGWRONGWINDOW);
		return 0;
	}
	return ww_selectWindow(wp);
}

/*------------------------------------------------------------
 * ft_select()
 * Make the passed filebuffer the "current" edited file in PKS Edit.
 */
int ft_currentFileChanged(FTABLE *fp) {
	if (fp && fp->navigationPattern) {
		if (_currentSteplistFile != fp) {
			_currentSteplistFile = fp;
			EdTRACE(log_message(DEBUG_TRACE, "ft_currentErrorListChanged(name = %s)", fp->fname));
		}
	}
	if (fp == _currentFile) {
		return 1;
	}
	EdTRACE(log_message(DEBUG_TRACE,"ft_currentFileChanged(name = %s)", fp ? fp -> fname : "NO-FILE"));
	_currentFile = fp;
	action_commandEnablementChanged(ACTION_CHANGE_COMMAND_ALL);
	if (fp == 0) {
		return 0;
	}
	grammar_documentTypeChanged(fp->documentDescriptor->grammar);
	return 1;
}

/*------------------------------------------------------------
 * fsel_selectFileWithTitle()
 * Select a file given a resource id for the nCommand and return the selected
 * filename in "result" (must be large enough to hold the pathname (EDMAXPATHLEN)).
 */
static FSELINFO _txtfninfo = {"."};
int fsel_selectFileWithTitle(int nCommand, char *result, FILE_SELECT_PARAMS* pFSP)
{
	char *fn;
	BOOL	bRet;

	bRet = FALSE;
	WINFO* wp = ww_getCurrentEditorWindow();
	if (wp) {
		FTABLE* fp = wp->fp;
		string_splitFilename(fp->fname, _txtfninfo.path, 0, sizeof _txtfninfo.path);
	}
	if ((fn = fsel_selectFileWithOptions(&_txtfninfo, nCommand, pFSP)) != 0) {
		strcpy(result,fn);
		bRet = TRUE;
	}
	return (int) bRet;
}


/*------------------------------------------------------------
 * ft_activateWindowOfFileNamed()
 * Activate the window of the file named 'fn'.
 */
WINFO* ft_activateWindowOfFileNamed(const char *fn) {
	FTABLE 	*fp;
	char 	fullname[EDMAXPATHLEN];

	memset(fullname, 0, sizeof fullname);
	string_getFullPathName(fullname,fn, sizeof fullname);
	fp = ft_fpbyname(fullname);
	if (fp == NULL) {
		return 0;
	}
	WINFO* wp = WIPOI(fp);
	if (wp == NULL) {
		return 0;
	}
	return ww_selectWindow(wp) ? wp : NULL;
}

/*------------------------------------------------------------
 * ft_openFileWithoutFileselector()
 * Open a file with a file name and jump into a line. Place the window to
 * open as defined in the param wsp.
 */
FTABLE* ft_openFileWithoutFileselector(const char *fn, long line, FT_OPEN_OPTIONS* pOptions) {   
	char 		szResultFn[EDMAXPATHLEN];
	char		szAsPath[EDMAXPATHLEN];
	FTABLE 		*fp;
	int 		ret;
	int			nFileCreationFlags = 0;
	const char* pszHint = pOptions->fo_dockName;

	memset(szResultFn, 0, sizeof szResultFn);
	memset(szAsPath, 0, sizeof szAsPath);
	if (fn) {
		string_getFullPathName(szResultFn,fn, sizeof szResultFn);
		if (!szResultFn[0]) {
			strcpy(szResultFn, "file_name_too_long");
		}
		fn = szResultFn;
	}
	EdTRACE(log_message(DEBUG_TRACE, "ft_openFile(name = %s)", fn == NULL ? "no file name" : fn));
	if (fn && ft_editing(fn) != 0) {
		ret = error_displayYesNoCancelConfirmation(IDS_MSGINWORK,string_abbreviateFileName(fn));
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
		nFileCreationFlags = F_NEWFILE;
		if (pOptions->fo_openHistory) {
			error_showErrorById(IDS_WARN_FILE_MOVED_OR_DELETED, fn);
			return 0;
		}
		if (!pOptions->fo_isNewFile && error_displayYesNoConfirmation(IDS_MSGQUERYNEWFILE, string_abbreviateFileName(fn)) == IDNO) {
			return 0;
		}
		szAsPath[0] = 0;
	} else {
		char szBuf[80];
		if (fn == NULL) {
			nFileCreationFlags = F_NEWFILE|F_NAME_INPUT_REQUIRED;
			for (int i = 1; i < 100; i++) {
				sprintf(szBuf, "newfile%d.txt", i);
				if (ft_fpbyname(szBuf) == NULL) {
					break;
				}
			}
			fn = szBuf;
		}
		if (!(nFileCreationFlags & F_NEWFILE) && 
			(GetConfiguration()->options & O_DELETE_AUTOSAVE_FILES) &&
			ft_generateAutosavePathname(szAsPath, fn) &&
			file_fileNamesDiffer(szAsPath, fn) &&
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
	fp->flags |= nFileCreationFlags;
	OPEN_WINDOW_OPTIONS options = {
	.owo_dockName = pszHint,
	.owo_activate = !pOptions->fo_openHistory
	};
	if (doctypes_assignDocumentTypeDescriptor(fp, NULL, NULL) == 0 ||
         ft_readfile(fp, fp->documentDescriptor,pOptions->fo_codePage, 0) == 0 || 
	    (lstrcpy(fp->fname, fn), ft_openwin(fp, &options) == 0)) {
		ft_destroy(fp);
		return 0;
	}
	if (szAsPath[0]) {
		ft_setFlags(fp, fp->flags | F_MODIFIED);
		// restore the time from the actual file - do not use the timestamp of the auto-saved file.
		file_getFileAttributes(fp->fname, &fp->ti_created, &fp->ti_modified, NULL);
	}

	caret_placeCursorInCurrentFile(WIPOI(fp), line,0L);

	if (nFileCreationFlags != 0 && fp->documentDescriptor) {
		macro_executeByName(fp->documentDescriptor->createActionName);
		const char* pszTemplate = grammar_getFileTemplate(fp->documentDescriptor->grammar, fp->fname);
		if (pszTemplate) {
			undo_startModification(fp);
			template_insertCodeTemplateBuffer(WIPOI(fp), pszTemplate, "", FALSE, 0);
		}
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
	FTABLE* fpBackup = ft_openFileWithoutFileselector(backupFilename, 0l, &(FT_OPEN_OPTIONS) { NULL, fp->codepageInfo.cpi_codepage });
	if (fpBackup != NULL) {
		ft_setFlags(fpBackup, fpBackup->flags | F_RDONLY | F_TRANSIENT);
		doctypes_assignDocumentTypeDescriptor(fpBackup, fp->documentDescriptor, NULL);
		doctypes_documentTypeChanged(FALSE);
	}
	return fpBackup;
}

/*
 * Open multiple files encoded into the pszMultiFiles string as described by the GetOpenFilename WIN32 API.
 */
static int ft_openMultipleFiles(const char* pszMultiFiles, int codepage) {
	char fullpath[EDMAXPATHLEN];

	strcpy(fullpath, pszMultiFiles);
	int nOffs = (int)strlen(fullpath);
	fullpath[nOffs++] = '\\';
	int nFilesOffset = nOffs;

	const char* pszRun = pszMultiFiles;
	int nCount = 0;
	while (pszRun[nFilesOffset]) {
		pszRun = &pszRun[nFilesOffset];
		nFilesOffset = (int)(strlen(pszRun) + 1);
		nCount++;
	}
	int nMax = GetConfiguration()->maximumNumberOfOpenWindows;
	if (nMax > 0 && nCount > nMax) {
		error_showErrorById(IDS_TRYING_TO_OPEN_TOO_MANY_FILES, nMax);
		nCount = nMax;
	}
	nFilesOffset = nOffs;
	pszRun = pszMultiFiles;
	while (pszRun[nFilesOffset]) {
		pszRun = &pszRun[nFilesOffset];
		strcpy(&fullpath[nOffs], pszRun);
		nFilesOffset = (int)(strlen(pszRun) + 1);
		if (ft_openFileWithoutFileselector(fullpath, 0L, &(FT_OPEN_OPTIONS) { NULL, codepage }) == NULL) {
			return 0;
		}
		if (--nCount <= 0) {
			break;
		}
	}
	return 1;
}

/*------------------------------------------------------------
 * EdEditFile()
 * Edit a file with a filename and with potential flags.
 */
long long EdEditFile(OPEN_WINDOW_FLAGS editflags, char *filename) {
	int		nEntry;

	if ((editflags & OPEN_DIRGIVEN) && filename) {
		string_splitFilename(filename,_txtfninfo.path,_txtfninfo.search, sizeof _txtfninfo.search);
		filename = 0;
	}
	if (editflags & OPEN_HISTORY) {
		nEntry = (editflags & 0xF000) >> 12;
		if ((filename = hist_getString(OPEN_FILES, nEntry)) == 0) {
			return 0;
		}
	}
	long codepage = -1;
	if (editflags & OPEN_NEW_WITH_FILE_SELECTION) {
		FILE_SELECT_PARAMS fsp = {
			.fsp_saveAs = FALSE,
			.fsp_codepage = -1,
			.fsp_createNewFile = TRUE,
			.fsp_optionsAvailable = FALSE,
			.fsp_multiSelect = FALSE
		};
		memset(_fseltarget, 0, EDMAXPATHLEN);
		if (!fsel_selectFileWithTitle(CMD_OPEN_NEW_FILE, _fseltarget, &fsp)) {
			return 0;
		}
		filename = _fseltarget;
	} else if ((editflags & OPEN_FILE_NO_FILE_SELECTION) == 0) {
		FILE_SELECT_PARAMS fsp = {
			.fsp_saveAs = FALSE,
			.fsp_codepage = -1,
			.fsp_optionsAvailable = TRUE,
			.fsp_multiSelect = TRUE
		};
		memset(_fseltarget, 0, EDMAXPATHLEN);
		if (!fsel_selectFileWithTitle(CMD_OPEN_FILE, _fseltarget, &fsp)) {
			return 0;
		}
		codepage = fsp.fsp_codepage;
		size_t nLen = strlen(_fseltarget) + 1;
		if (_fseltarget[nLen]) {
			return ft_openMultipleFiles(_fseltarget, codepage);
		}
		filename = _fseltarget;
	}
	return ft_openFileWithoutFileselector(filename, 0L, &(FT_OPEN_OPTIONS) { .fo_dockName = NULL, .fo_codePage = codepage, 
		.fo_isNewFile = (editflags & OPEN_NEW_WITH_FILE_SELECTION) != 0 }) != NULL ? 1 : 0;
}

/*------------------------------------------------------------
 * ft_rereadFileAndDiscardChanges()
 * Discard changes in a file and re-read.
 */
struct tagPOSITION {
	long ln;
	long col;
};
static int ft_changesDiscarded(WINFO* wp, struct tagPOSITION* pPosition) {
	caret_placeCursorInCurrentFile(wp, pPosition->ln, pPosition->col);
	return 1;
}
int ft_rereadFileAndDiscardChanges(FTABLE *fp, EDIT_CONFIGURATION *pEditorConfiguration) {
	long   	ln,col;

	if  (fp == 0) {
		return 0;
	}
	WINFO* wp = WIPOI(fp);

	if (fp->flags & F_MODIFIED) {
		if (error_displayYesNoConfirmation(IDS_MSGDISCARD_ALL_CHANGES) == IDNO)
			return 1;
	}

	ln = wp->caret.ln;
	col = wp->caret.offset;
	caret_moveToLine(wp, 0);
	fp->nlines = 0;
	ft_bufdestroy(fp);

	if (undo_initializeManager(fp) == 0 || 
	    !doctypes_assignDocumentTypeDescriptor(fp, pEditorConfiguration, NULL) ||
	    !ft_readfile(fp,fp->documentDescriptor, fp->codepageInfo.cpi_codepage, 0)) {
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

	ft_forAllViews(fp, ft_changesDiscarded, &(struct tagPOSITION){ln, col});

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
	return (fp->flags & (F_RDONLY|F_WATCH_LOGFILE)) ? 1 : 0;
}

/*--------------------------------------------------------------------------
 * EdRereadFileFromDisk()
 * Cancel all changes in the specified editor..
 */
long long EdRereadFileFromDisk(WINFO* wp) {
	return ft_rereadFileAndDiscardChanges(wp->fp, (EDIT_CONFIGURATION *)0);
}

/**
 * Save all files currently in state modified. 
 */
long long EdSaveAllFiles() {
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
long long EdSaveFile(SAVE_WINDOW_FLAGS flags) {
	FTABLE *fp;

#ifdef	DEMO
	if (flags & ~SAV_QUIT) {
		error_displayAlertDialog("Demoversion\nSichern nicht m�glich");
		return 0;
	}
#else
	if ((fp = ft_getCurrentDocument()) == 0) 
		return(0);	 		/* currently no File open */

	if (flags != SAV_QUIT && ft_checkReadonlyWithError(fp)) {
		return 0;
	}

	if ((flags & SAV_AS) || (fp->flags & (F_NAME_INPUT_REQUIRED|F_MODIFIED)) == (F_NAME_INPUT_REQUIRED | F_MODIFIED)) {
		char newname[EDMAXPATHLEN];
		EDIT_CONFIGURATION* pConfig = fp->documentDescriptor;
		string_splitFilename(fp->fname,newname,_txtfninfo.fname, sizeof _txtfninfo.fname);
		if (newname[0]) {
			strcpy(_txtfninfo.path, newname);
		}
		FILE_SELECT_PARAMS fsp = {
			.fsp_saveAs = TRUE,
			.fsp_optionsAvailable = TRUE,
			.fsp_codepage = fp->codepageInfo.cpi_codepage,
			.fsp_encrypted = pConfig->workmode & O_CRYPTED ? TRUE : FALSE
		};
		if (fsel_selectFileWithTitle(CMD_SAVE_FILE_AS, newname, &fsp) == 0) {
			return 0;
		}
		if (fsp.fsp_encrypted) {
			pConfig->workmode |= O_CRYPTED;
		} else {
			pConfig->workmode &= ~O_CRYPTED;
		}
		fp->codepageInfo.cpi_codepage = fsp.fsp_codepage;
		if (file_fileNamesDiffer(newname,fp->fname) && file_exists(newname) >= 0) {
			if (error_displayYesNoConfirmation(IDS_MSGOVERWRITE,string_abbreviateFileName(newname)) == IDNO)
				return 0;
			/* if ret == "APPEND".... fp->flags |= F_APPEND */
		}
		strcpy(fp->fname, newname);
		// Document type may have changed as a result of saving under a new name. Recalculate document descriptor.
		doctypes_assignDocumentTypeDescriptor(fp, NULL, NULL);
		doctypes_documentTypeChanged(FALSE);
		int fpflags = fp->flags;
		ft_forAllViews(fp, (int(*)(WINFO*,void*))ww_setwindowtitle, (void*)TRUE);
		fpflags |= F_CHANGEMARK;
		if (!(fpflags & F_APPEND)) fpflags |= F_SAVEAS;
		fpflags &= ~(F_NEWFILE|F_NAME_INPUT_REQUIRED);
		ft_setFlags(fp, fpflags);
		if (!ft_writeFileWithAlternateName(fp)) {
			return 0;
		}
	}

	if (flags & SAV_FORCED)
		ft_setFlags(fp, fp->flags | F_WFORCED);  /* force to write even if 
							   not edited */

	if ((flags & SAV_SAVE) && !ft_writeFileWithAlternateName(fp)) return 0;

	if (flags & SAV_QUIT) {
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
		PROPERTY_CHANGE change = {
			.prop_type = FT_FLAGS
		};
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
	PROPERTY_CHANGE change = {
		.prop_type = FT_NAME,
		.prop_oldValue.v_string = oldName,
		.prop_newValue.v_string = pNewName
	};
	strcpy(fp->fname, pNewName);
	ft_firePropertyChange(fp, &change);
}

/*
 * Assign a new nCommand to display for a file.
 */
void ft_setTitle(FTABLE* fp, const char* pNewName) {
	char oldName[256];
	if (fp->title) {
		if (strcmp(fp->title, pNewName) == 0) {
			return;
		}
		free(fp->title);
		fp->title = 0;
	}
	strcpy(oldName, pNewName);
	PROPERTY_CHANGE change = {
		.prop_type = FT_NAME,
		.prop_oldValue.v_string = oldName,
		.prop_newValue.v_string = (char*)pNewName
	};
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

	int nMax = GetConfiguration()->maximumNumberOfOpenWindows;
	if (nMax > 0 && nTotal > nMax) {
		error_showErrorById(IDS_TRYING_TO_OPEN_TOO_MANY_FILES, nMax);
		nTotal = nMax;
	}
	for ( i = 0; i < nTotal; i++ ) {
		nFileLength  = DragQueryFile( hDrop , i , NULL, 0 );
		pszFileName = malloc((size_t)nFileLength + 1);
		DragQueryFile( hDrop , i, pszFileName, nFileLength + 1 );
		long long nSuccess = EdEditFile(OPEN_FILE_NO_FILE_SELECTION, pszFileName);
		free(pszFileName);
		if (!nSuccess) {
			break;
		}
	}
}

