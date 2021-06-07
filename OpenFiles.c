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
#include <stdio.h>
#include <shellapi.h>
#include <time.h>

#include "trace.h"
#include "caretmovement.h"
#include "edierror.h"
#include "errordialogs.h"
#include "editorconfiguration.h"

#include "winfo.h"
#include "winterf.h"

/* #define DEMO 1 /* D E M O V E R S I O N */

#pragma hdrstop

#include "dos.h"
#include "pathname.h"
#include "fileselector.h"
#include "edhist.h"
#include "pksedit.h"
#include "stringutil.h"
#include "fileutil.h"
#include "winfo.h"
#include "documenttypes.h"
#include "editorconfiguration.h"
#include "regexp.h"
#include "edfuncs.h"
#include "crossreferencelinks.h"

/*------------------------------------------------------------
 * win_createMdiChildWindow()
 */
extern HWND win_createMdiChildWindow(char* szClass, char* fn, int itemno, LPARAM lParam, WINDOWPLACEMENT* wsp);
extern HWND 	ww_winid2hwnd(int winid);
extern int 	EdPromptAutosavePath(char *path);

extern char *	_datadir;
extern void *	lastSelectedDocType;

static	FTABLE 	*_currentFile,*_currentErrorFile;
static FTABLE 	*_filelist;

#define HISTORY_FILE_NAME "pksedit.his"
static char		*_historyFileName = NULL;
int				_ExSave;

/*--------------------------------------------------------------------------
 * ft_generateBackupPathname()
 */
static int ft_generateBackupPathname(char *destinationName, char *fname)
{	char fn[EDMAXPATHLEN];
	char szBuff[EDMAXPATHLEN];

	if (GetConfiguration()->pksEditTempPath[0]) {
		string_splitFilename(fname,(char *)0,fn);
		string_concatPathAndFilename(szBuff, GetConfiguration()->pksEditTempPath,fn);
		string_getFullPathName(destinationName,szBuff);
		return 1;
	}
	return 0;
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
	return _currentErrorFile;
}

/**
 * Make the passed file the "current error file" - which can be used by clicking on
 * lines displayed in that file to navigate to positions (compiler errors etc...).
 */
void ft_setCurrentErrorDocument(FTABLE* fp) {
	_currentErrorFile = fp;
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

void ft_checkForChangedFiles(void) {
	FTABLE *	fp;
	EDTIME		lCurrentTime;
	
	for (fp = _filelist; fp; fp = fp->next) {
		if (fp->ti_created < (lCurrentTime = file_getAccessTime(fp->fname))) {
			EdSelectWindow(WIPOI(fp)->win_id);
			if (errorDisplayYesNoConfirmation(IDS_MSGFILESHAVECHANGED, string_abbreviateFileNameOem(fp->fname)) == IDYES) {
				ft_abandonFile(fp, (DOCUMENT_DESCRIPTOR *)0);			
			}
		}
		fp->ti_created = lCurrentTime;
	}
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
	static int 		inas;

	if (inas || (dclicks = (GetConfiguration()->asminutes * 60L * HZ)) == 0) {
		/* autosave option is OFF */
		return 0;
	}

	now = GetTickCount();
	if (now < nchkclick) {
		return 0;
	}

	inas = 1;

	/* check every 5 secs */
	nchkclick = now + 5 * HZ;

	saved = 0;
	for (fp = _filelist; fp; fp = fp->next) {

		/* file in unsaved state ? */
		flags = fp->flags;
		if ((flags & F_NEEDSAUTOSAVE) == 0)
			continue;

		/* no "autosave needed"-time: set to current + #of minutes */
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
		/* "best before" date expired: do autosave */
		strcpy(spath,fp->fname);

		ft_generateBackupPathname(fp->fname,fp->fname);
		ret = ft_writefileMode(fp,1);

		/* restore MODIFIED and ISBACKUPED - Flags */
		fp->flags = flags;

		if (ret > 0) {
		/* we autosaved into source file: set state to unmodified */
			if (!GetConfiguration()->pksEditTempPath[0])
				fp->flags &= ~F_MODIFIED;
			error_showMessageInStatusbar(IDS_MSGAUBE,ft_visiblename(fp));
		} else if (EdPromptAutosavePath(GetConfiguration()->pksEditTempPath)) {
		/* let the user correct invalid autosave pathes */
			prof_saveaspath(GetConfiguration());
			goto autosave;
		}
	
		/* restore file name */
		strcpy(fp->fname,spath);

		/*
		 * maybe autosave failed - reset autosave condition nevertheless
		 * cause autosave probably will fail on next cycle(in 5 sec.)
		 * too
		 */
		fp->flags &= ~F_NEEDSAUTOSAVE;
		fp->as_time = 0;
		saved = ret;

	}
	
	inas = 0;
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
	WINDOWPLACEMENT 	ws;
	char   				szBuff[EDMAXPATHLEN];
	char*				pszFilename;

	ft.caret.linePointer = 0;
	ln_createAndAddSimple(&ft,"[files]");

	/* save names of bottom windows first !! */
	for (s = ww_getNumberOfOpenWindows()-1; s >= 0; s--) {
		if ((wp = ww_getWindowFromStack(s)) != 0) {
			ww_getstate(wp, &ws);
			prof_printws(szBuff,&ws);
			FTABLE* fp = wp->fp;
			xref_addSearchListEntry(&ft,fp->fname,fp->ln,szBuff);
		}
	}

	hist_allsave(&ft);
	pszFilename = _historyFileName;
	if (pszFilename == NULL) {
		string_concatPathAndFilename(szBuff, _datadir, HISTORY_FILE_NAME);
		pszFilename = szBuff;
	}
	ft_writeFileAndClose(&ft, pszFilename, FA_NORMAL);
}

/*---------------------------------*/
/* ft_restorePreviouslyOpenedWindows()					*/
/*---------------------------------*/
int ft_restorePreviouslyOpenedWindows(void )
{
	FTABLE 	ft;
	char *	pszFound;
	char	szBuff[EDMAXPATHLEN];

	if (GetConfiguration()->options & O_READPIC) {
		if ((pszFound = file_searchFileInPKSEditLocation(HISTORY_FILE_NAME)) != 0 &&
		    ft_readfileWithOptions(&ft, pszFound, -1)) {
			// save complete filename of history file.
			GetFullPathName(pszFound, sizeof szBuff, szBuff, NULL);
			_historyFileName = string_allocate(szBuff);
			if (_filelist == 0) {
				xref_openSearchListResultFromLine(ft.firstl);
			}
			hist_read(ft.firstl);
			ln_listfree(ft.firstl);
			return 1;
		}
	}
	return 0;
}

/*------------------------------------------------------------
 * ft_bufdestroy().
 * Release all resources associated with a file.
 */
void ft_bufdestroy(FTABLE *fp)
{
	ll_destroy((LINKED_LIST**)&fp->fmark,(int (*)(void *elem))0);
	fp->blstart = 0;
	fp->blend = 0;
	destroy(&fp->documentDescriptor);
	ln_listfree(fp->firstl);
	fp->tln = fp->firstl = fp->caret.linePointer = 0;
	file_closeFile(&fp->lockFd);
	undo_destroyManager(fp);
}

/*------------------------------------------------------------
 * ft_settime()
 */
void ft_settime(EDTIME *tp)
{	time (tp);
}

/*------------------------------------------------------------
 * ft_deleteautosave()
 */
void ft_deleteautosave(FTABLE *fp)
{
	char as_name[EDMAXPATHLEN];

	if ((GetConfiguration()->options & O_GARBAGE_AS) && ft_generateBackupPathname(as_name,fp->fname)) {
		if (areFilenamesDifferent(fp->fname,as_name)) {
			_unlink(as_name);
		}
	}
}

/*------------------------------------------------------------
 * ft_destroy()
 */
void ft_destroy(FTABLE *fp)
{
	EdTRACE(Debug(DEBUG_TRACE,"ft_destroy File 0x%lx",fp));

	ft_deleteautosave(fp);
	ft_bufdestroy(fp);

	ll_delete((LINKED_LIST**)&_filelist,fp);

	if (!_filelist || P_EQ(fp, ft_getCurrentDocument())) {
		_currentFile = NULL;
	}
	if (P_EQ(fp, ft_getCurrentErrorDocument())) {
		_currentErrorFile = NULL;
	}
}

/*------------------------------------------------------------
 * ft_new()
 */
FTABLE *ft_new(void)
{	FTABLE *fp;

	if ((fp = (FTABLE*)ll_insert((LINKED_LIST**)&_filelist,sizeof *fp)) == 0)
		return 0;

	if (undo_initializeManager(fp) == 0) {
		ll_delete((LINKED_LIST**)&_filelist,fp);
		return 0;
	}
	EdTRACE(Debug(DEBUG_TRACE,"ft_new File 0x%lx",fp));

	return fp;
}

/*------------------------------------------------------------
 * ft_name()
 */
char *ft_visiblename(FTABLE *fp)
{	static char buf[512];

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
 * ft_checkSelection()
 * Check whether a block selection exists.
 *---------------------------------*/
int ft_checkSelection(FTABLE* fp) {
	if (fp == 0 || fp->blstart == 0L || fp->blend == 0L)
		return 0;
	return 1;
}

/*---------------------------------
 * ft_checkSelectionWithError()
 * Check whether a block selection exists. If not
 * report an error to the user.
 *---------------------------------*/
EXPORT int ft_checkSelectionWithError(FTABLE* fp) {
	if (ft_checkSelection(fp) == 0) {
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
 * Answer true, if we are editing the file named fn.
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
static int ft_openwin(FTABLE *fp, WINDOWPLACEMENT *wsp)
{
	if (wsp == 0 && fp->documentDescriptor) {
		if (fp->documentDescriptor->dispmode & SHOWFIXEDWI) {
			wsp = (WINDOWPLACEMENT*)&fp->documentDescriptor->placement;
		}
	}

	if (win_createMdiChildWindow(szEditClass, fp->fname, ww_getNumberOfOpenWindows()+1, (LPARAM)(uintptr_t)fp, wsp) == 0) {
		return 0;
	}
	return 1;
}

/*------------------------------------------------------------
 * ft_requestToClose()
 * The user requests to close a file (last window of a file). 
 * If the file is modified and cannot be saved or some other error
 * occurs, return 0, otherwise, if the file can be closed return 1.
 */
int ft_requestToClose(FTABLE *fp)
{
	if (fp->documentDescriptor->cm[0]) {
		if (!macro_executeByName(fp->documentDescriptor->cm)) {
			return 0;
		}
	}
	if (fp->flags & F_MODIFIED) {
		ShowWindow(hwndClient,SW_SHOW);
		EdSelectWindow(WIPOI(fp)->win_id);
		if (_ExSave || (GetConfiguration()->options & AUTOWRITE)) {
	     	return ft_writeFileWithAlternateName(fp);
		}
		switch(error_displayYesNoCancelConfirmation(IDS_MSGQUITORSAVE,ft_visiblename(fp))) {
			case IDYES:
				if (!ft_writeFileWithAlternateName(fp)) {
					if (!(GetConfiguration()->options & WARNINGS)) {
						fp->flags &= ~F_MODIFIED;
					}
					return 0;
				}
				break;
			case IDNO:	 break;
			case IDCANCEL:  return (0);
		}
	}
	hist_enq(OPEN_FILES, fp->fname);
	hist_updatemenu(OPEN_FILES);
	return 1;
}

/*------------------------------------------------------------
 * ft_selectWindowWithId()
 * Select and activate the window with the given window id.
 */
int ft_selectWindowWithId(int winid, BOOL bPopup)
{
	HWND 	hwndChild;
	WINFO *	wp;

	if (winid < 0) {
		if (winid == SEL_CYCLE) {
			winid = -(ww_getNumberOfOpenWindows() - 1);
		}
		if ((wp = ww_getWindowFromStack(-winid)) == 0) {
			return 0;
		}
		winid = wp->win_id;
	}

	if ((hwndChild = ww_winid2hwnd(winid)) == 0) {
		error_showErrorById(IDS_MSGWRONGWINDOW);
		return 0;
	}

	if (bPopup) {
		ww_popup(hwndChild);
	} else {
		SendMessage(hwndClient,WM_MDIACTIVATE,(WPARAM)hwndChild,(LPARAM)0L);
	}
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
int ft_select(FTABLE *fp)
{
	EdTRACE(Debug(DEBUG_TRACE,"ft_select File 0x%lx",fp));
	_currentFile = fp;
	if (fp == 0) {
		return 0;
	}
	regex_compileCharacterClasses(fp->documentDescriptor->u2lset);
	return 1;
}

/*------------------------------------------------------------
 * fsel_selectFileWithTitle()
 * Select a file given a resource id for the title and return the selected
 * filename in "result" (must be large enough to hold the pathname (EDMAXPATHLEN)).
 */
static FSELINFO _txtfninfo = {"."};
int fsel_selectFileWithTitle(int title, char *result)
{
	char *fn;
	BOOL	bRet;

	bRet = FALSE;
	if ((fn = fsel_selectFileWithOptions(&_txtfninfo, title, FALSE)) != 0) {
		strcpy(result,fn);
		bRet = TRUE;
	}
	return (int) bRet;
}


/*------------------------------------------------------------
 * ft_activateWindowOfFileNamed()
 * Activate the window of the file named...
 */
int ft_activateWindowOfFileNamed(char *fn)
{
	FTABLE 	*fp;
	char 	fullname[256];

	string_getFullPathName(fullname,fn);
	if ((fp = ft_fpbyname(fullname)) != 0 && WIPOI(fp) != NULL) {
		return EdSelectWindow(WIPOI(fp)->win_id);
	}
	if (fp != NULL) {
		return 1;
	}
	return 0;
}

/*------------------------------------------------------------
 * ft_optionFileWithoutFileselector()
 * Open a file with a file name and jump into a line. Place the window to
 * open as defined in the param wsp.
 */
int ft_optionFileWithoutFileselector(char *fn, long line, WINDOWPLACEMENT *wsp)
{   
	char 		szResultFn[EDMAXPATHLEN];
	char		szAsPath[EDMAXPATHLEN];
	FTABLE 		*fp;
	int 		ret;
	int			fileflags = 0;

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
			return ft_activateWindowOfFileNamed(fn);
		}
	}
	if (fn && file_exists(fn) < 0) {
		if (errorDisplayYesNoConfirmation(IDS_MSGQUERYNEWFILE, string_abbreviateFileNameOem(fn)) == IDNO) {
			return 0;
		}
		fileflags = F_NEWFILE;
	} else {
		if (fn == NULL) {
			char szBuf[80];
			fileflags = F_NEWFILE|F_NAME_INPUT_REQUIRED;
			for (int i = 1; i < 100; i++) {
				sprintf(szBuf, "newfile%d.txt", i);
				if (ft_fpbyname(szBuf) == NULL) {
					break;
				}
			}
			fn = szBuf;
		}
		if ((GetConfiguration()->options & O_GARBAGE_AS) &&
			ft_generateBackupPathname(szAsPath, fn) &&
			areFilenamesDifferent(szAsPath, fn) &&
			file_exists(szAsPath) == 0 &&
			errorDisplayYesNoConfirmation(IDS_MSGRECOVER) == IDYES) {
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
         ft_readfile(fp, fp->documentDescriptor) == 0 || 
	    (lstrcpy(fp->fname, fn), ft_openwin(fp, wsp) == 0)) {
		ft_destroy(fp);
		return 0;
	}

	if (szAsPath[0]) {
		fp->flags |= F_MODIFIED;
	}

	caret_placeCursorInCurrentFile(line,0L);

	if (fileflags != 0 && fp->documentDescriptor) {
		macro_executeByName(fp->documentDescriptor->creationMacroName);
	}

	return 1;
}

/*------------------------------------------------------------
 * EdEditFile()
 * Edit a file with a filename and with potential flags.
 */
int EdEditFile(long editflags, char *filename)
{
	int		ret;
	int		nEntry;

	if ((editflags & OPEN_DIRGIVEN) && filename) {
		string_splitFilename(filename,_txtfninfo.path,_txtfninfo.search);
		filename = 0;
	}
	if (editflags & OPEN_HISTORY) {
		nEntry = (editflags & 0xF000) >> 12;
		if ((filename = hist_getstring(OPEN_FILES, nEntry)) == 0) {
			return 0;
		}
	}
	ret = ft_optionFileWithoutFileselector(filename, 0L, (WINDOWPLACEMENT*)0);
	return ret;
}

/*------------------------------------------------------------
 * ft_abandonFile()
 * Discard changes in a file and re-read.
 */
int ft_abandonFile(FTABLE *fp, DOCUMENT_DESCRIPTOR *linp)
{
	long   	ln,col;

	if  (fp == 0) {
		return 0;
	}

	if (fp->flags & F_MODIFIED) {
		if (errorDisplayYesNoConfirmation(IDS_MSGABANDON) == IDNO)
			return 1;
	}

	ln = fp->ln;
	col = fp->caret.offset;
	fp->ln = 0;
	fp->nlines = 0;
	ft_bufdestroy(fp);

	if (undo_initializeManager(fp) == 0 || 
	    !doctypes_assignDocumentTypeDescriptor(fp, linp) ||
	    !ft_readfile(fp,fp->documentDescriptor)) {
		fp->flags = 0;
		ww_close(WIPOI(fp));
		return 0;
	}

	fp->flags &= ~F_CHANGEMARK;

	/*
	 * uses currfile----
	 */
	if (ln >= fp->nlines) {
		ln = fp->nlines-1;
	}

	caret_placeCursorInCurrentFile(ln,col);

	doctypes_documentTypeChanged();
	render_repaintAllForFile(fp);

	return 1;
}

/*---------------------------------*/
/* ft_checkReadonlyWithError()					*/
/*---------------------------------*/
int ft_checkReadonlyWithError(FTABLE* fp)
{
	if (fp->documentDescriptor->workmode & O_RDONLY) {
		error_showErrorById(IDS_MSGRDONLY);
		return 1;
	}
	return 0;
}

/*--------------------------------------------------------------------------
 * EdFileAbandon()
 * Cancel all changes in he current file.
 */
void EdFileAbandon(void)
{
	ft_abandonFile(ft_getCurrentDocument(), (DOCUMENT_DESCRIPTOR *)0);
}

/*------------------------------------------------------------
 * EdSaveFile()
 * Save the current editor window. Depending on the passed options
 * this may require the user to enter a file name or to do nothing (if the file
 * is unchanged) etc...
 */
int EdSaveFile(int flg)
{
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

		string_splitFilename(fp->fname,_txtfninfo.path,_txtfninfo.fname);
		if (fsel_selectFileWithTitle(MSAVEAS, newname) == 0) {
			return 0;
		}
		if (areFilenamesDifferent(newname,fp->fname) && file_exists(newname) >= 0) {
			if (errorDisplayYesNoConfirmation(IDS_MSGOVERWRITE,string_abbreviateFileNameOem(newname)) == IDNO)
				return 0;
			/* if ret == "APPEND".... fp->flags |= F_APPEND */
		}
		strcpy(fp->fname,newname);
		ww_setwindowtitle(WIPOI(fp));
		fp->flags |= F_CHANGEMARK;
		if (!(fp->flags & F_APPEND)) fp->flags |= F_SAVEAS;
		fp->flags &= ~(F_NEWFILE|F_NAME_INPUT_REQUIRED);
		if (!ft_writeFileWithAlternateName(fp)) 	
			return 0;
	}

	if (flg & SAV_FORCED)
		fp->flags |= F_WFORCED;  /* force to write even if 
							   not edited */

	if ((flg & SAV_SAVE) && !ft_writeFileWithAlternateName(fp)) return 0;

	if (flg & SAV_QUIT) {
		ww_close(WIPOI(fp));
	}
#endif

	return(1);
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
		pszFileName = _alloc(nFileLength + 1);
		DragQueryFile( hDrop , i, pszFileName, nFileLength + 1 );
		if (!EdEditFile(OPEN_NOFN, pszFileName)) {
			break;
		}
		_free(pszFileName);
	}
}

