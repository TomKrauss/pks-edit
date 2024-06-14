/*
 * documentmodel.h
 *
 * Project: PKS Edit for Windows
 *
 * Describes the model / internal structure of documents edited by PKS Edit
 * and operations on these.
 *
 * 										created: 10.02.87
 * 										last modified:
 *										author: Tom
 *
 *  * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


#ifndef	EDITAB_H
#define	EDITAB_H

#include <crtdefs.h>
#include "edtypes.h"
#include "gitintegration.h"

#define	ED_VERSION	0x20

#define	LN_ALIGN		2		/* minimum size of the linebuffer content area */
#define	FBUFSIZE		48000	/* maximum filebuffer- and linesize */

#define	LINEBUFSIZE		2*FBUFSIZE+160	/* sizeof of _linebuf		 */
#define	MAXLINELEN		24000

#define	MAXSHIFT		36		/* maximum shiftwidth 			 */
#define	RM_DELTA		2		/* delta to automatic right margin  */

/*-------- TYPES --------------*/

typedef struct tagEDIT_CONFIGURATION EDIT_CONFIGURATION;

typedef struct line   {
	struct line*	next;
	struct line*	prev;
	int  			len; 		/* linelength in chars */
	unsigned short	lflg;		/* line flags */
	char 		lbuf[LN_ALIGN];
} LINE;

typedef struct tagLINEINDIRECT {
	int			typ;
	union {
		struct {
			long		deepCount;
			long		count;
			LINE *	list;
		} ind;
	} u;
} LINEINDIRECT;

/// Currently defined via "working mode" in document type descriptor  
#define	O_CRYPTED		0x1000              // not really a workmode

#define	LI_HIDDENLIST		0x0
#define	LI_RULER			0x10

#define	LpIndirect(lp)		((LINEINDIRECT *)(lp)->lbuf)
#define	LpIndirectTyp(lp)	((LINEINDIRECT *)(lp)->lbuf)->typ
#define	LpIndHiddenList(lp)	LpIndirect(lp)->u.ind.list
#define	LpIndNHidden(lp) 	LpIndirect(lp)->u.ind.count
#define	LpIndNTotal(lp) 	LpIndirect(lp)->u.ind.deepCount
#define	LpHasHiddenList(lp)	(((lp)->lflg & LNINDIRECT) && \
							LpIndirectTyp(lp) == LI_HIDDENLIST)

#define	LSTRUCSIZE		(sizeof(LINE) - LN_ALIGN)
#define	LINESIZE(l) 	((long)(LSTRUCSIZE + l->len)) 	 

extern char * _linebuf;

/*---------------------------------*/
/* Pastebuffer					*/
/*---------------------------------*/
typedef struct pastebuf {
	LINE 	*pln;		/* list of lines */
	long 	size,nlines;	/* size & nr of lines */
	int  	pflg;		/* used for macros */
} PASTE;

typedef enum { PLT_CLIPBOARD, PLT_HISTORY, PLT_NAMED_BUFFER } PASTE_LIST_TYPE;

typedef struct tagPASTELIST {
	struct tagPASTELIST *pl_next;		// next paste buffer  - important, that this the 1st struct element to be compatible with LINKED_LIST
	char			pl_id[32];			// name of buffer - important, that this the 2nd struct element to be compatible with LINKED_LIST
	PASTE_LIST_TYPE pl_type;			// the type of this buffer: is it the current standard clip, a previous clip or an explicitly defined named buffer
	PASTE			pl_pasteBuffer;		// The actual data
} PASTELIST;

/* special marker IDs */
typedef enum {
	CNOMARK = 0x00,						// release mark without freeing it
	MARKSTART = 0x100,					// text selection start mark
	MARKEND = 0x101,					// text selection end mark
	MARKSELSTART = 0x102,				// Used in "MARK Text" in find and replace
	MARKSELEND = 0x103,					// Used in "MARK Text" in find and replace 
	MARKDOT = 0x104,					// Temporary mark set before an operation starts (e.g. block formatting) and restored after operation
	MTE_AUTO_LAST_SEARCH = 0x105,		// mark defined automatically before navigating to a new position
	MTE_AUTO_LAST_INSERT = 0x106		// mark defined automatically when editing text
} MARK_TYPE_ENUM;

/* goto textmark (lastpos) */
/* should ideally extend CARET */
typedef struct tagMARK {
	struct tagMARK* m_next;
	int  			m_identifier;		// ID of mark (either a character or a character or a MARK_TYPE_ENUM)
	LINE*			m_linePointer;		// the pointer to the line
	long			m_line;				// the line number
	int  			m_column;			// the marked column.
	char*			m_fname;			// for global marks - the name of the file in which the mark is placed.
} MARK;

typedef struct tagWINFO WINFO;

/*---------------------------------
 * Undoable edit operations on one o more lines
 */
#define	O_MODIFY			0		// modify a line
#define	O_INSERT			1		/* insert a line */
#define	O_DELETE			2		/* EdCharDelete a line */
#define	O_MARK				3		/* mark a textblock */
#define	O_LNORDER			4		/* reorder linelist according to a table */
#define	O_HIDE				5		/* hide a list of lines */
#define	O_UNHIDE			6		/* unhide a list of lines */
#define O_FLAG				7		/* the line flags were changed */

/*---------------------------------*/
/* EDIT_CONFIGURATION			*/
/*---------------------------------*/
#define	fnt_name		editFontStyle.faceName
#define	fnt_fgcolor		editFontStyle.fgcolor
#define	fnt_bgcolor		editFontStyle.bgcolor
#define	fnt_charset		editFontStyle.charset

#if defined(_WINUSER_)

typedef struct tagGRAMMAR GRAMMAR;

/*
 * Defines one segment of the status line with an optional icon and a text.
 * A segment may be tagged with a language if it is available only in a special selected
 * language. Language identifiers are "de", "en", ...
 */
typedef struct tagSTATUS_LINE_SEGMENT {
	struct tagSTATUS_LINE_SEGMENT* sls_next;
	char						   sls_lang[8];
	// Logical width of this segment (1,2,3,...)
	int							   sls_width;
	char*						   sls_text;
	char*						   sls_icon;
	// Optional color for an icon to use
	COLORREF					   sls_iconColor;
} STATUS_LINE_SEGMENT;

typedef struct tagEDIT_CONFIGURATION {
	struct tagEDIT_CONFIGURATION* next;
	unsigned char	name[16];				// configuration name
	GRAMMAR*		grammar;				// the grammar itself.
	int				id;						// # ID currently not really used.
	int				lmargin, rmargin;		// left and right margins (used e.g. for wrapping and formatting text).
	int				tabsize;				// number of columns for the tab character
	int				shiftwidth;				// Number of character positions to shift a text in a shift operation
	int				nl, nl2, cr;			// Lineends 
	int				dispmode;				// show control...
	int				workmode;				// watch brackets, abbrev... 
	long			codepage;				// the code page for the encoding.
	char			tabDisplayFillCharacter;// Display tabs filled with this.
	char			expandTabsWith;			// when inserting a tabulator - replace with this (fill character). If 0 - tabs are not expanded.
	int				tabulatorSizes[32];		// arbitrary tab stops - allowing us to have have tabs at positions 2, 5, 9, 15, ...
	STATUS_LINE_SEGMENT* statuslineSegments; // A status line defined for this configuration.
	char			backupExtension[10];	// Backup extension
	unsigned char	createActionName[24];	// action (command / macro) to execute on document creation
	unsigned char	saveActionName[24];		// action to execute before saving
	int				scrollflags;			// thumbtrack.. 
	int				cursaftersearch;		// CURSOR_SEARCH_PLACEMENT: CP_POSTOP, CP_POSLOW, ...
	int				hscroll;				// scroll n columns
	int				vscroll;				// scroll n Lines
	int				scroll_dy;				// scroll on dist dy to screen
	char			actionContext[32];		// A possibly custom action context for documents described by this type.
	void*			documentType;			// the document type descriptor from which this configuration was created or 
											// NULL for "default" document types not known to PKS Edit
											// i.e. - no file name pattern matches etc...
} EDIT_CONFIGURATION;

/*--------------------------------------------------------------------------
 * Creates the default attributes for editing a document. The returned structure
 * must be freed, when done using it.
 */
extern EDIT_CONFIGURATION* doctypes_createDefaultDocumentTypeDescriptor();

#endif

/* default document descriptor context (Makros are global) */
#define	DEFAULT_DOCUMENT_DESCRIPTOR_CTX			0
#define	DOCUMENT_DESCRIPTOR_NO_CTX				-1
#define	DOCUMENT_DESCRIPTOR_NOT_YET_READ(lp)	(lp->lmargin == -1)

typedef time_t EDTIME;

/*---------------------------------*/
/* Infos about the files			*/
/*---------------------------------*/
typedef void * HIDDENP;

#define	WIPOI(fp)	ft_getPrimaryView(fp)

/*
 * Represents the position of one caret representing an insertion point in a file.
 */
typedef struct tagCARET {
	struct tagCARET* next;	// Multi-caret editing - points to the next caret.
	LINE* linePointer;		/* the current line */
	int   offset;			/* current offset in linebuffer memory */
	int	  virtualOffset;	/* the virtual offset into the linebuffer. That is used to have the cursor try to stay in the same column as it is moved 
							   up and down the screen and will eventually pass by "short lines" forcing it to jump to smaller column positions. */
	long ln;				/* zero based line index into the list of lines */
	long  col;				/* The column index in screen coordinates (not into the buffer) */
} CARET;

typedef enum {
	BOM_NONE = 0,
	BOM_UTF7,
	BOM_UTF8,
	BOM_UTF16BE,
	BOM_UTF16LE,
	BOM_UTF32BE,
	BOM_UTF32LE
} BOM_TYPE;

/*
 * Returns the name of a magic BOM type encoding.
 */
extern char* ft_magicName(BOM_TYPE type);

typedef struct tagCODE_PAGE_INFO {
	long	 cpi_codepage;				// The code page to use (encoding).
	BOM_TYPE cpi_bomType;				// The type of BOM present in the original file or to be written when saving the file
	BOOL     cpi_hasUnsupportedChars;	// set to TRUE, if reading the file revealed, that the file contains characters which could not be converted
										// to 8 bit characters and are not support by PKS Edit
} CODE_PAGE_INFO;

typedef struct tagFTABLE {
	struct tagFTABLE*next;
	char 	fname[512];			// The full pathname.
	char*	title;			// alternate title to display in the caption
	long 	nlines;				// The number of total lines in the fill. Will be reset by changes.
	unsigned int flags;			// A bitwise combination of the F_... flags defined below.
	int		longLinesSplit;		// Count of all long lines, which were split during read, as the lines were too long 
	LINE 	*tln;				// Pointer to current edited line
	LINE 	*firstl,			// first line
			*lastl;				// last line
	LINE*	lpReadPointer;		// used during read operations temporarily
	long	nbytes;				// The number of total bytes in the file. Will be reset by changes to -1.
	EDIT_CONFIGURATION	*documentDescriptor;
	HIDDENP	views;			/* the list of our views */
	HIDDENP	undo;
	long    fileSize;			// The size of the file as it was "last known": is updated during reading and saving the file.
	long 	as_time;			// next time for AUTOSAVE 
	VC_INFO* vcInfo;			// The current repository version status information
	EDTIME	ti_modified;		// last modification time
	EDTIME	ti_created;			// creation time
	EDTIME  ti_saved;			// last save time
	EDTIME  ti_lastChanged;		// last time the file was changed in PKS edit by the user.
	CODE_PAGE_INFO codepageInfo;// The codepage with which this document was read.
	void* navigationPattern;	// For files containing either a PKS Edit search result list or displaying a compiler output, this contains a pointer 
								// to the NAVIGATION_PATTERN (see CrossReferenceLinks.c) defining how to parse the individual lines in the file.
	int		lockFd;				// Filedescriptor for locking - <= 0 if none
} FTABLE;

typedef enum { LINE_MODIFIED, LINE_REPLACED, LINE_INSERTED, LINE_DELETED, LINE_SPLIT, LINES_JOINED, EVERYTHING_CHANGED } MODEL_CHANGE_TYPE;

// Sent, when changes occur in the data model.
typedef struct tagMODEL_CHANGE {
	MODEL_CHANGE_TYPE type;	// The type of change
	FTABLE* fp;				// the file pointer
	LINE* lp;				// single line modified
	LINE* lpNew;			// If a line was replaced this is the new line.
	int col1;				// modification "point" - the regex_addCharacterToCharacterClass where the modification starts
	int col2;				// the second point in the line - if left to col1, chars are deleted if right to it chars are inserted
	int len;				// number of characters inserted / removed
} MODEL_CHANGE;


extern int 	_playing;				/* recorder plays its game ... */

void 	log_vsprintf(char *fmt,...);

/*-------- line ops ----------*/

extern LINE *ln_hide(FTABLE *fp, LINE *lp1, LINE *lp2);
extern LINE *ln_settmp(FTABLE *fp,LINE *lp,LINE **lpold);
extern LINE *ln_modify(FTABLE *fp,LINE *lp,int col1,int col2);
extern LINE *ln_create(int size);
/*----------------------------
 * ln_indexOf()
 * Find the index of a line relative to the beginning or
 * return -1 if not found.
 */
extern long ln_indexOf(FTABLE *fp,LINE *lp);
extern void ln_insert(FTABLE *fp,LINE *pos,LINE *lp);
extern int ln_delete(FTABLE *fp,LINE *lp);
extern LINE *ln_cut(LINE *lp,int physize,int start,int end);
extern LINE *ln_break(FTABLE *fp, LINE *linep, int col);
extern LINE *ln_split(FTABLE *fp, LINE *lc, int pos2, int pos1);
extern LINE *ln_join(FTABLE *fp,LINE *ln1,LINE *ln2,int flg);
extern int ln_joinLines(FTABLE *fp);
extern LINE *ln_goto(FTABLE *fp,long l);
extern LINE *ln_relative(LINE *cl, long l);
extern long ln_cnt(LINE *lps,LINE *lpe);
extern void ln_replace(FTABLE *fp,LINE *oln,LINE *nl);

/*
 * Returns the current active document. Should not be used any more to often
 */
extern FTABLE* ft_getCurrentDocument();

/*
 * Returns the current error document. Should not be used any more to often
 */
extern FTABLE* ft_getCurrentDocument();

/*---------------------------------
 * ft_cutMarkedLines()
 * Cut out the lines which which have a line marker flag.
 * Depending on the bDelete param delete the line.
 *---------------------------------*/
void ft_cutMarkedLines(WINFO* wp, BOOL bDelete);

/*------------------------------------------------------------
 * ft_rereadFileAndDiscardChanges()
 * Discard changes in a file and re-read.
 */
extern int ft_rereadFileAndDiscardChanges(FTABLE* fp, EDIT_CONFIGURATION* linp);

/*------------------------------------------------------------
 * The current active file (window of active file) has changed.
 */
extern int ft_currentFileChanged(FTABLE* fp);

/*
 * Clone a window given the file name of the window.
 */
extern int ft_cloneWindowNamed(char* pszFilename, const char* pszDock);

/*
 *  Answer 1, if the passed file is modified.
 */
extern int ft_isFileModified(FTABLE* fp);

/*--------------------------------------------------------------------------
 * ft_countlinesStartingFromDirection()
 */
extern long ft_countlinesStartingFromDirection(FTABLE* fp, long start, int dir);

/*
 * Checks whether the passed file buffer can be modified or whether it is readonly.
 */
extern int ft_isReadonly(FTABLE* fp);

/*---------------------------------*/
/* ft_checkReadonlyWithError()					*/
/*---------------------------------*/
extern int ft_checkReadonlyWithError(FTABLE* fp);

typedef struct tagWINDOWPLACEMENT WINDOWPLACEMENT;


/*
 * Options for opening a file. 
 */
typedef struct tagFT_OPEN_OPTIONS {
	const char* fo_dockName;			// The place on the screen where the initial window is being displayed.
	const long  fo_codePage;			// The code page to use when opening the document. Specify -1 to have the editor "guess" the correct codepage.
	int         fo_isNewFile;			// can be set to true, to open a window for a new file with a given name.
	int			fo_openHistory;			// set to true, if a window is opened as part of restoring the history.
} FT_OPEN_OPTIONS;

/*------------------------------------------------------------
 * ft_openFileWithoutFileselector()
 * Open a file with a file name and jump into a line. Place the window to
 * open as defined in the param wsp.
 */
extern FTABLE* ft_openFileWithoutFileselector(const char* fn, long line, FT_OPEN_OPTIONS* pOptions);

/*------------------------------------------------------------
 * ft_openBackupfile()
 * Open the backup file of the given file. The backup file is opened in
 * read-only mode by default. If the file cannot be opened NULL is returned.
 */
extern FTABLE* ft_openBackupfile(FTABLE* fp);

/*---------------------------------
 * ln_changeFlag()
 *
 * Add / remove flags from multiple lines - all lines having an expected flag
 * are considered.
 * Return 1, if at least one line has changed.
 */
extern int ln_changeFlag(FTABLE* fp, LINE* lpstart, const LINE* lpend, int flagsearch, int flagmark, int set);

/*
 * Calculates the number of bytes occupied by one line.
 */
extern long ln_nBytes(LINE* lp);

/*
 * Calculates the number of bytes in a file.
 */
extern long long ft_totalBytes(FTABLE* fp);

/*---------------------------------
 * ln_removeFlag()
 *
 * Remove a flag from all lines between lpstart and lpend.
 *---------------------------------*/
extern void ln_removeFlag(LINE* lpstart, LINE* lpend, int flg);

extern char* ft_visibleName(FTABLE* fp);
/* do an autosave */
extern int ft_triggerAutosaveAllFiles(void);
extern void ft_deleteautosave(FTABLE* fp);
extern void ft_destroy(FTABLE* fp);
extern FTABLE* ft_new(void);

/*------------------------------
 * ft_initializeReadWriteBuffers()
/*------------------------------*/
extern BOOL ft_initializeReadWriteBuffers(void);


/*---------------------------------
 * ft_readDocumentFromFile()
 * Standard implementation to read a file in PKS Edit given the file descriptor,
 * a callback method to invoked for each line read and an optional parameter (typically, but not neccessarily the filepointer itself) to
 * be parsed as the first argument to the callback.
 * If the codepage passed is -1, a codepage detection is performed and the detected code page will be returned in pCodepage.
 *---------------------------------*/
extern int ft_readDocumentFromFile(int fd, CODE_PAGE_INFO *pCodepage, 
	unsigned char* (*lineExtractedCallback)(void*, EDIT_CONFIGURATION*, unsigned char*, unsigned char*), void* par);

/*
 * Determines the name of the code page of the file and writes the codepage name
 * to pszName, but only if there is enough space as described by nMaxNameLen.
 */
extern void ft_getCodepageName(FTABLE* fp, char* pszName, size_t nMaxNameLen);

/*--------------------------------------*/
/* ft_readfile()						*/
/*--------------------------------------*/
extern int ft_readfile(FTABLE* fp, EDIT_CONFIGURATION* documentDescriptor, long codepage, long nFileOffset);

#define WFM_QUIET		0x1
#define WFM_AUTOSAVING	0x2

/*-----------------------------------
 * ft_writefileMode()
 * write a file with one of the flags defined above.
 */
extern int ft_writefileMode(FTABLE* fp, int writeFlags);

/*---------------------------------
 * ft_writeFileWithAlternateName()
 */
extern int ft_writeFileWithAlternateName(FTABLE* fp);

/*---------------------------------
 * ft_writefileAsWithFlags()
 */
extern int ft_writefileAsWithFlags(FTABLE* fp, char* fn, int flags, BOOL bVerbose);

/*---------------------------------
 * read a file for internal purpose like a macro file etc...
 */
typedef struct tagFILE_READ_OPTIONS {
	int fro_useDefaultDocDescriptor;	// if 1, use default document descriptor. Otherwise try to load document descriptor depending on filename
	long fro_fileReadOffset;			// if != 0, start to read the file at the given offset (to be able to lazy load file pieces).
	char* fro_fileName;					// the name of the file to read.
	long fro_codePage;					// the wanted code page or -1 for auto-detection of code pages.
} FILE_READ_OPTIONS;

extern int ft_readfileWithOptions(FTABLE* fp, FILE_READ_OPTIONS* pOptions);

extern void ft_checkForChangedFiles(BOOL bActive);

/*---------------------------------
 * ft_triggerAutosaveAllFiles()
 * do an autosave
 *---------------------------------*/
extern int ft_triggerAutosaveAllFiles(void);

/*---------------------------------
 * ft_saveWindowStates()
 * Save the state of the currently opened windows in the config for restore.
 ---------------------------------*/
extern void ft_saveWindowStates(void);

/*---------------------------------
 * ft_restorePreviouslyOpenedWindows()
 *---------------------------------*/
extern int ft_restorePreviouslyOpenedWindows(void);

/*------------------------------------------------------------
 * ft_selectWindowWithId()
 * Select and activate the window with the given window id.
 */
extern int ft_selectWindowWithId(int winid);

/*
 * Calculate the name of the backup file to create for file fp. The pszResult
 * string must be big enough to hold EDMAXPATHLEN number of characters.
*/
void ft_getBackupFilename(FTABLE* fp, char* pszResult);

/*
 * Return TRUE if for the passed file a backup file had been created previously.
 */
extern BOOL ft_backupFileExists(FTABLE* fp);

extern char* ft_visibleName(FTABLE* fp);
/*------------------------------------------------------------
 * ft_size()
 * calculate current file size
 */
extern long ft_size(FTABLE* fp);
/*------------------------------------------------------------
 * ft_fpbyname()
 * Find a filebuffer given the name of the file.
 */
extern FTABLE* ft_fpbyname(const char* fn);
/*------------------------------------------------------------
 * ft_editing()
 * Answer true, if we are editing the file named filename.
 */
extern int ft_editing(const char* fn);

/*--------------------------------------------------------------------------
 * ft_sortFile()
 * Sort the current file / document.
 */
int ft_sortFile(FTABLE* fp, int scope, char* fs, char* keys, char* sel, int sortflags);

/*--------------------------------------------------------------------------
 * Expand tabs in a given source buffer with a source len and replace with spaces and write
 * the result to a destination buffer. Return the result size of the destination buffer or -1
 * on failure and the number of the tabs expanded in pTabs.
 */
extern int ft_expandTabsWithSpaces(WINFO* wp, char* pszDest, size_t nDestLen, const char* pszSource, size_t nSourceLen, long* pTabs);

/*
 * Try to "optimize" spaces with tab characters in a given string (reverse operation to ft_expandTabsWithSpaces). 
 * Return the number of tabs inserted as a replacement for the corresponding spaces in pTabs and the total len 
 * of the resulting buffer as a return value.
 */
extern int ft_compressSpacesToTabs(WINFO* wp, char* pszDest, size_t nDestLen, const char* pszSource, size_t nSourceLen, long* pTabs);

/*
 * Returns the character used to fill "spaces" (inserted instead of a tab character) - typically ' '.
 */
extern char ft_getSpaceFillCharacter(WINFO* wp);

/*------------------------------------------------------------
 * ft_bufdestroy().
 * Release all resources associated with a file.
 */
extern void ft_bufdestroy(FTABLE* fp);

/*
 * Release the scratch buffer as soon as we are done.
 * Note, that afterwards PKS Edit is not functionable any more.
 */
void ft_destroyCaches();

/*------------------------------------------------------------
 * ft_activateWindowOfFileNamed()
 * Activate the window of the file with the given name.
 */
extern int ft_activateWindowOfFileNamed(const char* fn);

/*--------------------------------------------------------------------------
 * ln_destroy()
 * Destroy a line and free all resources occupied by the line.
 * This will not remove the line from the document.
 */
extern void ln_destroy(LINE* lp);

/*
 * ft_setFlags()
 * Assign new flags to the file. If the flags have changed, notify all our views of the change.
 */
extern void ft_setFlags(FTABLE* fp, int newFlags);

/*
 * Assign a new file name.
 */
extern void ft_setOutputFilename(FTABLE* fp, char* pNewName);

/*
 * Assign a new title to display for a file.
 */
extern void ft_setTitle(FTABLE* fp, char* pNewName);

/*------------------------------------------------------------
 * ln_needbytes()
 * Calculates the number of bytes needed for one line.
 */
extern long ln_calculateMemorySizeRequired(LINE* lp, int nl, int cr);

/*--------------------------------------------------------------------------
 * ln_unhide()
 */
extern int ln_unhide(FTABLE* fp, LINE* lpind);

/*------------------------------*
 * ln_listfree()	
 * Free a line list.
 *------------------------------*/
extern void ln_listfree(LINE* lp);

/*--------------------------------------------------------------------------
 * ln_countLeadingSpaces(l)
 */
extern int ln_countLeadingSpaces(LINE* lp);

/*
 * Paste a list of lines into a target line with a target offset column.
 * if bExpandTabs is 1, tabs are expanded by spaces along the way.
 */
extern int ln_pasteLines(WINFO* wp, LINE* lps, const LINE* lpLast, LINE* lpd, int col, int bExpandTabs);

/*--------------------------------------------------------------------------
 * ln_lineIsEmpty()
 */
extern int ln_lineIsEmpty(LINE* lp);

/*
 * Mark a line as being modified.
 */
extern void ln_markModified(LINE* lp);

/*--------------------------------------------------------------------------
 * undo_initializeManager()
 * Initialize the undo manager for text document.
 */
extern BOOL undo_initializeManager(FTABLE* fp);

/*--------------------------------------------------------------------------
 * undo_redoLastModification()
 * Redo the last modification undone the actual undo reverting the last operation.
 */
extern BOOL undo_redoLastModification(FTABLE* fp);

/*--------------------------------------------------------------------------
 * undo_destroyManager()
 * Removes the current undo manager and deallocates all undo buffers.
 */
extern void undo_destroyManager(FTABLE* fp);

/*--------------------------------------------------------------------------
 * undo_startModification()
 * Invoked before a modification on a file is performed.
 */
extern void undo_startModification(FTABLE* fp);

/*--------------------------------------------------------------------------
 * undo_saveOperation()
 * Remembers the actual operation performed.
 */
extern BOOL undo_saveOperation(FTABLE* fp, LINE* lp, LINE* lpAnchor, int op);

/*--------------------------------------------------------------------------
 * undo_lastModification()
 * Performs the actual undo reverting the last operation.
 */
extern BOOL undo_lastModification(FTABLE* fp);

/*
 * Answer true, if redo is available for the given file.
 */
extern BOOL undo_isRedoAvailable(FTABLE* fp);

/*
 * Answer true, if undo is available for the given file.
 */
extern BOOL undo_isUndoAvailable(FTABLE* fp);

extern void recorder_recordAutoInsertFunction(FTABLE* fp, int p);

/*--------------------------------------------------------------------------
 * string_formatDate()
 */
extern void string_formatDate(char* szDbuf, EDTIME* ltime);

/* Flags for EdEditFile */
typedef enum {
	OPEN_LINEAL = 0x1,
	OPEN_NOLINFN = 0x20,
	OPEN_DIRGIVEN = 0x40,
	OPEN_NOFN = 0x80,
	OPEN_HISTORY = 0x100
} OPEN_WINDOW_FLAGS;

/*------------------------------------------------------------
 * EdEditFile()
 * Edit a file with a filename and with potential flags.
 */
extern long long EdEditFile(OPEN_WINDOW_FLAGS editflags, char* filename);

/* Flags for EdSaveFile */
typedef enum {
	SAV_SAVE = 0x1,
	SAV_QUIT = 0x2,
	SAV_AS = 0x4,
	SAV_FORCED = 0x8
} SAVE_WINDOW_FLAGS;

/*------------------------------------------------------------
 * EdSaveFile()
 * Save the current editor window. Depending on the passed options
 * this may require the user to enter a file name or to do nothing (if the file
 * is unchanged) etc...
 */
extern long long EdSaveFile(SAVE_WINDOW_FLAGS flags);

/*--------------------------------------------------------------------------
 * EdRereadFileFromDisk()
 * Cancel all changes in he current file.
 */
extern long long EdRereadFileFromDisk(WINFO* wp);

/* Flags for ínserting a new line into the text */
typedef enum {
	RET_PLUS = 0x1,
	RET_MINUS = 0x2,
	RET_SOFT = 0x4
} RETURN_ACTION_FLAGS;

/*--------------------------------------------------------------------------
 * edit_splitLine()
 * do cr+lf-Actions
 */
extern long long  edit_splitLine(WINFO* wp, RETURN_ACTION_FLAGS flags);

/*--------------------------------------------------------------------------
 * ln_insertIndent()
 * Insert white space characters at the beginning of the passed line so
 * we fill up the space at the beginning of the line, so that the cursor
 * would be visible in the passed column. Return the number of characters
 * inserted parameter &inserted.
 */
extern LINE* ln_insertIndent(WINFO* wp, LINE* lp, int col, int* inserted);

/*-----------------------------------------
 * ln_createMultipleLinesUsingSeparators()
 * Split a passed buffer (end is marked by pend) with given line separator
 * characters t1 and t2. If cr is passed (not 0), the cr character
 * is handled as such.
 */
extern unsigned char* ln_createMultipleLinesUsingSeparators(FTABLE* fp, const unsigned char* p, const unsigned char* pend, int t1, int t2, int cr);

/*------------------------------------
 * ln_createMultipleLinesFromBuffer()
 * Create multiple lines from a passed buffer p. The
 * end of the buffer is passed in pend.
 */
extern unsigned char* ln_createMultipleLinesFromBuffer(FTABLE* fp, EDIT_CONFIGURATION* documentDescriptor, unsigned char* p, unsigned char* pend);

/*---------------------------------
 * ln_createFromBuffer()
 * Creates a line from a buffer p. The end of the buffer
 * is passed in pend.
 */
extern unsigned char* ln_createFromBuffer(FTABLE* fp, EDIT_CONFIGURATION* documentDescriptor, unsigned char* p, const unsigned char* pend);

/*----------------------------------------------
 * ln_createAndAdd()
 * create a line and add it to the editor model. Returns true, if successful.
 */
extern int ln_createAndAdd(FTABLE* fp, const char* q, int len, int flags);


/*---------------------------------
 * ft_formatText()
 * Formt the text in the current file.
 *---------------------------------*/
typedef enum { FMT_LEFT = 0, FMT_RIGHT = 1, FMT_CENTER = 2, FMT_JUSTIFIED = 3 } FORMATTING_ALIGNMENT;
extern int ft_formatText(WINFO* wp, int scope, FORMATTING_ALIGNMENT alignment);
/*
 * Can be used to determine whether formatting text is supported for the current file.
 */
extern int ft_supportsFormatting(long long pUnused);

/*
 * Count the number of words in a file. A word is defined by the identifier definition
 * in the grammar.
 */
extern long ft_countWords(FTABLE* fp);

/*
 * Invoke a callback for every view of a editor document model.
 * The callback may return 0 to abort the iteration process.
 * The callback is invoked with the WINFO pointer an an optional parameter
 * passed as the last argument.
 */
extern void ft_forAllViews(FTABLE* fp, int (*callback)(WINFO* wp, void* pParameterPassed), void* parameter);

/*
 * Return the primary view displaying a file - if any.
 */
extern WINFO* ft_getPrimaryView(FTABLE* fp);

/*
 * Connect a view with a file - set the model and add the view as a dependent.
 */
extern void ft_connectViewWithFT(FTABLE* fp, WINFO* wp);

/*
 * Disconnect a view from a file.
 */
extern void ft_disconnectViewFromFT(FTABLE* fp, WINFO* wp);


/**
 * Checks, whether the given window is a view of the file
 */
extern BOOL ft_hasView(FTABLE* fp, WINFO* wp);


/*-------- FILE FLAGS ----------*/

#define	F_INUSE				0x01 		// Mark filestructure as used
#define	F_MODIFIED			0x02 		// File has been modfied
#define	F_NEWFILE			0x04 		// new File to be created
#define	F_APPEND			0x08 		// append to existing File
#define	F_NORMOPEN			0x10 		// normal File
#define	F_SAVEAS			0x20 		// File is saved under new name
#define	F_WFORCED			0x40 		// Forced writing, if not modified
#define	F_ISBACKUPPED		0x80		// Backup already created
#define F_WATCH_LOGFILE		0x100	// watch the file. Update and scroll to end when the file changes. Editing is not possible in this case.
										// used e.g. by the output console.
#define	F_STDLINEAL			0x200		// Use a reasonable standard document descriptor
#define	F_HASWIN			0x400		// file has an associated window
#define	F_NEEDSAUTOSAVE		0x800 		// File is not autosaved
#define	F_HIDDEN			0x1000		// create File hidden
#define	F_RDONLY			0x2000		// File is read only
#define	F_TRANSIENT			0x4000		// File is only opened for transient purposes (macro file or backup file to compare) but 
										// not intended for editing and not to be reopened the next time PKS Edit starts
#define F_NAME_INPUT_REQUIRED 0x8000	// When the users presses Save the Save As dialog is being opened to force the input of a new file name 
#define	F_CHANGEMARK		(F_NEEDSAUTOSAVE|F_MODIFIED)

/*---------- LINEFLAGS ---------*/

#define	LNSAVED				0x01 	// the line has been saved
#define	LNMODIFIED			0x04 	// something in line has been modified
#define LNUNDO_AFTER_SAVE	0x08	// the line has been saved und undo had been applied afterwards
#define	LNNOCR				0x10	// mark as having no CR in the line end - UNIX line end style.
#define	LNINDIRECT			0x20	// indirect flag 
#define	LNNOTERM			0x40	// unterminated line 
#define	LNXMARKED			0x80	// marked for EdCharDelete ...	
#define LN_COMPARE_MODIFIED 0x100	// during compare with another file this line was considered to be modified.
#define LN_COMPARE_ADDED	0x200	// during compare with another file this line was considered to be added.
#define LN_COMPARE_DELETED	0x400	// during compare with another file this line was considered to be deleted.
#define LN_MARKED_FOR_COUNTING 0x800	// lines were marked for being counted using the ft_countlinesStartingFromDirection function.
#define	LN_COMPARE_DIFFERENT	(LN_COMPARE_MODIFIED|LN_COMPARE_ADDED|LN_COMPARE_DELETED) 		
									// this line was either found modified, added or deleted during a compare files

#define LINE_HAS_LINE_END(lp)		((lp->lflg & LNNOTERM) == 0)
#define LINE_HAS_CR(lp)				((lp->lflg & LNNOCR) == 0)

/*---- SIZE OF EXPRESSION BUFFERS -----------------------------*/

#define	ESIZE		560		 		// maximum Expressionsize in standard search expressions 
									// - must have at least a size of 256 bytes + length of pattern for Boyer matches + RE-BUFFER header

#ifndef 	DIM
#define	DIM(x)		(sizeof(x)/sizeof(x[0]))
#endif

#ifndef HIDDEN
#define HIDDEN		0x2
#endif

#define P_GT(p1,p2)		((void *)(p1) > (void *)(p2))
#define P_LT(p1,p2)		((void *)(p1) < (void *)(p2))
#define P_EQ(p1,p2)		((void *)(p1) == (void *)(p2))
#define P_GE(p1,p2)		((void *)(p1) >= (void *)(p2))
#define P_NE(p1,p2)		((void *)(p1) != (void *)(p2))

#endif	/* EDITAB_H */
