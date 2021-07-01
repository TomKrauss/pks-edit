/*
 * lineoperations.h
 *
 * PROJEKT: PKS-EDIT for ATARI - GEM
 *
 * maintain list of lines: delete, insert, modify,....
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
#include "editorfont.h"

#define	ED_VERSION	0x20

#define	LN_ALIGN		2		/* minimum size of the linebuffer content area */
#define	FBUFSIZE		24000	/* maximum filebuffer- and linesize */

#define	LINEBUFSIZE		2*FBUFSIZE+160	/* sizeof of _linebuf		 */
#define	MAXLINELEN		12000

#define	MAXSHIFT		36		/* maximum shiftwidth 			 */
#define	RM_DELTA		2		/* delta to automatic right margin  */

/*-------- TYPES --------------*/

typedef struct tagEDIT_CONFIGURATION EDIT_CONFIGURATION;

typedef struct line   {
	struct line* next;
	struct line* prev;
	int  		len; 		/* linelength in chars */
	unsigned char	lflg;		/* line flags */
	unsigned char	attr;		/* attribute byte */
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

typedef struct pastelist {
	struct pastelist	*next;
	int  			id;	/* name of buffer */
	PASTE			pbuf;
} PASTELIST;

/* should ideally extend CARET */
typedef struct tagMARK {
	struct tagMARK* next;
	int  			mchar;		/* ID of mark (char || BLOCKSTART,-END)	*/
	LINE* lm;
	int  			lc;
} MARK;

/* special marker IDs */
#define	CNOMARK			0x00		/* release mark without freeing it */
#define	MARKSTART		0x100		/* cut&paste - start */
#define	MARKEND			0x101		/*	  "	   - end */

#define	MARKSELSTART	0x102
#define	MARKSELEND		0x103

#define	MARKDOT			0x104

/*---------------------------------
 * Undoable edit operations on one o more lines
 */
#define	O_MODIFY			0		/* modify a line */
#define	O_INSERT			1		/* insert a line */
#define	O_DELETE			2		/* EdCharDelete a line */
#define	O_MARK				3		/* mark a textblock */
#define	O_LNORDER			4		/* reorder linelist according to a table */
#define	O_HIDE				5		/* hide a list of lines */
#define	O_UNHIDE			6		/* unhide a list of lines */

/*---------------------------------*/
/* EDIT_CONFIGURATION			*/
/*---------------------------------*/
#define	fnt_name		editFontStyle.faceName
#define	fnt_fgcolor		editFontStyle.fgcolor
#define	fnt_bgcolor		editFontStyle.bgcolor
#define	fnt_charset		editFontStyle.charset

#if defined(_WINUSER_)

typedef struct tagEDIT_CONFIGURATION {
	struct tagEDIT_CONFIGURATION* next;
	unsigned char  modename[16];	/* configuration name */
	char		name[32];			/* base name of the file defining this configuration */
	int			id;					/* # ID for context check */
	int			lmargin, rmargin;
	int			tabsize, shiftwidth;
	int			nl, nl2, cr;		/* Lineends */
	int			dispmode;			/* show control... */
	int			workmode;			/* watch brackets, abbrev... */
	char		t1, fillc;			/* Tabulator Character (fill character) */
	char		u2lset[32];			/* wordset and u2l ("abc=xyz") */
	int			tabulatorSizes[32];	/* arbitrary tab stops - allowing us to have have tabs at positions 2, 5, 9, 15, ...*/
	unsigned char 	tbits[MAXLINELEN / 8];	/* Bitset real Tabstops */
	unsigned char	statusline[60];	/* the special status */
	WINDOWPLACEMENT	placement;		/* for windows with fixed size */
	EDTEXTSTYLE*	editFontStyle;			/* font */
	char			editFontStyleName[16];	/* Name of the font style to use*/
	char		   backupExtension[10];		/* Backup extension */
	unsigned char  tagtag[12];		/* private tag tag */
	unsigned char	creationMacroName[24];	/* "Vorlage" Makro on creation */
	unsigned char	closingMacroName[24];	/* makro to execute before closing */
	int				scrollflags;		/* thumbtrack.. */
	int				cursaftersearch;	/* CP_POSTOP, CP_POSLOW, ... */
	int				hscroll;			/* scroll n columns */
	int				vscroll;			/* scroll n Lines */
	int				scroll_dy;			/* scroll on dist dy to screen */
	unsigned char	ts[256];		/* fast access 1st 256 Tabstops */
} EDIT_CONFIGURATION;

/*--------------------------------------------------------------------------
 * doctypes_calculateTabStop()
 * calculate next Tabstop
 */
extern int doctypes_calculateTabStop(int col, EDIT_CONFIGURATION* lp);

/*--------------------------------------------------------------------------
 * doctypes_calculateNextTabStop()
 * calculate next tabstop after the given column.
 */
extern int doctypes_calculateNextTabStop(int col, EDIT_CONFIGURATION* l);

/*--------------------------------------------------------------------------
 * Creates the default attributes for editing a document. The returned structure
 * must be freed, when done using it.
 */
extern EDIT_CONFIGURATION* doctypes_createDefaultDocumentTypeDescriptor();

/*--------------------------------------------------------------------------
 * doctypes_initDocumentTypeDescriptor()
 * Initialize a document type descriptor with the proper tabsize settings.
 */
extern void doctypes_initDocumentTypeDescriptor(EDIT_CONFIGURATION* lp, int ts);

#endif

extern char 	bittab[];

#define	TABTHERE(lin,i)		(lin->tbits[i >> 3] &  bittab[i & 07])
#define	TABPLACE(lin,i)		(lin)->tbits[i >> 3] |= bittab[i & 07]
#define	TABCLEAR(lin,i)		(lin)->tbits[i >> 3] &= (~bittab[i & 07])

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
	LINE* linePointer;		/* the current line */
	int   offset;			/* current offset in linebuffer memory */
	int	  virtualOffset;	/* the virtual offset into the linebuffer. That is used to have the cursor try to stay in the same column as it is moved 
							   up and down the screen and will eventually pass by "short lines" forcing it to jump to smaller column positions. */
	long ln;				/* zero based line index into the list of lines */
	long  col;				/* The column index in screen coordinates (not into the buffer) */
} CARET;

typedef struct ftable {
	struct ftable *next;
	char 	fname[256];
	long 	nlines;
	long 	lastln,lastcol;	/* start of previous search .. */
	int  	lastmodoffs;		/* last offset for last modification */
	int  	flags;
	int		longLinesSplit;	/* Count of all long lines, which were split during read, as the lines were too long */
	LINE 	*tln;			/* Pointer to current edited line */	    
	LINE 	*firstl,			/* first line */
			*lastl;			/* last line */
	LINE* lpReadPointer;	/* used during read operations temporarily */
	EDIT_CONFIGURATION	*documentDescriptor;
	HIDDENP	views;			/* the list of our views */
	HIDDENP	undo;
	long 	as_time;			/* next time for AUTOSAVE */
	EDTIME	ti_modified;		/* last modification time */
	EDTIME	ti_created;		/* creation time */
	EDTIME  ti_saved;			/* last save time */
	int		lockFd;			/* Filedescriptor for locking - <= 0 if none */
} FTABLE;

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
extern int lnjoin_lines(FTABLE *fp);
extern LINE *ln_goto(FTABLE *fp,long l);
extern LINE *ln_relative(LINE *cl, long l);
extern long ln_cnt(LINE *lps,LINE *lpe);
extern LINE *ln_findbit(LINE *lp,int bit);
extern void ln_replace(FTABLE *fp,LINE *oln,LINE *nl);

/*
 * Returns the current active document. Should not be used any more to often
 */
extern FTABLE* ft_getCurrentDocument();

/*
 * Returns the current error document. Should not be used any more to often
 */
extern FTABLE* ft_getCurrentDocument();

/*------------------------------------------------------------
 * ft_activateWindowOfFileNamed()
 * Activate the window of the file named...
 */
extern int ft_activateWindowOfFileNamed(char* fn);

/*---------------------------------
 * ft_cutMarkedLines()
 * Cut out the lines which which have a line marker flag.
 * The cut operation is one of the MLN_... constants defined for files.
 *---------------------------------*/
void ft_cutMarkedLines(FTABLE* fp, int op);

/*------------------------------------------------------------
 * ft_abandonFile()
 * Discard changes in a file and re-read.
 */
extern int ft_abandonFile(FTABLE* fp, EDIT_CONFIGURATION* linp);

/*------------------------------------------------------------
 * The current active file (window of active file) has changed.
 */
extern int ft_currentFileChanged(FTABLE* fp);

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


/*------------------------------------------------------------
 * ft_openFileWithoutFileselector()
 * Open a file with a file name and jump into a line. Place the window to
 * open as defined in the param wsp.
 */
int ft_openFileWithoutFileselector(char* fn, long line, WINDOWPLACEMENT* wsp);

/*---------------------------------
 * ln_addFlag()
 *
 * Add a flag to all lines between lpstart and lpend.
 */
extern void ln_addFlag(LINE* lpstart, LINE* lpend, int flg);

/*---------------------------------
 * ln_changeFlag()
 *
 * Add / remove flags from multiple lines - all lines having an expected flag
 * are considered.
 */
extern void ln_changeFlag(LINE* lpstart, LINE* lpend, int flagsearch, int flagmark, int set);

/*---------------------------------
 * ln_removeFlag()
 *
 * Remove a flag from all lines between lpstart and lpend.
 *---------------------------------*/
extern void ln_removeFlag(LINE* lpstart, LINE* lpend, int flg);

extern char* ft_visiblename(FTABLE* fp);
extern void ft_checkForChangedFiles(void);
/* do an autosave */
extern int ft_triggerAutosaveAllFiles(void);
extern void ft_deleteautosave(FTABLE* fp);
extern void ft_destroy(FTABLE* fp);
extern FTABLE* ft_new(void);

/*------------------------------*/
/* ft_initializeReadWriteBuffers()			  */
/*------------------------------*/
extern BOOL ft_initializeReadWriteBuffers(void);


/*---------------------------------
 * ft_readDocumentFromFile()
 * Standard implementation to read a file in PKS Edit given the file descriptor,
 * a callback method to invoked for each line read and an optional parameter (typically, but not neccessarily the filepointer itself) to
 * be parsed as the first argument to the callback.
 *---------------------------------*/
extern int ft_readDocumentFromFile(int fd, unsigned char* (*lineExtractedCallback)(void*, EDIT_CONFIGURATION*, unsigned char*, unsigned char*), void* par);

/*--------------------------------------*/
/* ft_readfile()						*/
/*--------------------------------------*/
extern int ft_readfile(FTABLE* fp, EDIT_CONFIGURATION* documentDescriptor);

/*--------------------------------------*/
/* ft_writefileMode() 					*/
/*--------------------------------------*/
extern int ft_writefileMode(FTABLE* fp, int quiet);

/*---------------------------------*/
/* ft_writeFileWithAlternateName()			*/
/*---------------------------------*/
extern int ft_writeFileWithAlternateName(FTABLE* fp);

/*---------------------------------*/
/* ft_writefileAsWithFlags()
/*---------------------------------*/
extern int ft_writefileAsWithFlags(FTABLE* fp, char* fn, int flags);

/*---------------------------------*/
/* read a file, which is never	*/
/* been redrawn on screen		*/
/* Macrofiles, ...				*/
/*---------------------------------*/
extern int ft_readfileWithOptions(FTABLE* fp, char* fn, int linflag);

/*---------------------------------*/
/* ft_writeFileAndClose()				*/
/*---------------------------------*/
extern int ft_writeFileAndClose(FTABLE* fp, char* name, int flags);

extern void ft_checkForChangedFiles(void);

/*---------------------------------*/
/* ft_triggerAutosaveAllFiles()					*/
/* do an autosave				*/
/*---------------------------------*/
extern int ft_triggerAutosaveAllFiles(void);

/*---------------------------------
 * ft_saveWindowStates()
 * Save the state of the currently opened windows in the config for restore.
 ---------------------------------*/
extern void ft_saveWindowStates(void);

/*---------------------------------*/
/* ft_restorePreviouslyOpenedWindows()					*/
/*---------------------------------*/
extern int ft_restorePreviouslyOpenedWindows(void);

/*------------------------------------------------------------
 * ft_selectWindowWithId()
 * Select and activate the window with the given window id.
 */
extern int ft_selectWindowWithId(int winid, BOOL bPopup);

/*------------------------------------------------------------
 * EdSelectWindow()
 */
int EdSelectWindow(int winid);

extern char* ft_visiblename(FTABLE* fp);
/*------------------------------------------------------------
 * ft_size()
 * calculate current file size
 */
extern long ft_size(FTABLE* fp);
/*------------------------------------------------------------
 * ft_fpbyname()
 * Find a filebuffer given the name of the file.
 */
extern FTABLE* ft_fpbyname(char* fn);
/*------------------------------------------------------------
 * ft_editing()
 * Answer true, if we are editing the file named fn.
 */
extern int ft_editing(char* fn);

/*--------------------------------------------------------------------------
 * ft_sortFile()
 * Sort the current file / document.
 */
int ft_sortFile(FTABLE* fp, int scope, char* fs, char* keys, char* sel, int sortflags);

/*--------------------------------------------------------------------------
 * ft_expandTabsWithSpaces()
 * Expand tabs and replace with spaces.
 * Return the number of expanded tabs.
 */
extern int ft_expandTabsWithSpaces(LINE* lp, long* nt);

/*------------------------------------------------------------
 * ft_bufdestroy().
 * Release all resources associated with a file.
 */
extern void ft_bufdestroy(FTABLE* fp);

/**
 * Make the passed file the "current error file" - which can be used by clicking on
 * lines displayed in that file to navigate to positions (compiler errors etc...).
 */
extern void ft_setCurrentErrorDocument(FTABLE* fp);

/*------------------------------------------------------------
 * ft_activateWindowOfFileNamed()
 * Activate the window of the file with the given name.
 */
extern int ft_activateWindowOfFileNamed(char* fn);

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

/*--------------------------------------------------------------------------
 * ln_lineIsEmpty()
 */
extern int ln_lineIsEmpty(LINE* lp);

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

extern void macro_recordFunction(FTABLE* fp, int p);

/*--------------------------------------------------------------------------
 * string_formatDate()
 */
extern void string_formatDate(char* szDbuf, EDTIME* ltime);

/*------------------------------------------------------------
 * EdEditFile()
 * Edit a file with a filename and with potential flags.
 */
extern int EdEditFile(long editflags, char* filename);

/*------------------------------------------------------------
 * EdSaveFile()
 * Save the current editor window. Depending on the passed options
 * this may require the user to enter a file name or to do nothing (if the file
 * is unchanged) etc...
 */
extern int EdSaveFile(int flg);

/*--------------------------------------------------------------------------
 * EdFileAbandon()
 * Cancel all changes in he current file.
 */
extern int EdFileAbandon(void);

/*--------------------------------------------------------------------------
 * EdLineSplit()
 * do cr+lf-Actions
 */
extern int EdLineSplit(int flags);

/*
 * Typical data structure for a linked list.
 */
typedef struct linkedList {
	struct linkedList* next;
	char name[4];
} LINKED_LIST;


/*--------------------------------------------------------------------------
 * ln_insertIndent()
 * Insert white space characters at the beginning of the passed line so
 * we fill up the space at the beginning of the line, so that the cursor
 * would be visible in the passed column. Return the number of characters
 * inserted parameter &inserted.
 */
extern LINE* ln_insertIndent(FTABLE* fp, LINE* lp, int col, int* inserted);

/**
 * Find the index of an element in a linked list or return -1 if not found.
 */
extern long ll_indexOf(LINKED_LIST* pHead, LINKED_LIST* lp);

/*--------------------------------------------------------------------------
 * ll_moveElementToFront()
 * put an element to the top of the linked list
 */
extern int ll_moveElementToFront(LINKED_LIST** pointerLinkedList, void* elem);

/*--------------------------------------------------------------------------
 * ll_insert()
 * insert an element with the given size into a linked list. The head of the list
 * will be updated to point to the new element and existing elements will be pushed back.
 */
extern LINKED_LIST* ll_insert(LINKED_LIST** pointerLinkedList, long size);

/**
 * Add an element to the end of a linked list. If the linked list does not yet exist,
 * the HEAD pointer is updated to point to the new element.
 */
extern void ll_add(LINKED_LIST** pointerLinkedList, LINKED_LIST* pElement);

/*--------------------------------------------------------------------------
 * ll_destroy()
 * destroy a linked list. Pass a pointer to the head of the linked list and
 * an additional destruction function for one element in the list.
 */
extern void ll_destroy(LINKED_LIST** pointerLinkedList, int (*destroy)(void* elem));

/*--------------------------------------------------------------------------
 * ll_delete()
 * delete an element in a linked list.Return 1 if the element was successfully deleted.
 */
extern int ll_delete(LINKED_LIST** pointerLinkedList, void* element);

/**
 * Return an element from the linked list at the given index or NULL
 * if no element exists for the index.
 */
extern LINKED_LIST* ll_at(LINKED_LIST* head, int idx);

/*--------------------------------------------------------------------------
 * ll_size()
 * Count the elements in a linked list.
 */
extern long ll_size(LINKED_LIST* linkedList);

/*--------------------------------------------------------------------------
 * ll_find()
 *
 * Find an element in a linked list, with a given name.
 */
extern LINKED_LIST* ll_find(LINKED_LIST* linkedList, char* name);

/*-----------------------------------------
 * ln_createMultipleLinesUsingSeparators()
 * Split a passed buffer (end is marked by pend) with given line separator
 * characters t1 and t2. If cr is passed (not 0), the cr character
 * is handled as such.
 */
extern unsigned char* ln_createMultipleLinesUsingSeparators(FTABLE* fp, unsigned char* p, unsigned char* pend, int t1, int t2, int cr);

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
extern unsigned char* ln_createFromBuffer(FTABLE* fp, EDIT_CONFIGURATION* documentDescriptor, unsigned char* p, unsigned char* pend);

/*----------------------------------------------
 * ln_createAndAdd()
 * create a line and add it to the editor model. Returns true, if successful.
 */
extern int ln_createAndAdd(FTABLE* fp, char* q, int len, int flags);

/*--------------------------------------------------------------------------
 * ln_createAndAddSimple()
 * Similar to ln_createAndAdd() - default size and default flags.
 */
extern int ln_createAndAddSimple(FTABLE* fp, char* b);

/*-------- FILE FLAGS ----------*/

#define	F_INUSE			0x01 	/* Mark filestructure as used	  */ 
#define	F_MODIFIED			0x02 	/* File has been modfied 	  */
#define	F_NEWFILE			0x04 	/* new File to be created	  */
#define	F_APPEND			0x08 	/* append to existing File	  */
#define	F_NORMOPEN			0x10 	/* normal File 			  */
#define	F_SAVEAS			0x20 	/* File is saved under new name */
#define	F_WFORCED			0x40 	/* Forced writing, if not mod.  */
#define	F_ISBACKUPPED		0x80		/* Backup already created	  */
#define	F_STDLINEAL			0x200	/* Use a reasonable standard document descriptor */
#define	F_HASWIN			0x400	/* file has an associated window */
#define	F_NEEDSAUTOSAVE		0x800 	/* File is not autosaved 	  */
#define	F_HIDDEN			0x1000	/* create File hidden		  */
#define	F_RDONLY			0x2000	/* File is read only		  */
#define	F_ISHELPFILE		0x4000	/* File is a Help Window File   */
#define F_NAME_INPUT_REQUIRED 0x8000 /* When the users presses Save the Save As dialog is being opened to force the input of a new file name */
#define	F_CHANGEMARK		(F_NEEDSAUTOSAVE|F_MODIFIED)

/*---------- LINEFLAGS ---------*/

#define	LNDIFFMARK			0x02 	/* Mark for last EdFilesCompare action */
#define	LNREPLACED			0x04 	/* something in line has been replaced */
#define	LNNOCR				0x10		/* mark not chapter lines */
#define	LNINDIRECT			0x20		/* indirect flag */
#define	LNNOTERM			0x40		/* unterminated line */
#define	LNXMARKED			0x80		/* marked for EdCharDelete ...	*/

#define LINE_HAS_LINE_END(lp)		((lp->lflg & LNNOTERM) == 0)
#define LINE_HAS_CR(lp)				((lp->lflg & LNNOCR) == 0)

/*---- SIZE OF EXPRESSION BUFFERS -----------------------------*/

#define	ESIZE		200		 	/* maximum Expressionsize		 */

#ifndef 	DIM
#define	DIM(x)		(sizeof(x)/sizeof(x[0]))
#endif

#ifndef HIDDEN
#define HIDDEN		0x2
#endif

/* far modell pointer compare ... bargh..! */
#define P_GT(p1,p2)		((void EDFAR*)(p1) > (void EDFAR*)(p2))
#define P_LT(p1,p2)		((void EDFAR*)(p1) < (void EDFAR*)(p2))
#define P_EQ(p1,p2)		((void EDFAR*)(p1) == (void EDFAR*)(p2))
#define P_GE(p1,p2)		((void EDFAR*)(p1) >= (void EDFAR*)(p2))
#define P_NE(p1,p2)		((void EDFAR*)(p1) != (void EDFAR*)(p2))

#endif	/* EDITAB_H */
