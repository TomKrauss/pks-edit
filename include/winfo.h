/*
 * winfo.h
 *
 * PROJEKT: PKS-EDIT for WINDOWS 10
 *
 * purpose: view model for PKS Edit
 *
 * 										created:
 * 										last modified:
 *										author: Tom
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */


# ifndef 	_WINFO_H

#include "syntaxhighlighting.h"
#include "editorfont.h"

/*----- display modes --------------*/

#define	SHOWCONTROL		            0x1
#define	SHOWSTATUS		            0x2
#define	SHOWHEX			            0x4
#define	SHOWRULER			        0x8
#define	SHOWHIDELINES		        0x40
#define	SHOWHIDEHSLIDER	            0x100
#define	SHOWHIDEVSLIDER	            0x200
#define	SHOWSTATUSBOT		        0x400
#define	SHOWLINENUMBERS		        0x800
#define	SHOWCARET_LINE_HIGHLIGHT	0x1000
#define	SHOW_SYNTAX_HIGHLIGHT       0x2000

#define	PLAINCONTROL(mode)	(((mode) & SHOWCONTROL) && !((mode) & SHOWRULER))

/*----- working modes --------------*/

#define	WM_INSERT		0x1
#define	WM_SHOWMATCH	0x2
#define	WM_ABBREV		0x4
#define	WM_AUTOWRAP	    0x8
#define	WM_AUTOINDENT	0x10
#define	O_FIXEDWI		0x40
#define	BLK_LINES		0x80
#define	WM_AUTOFORMAT	0x100
#define	WM_BRINDENT	    0x200
#define	BLK_COLUMN_SELECTION	0x400
#define	WM_OEMMODE	    0x800
#define WM_DELETE_MULTIPLE_SPACES  0x2000

/*--- Fill styles for marked text ----*/

typedef struct tagFSTYLE {
	WORD mode,style;
} FSTYLE;

#define	FS_BMARKED		0
#define	FS_XMARKED		1

/*---- Window-Description --*/

#define WIN_ICONIC			0
#define WIN_OPEN			1

#define SC_THUMBTRACK		0x1
#define SC_CURSORCATCH		0x2

#define CP_POSTOP			0x0 		/* pos cursor on window after */
#define CP_POSMID			0x1 		/* long pos jumps */
#define CP_POSLOW			0x2 		/* " */

typedef struct tagPROPERTY_CHANGE PROPERTY_CHANGE;

typedef struct tagINDENTATION {
    int tabsize;                            // The tab size - if no custom tab positions are specified, multiples of this size define the "stops" in the UI
                                            // Currently tab stops are defined in units of column indices, assuming every column occupies a fixed width.
    unsigned char   ts[256];		        // individual 1st 256 Tabstops
    unsigned char   tbits[MAXLINELEN / 8];	// Bitset Tabstops - allows us to have individual tab sizes (non standard).
} INDENTATION;

typedef int (*RENDER_LINE_FUNCTION)(HDC hdc, int x, int y, WINFO* wp, LINE* lp, long lineNo);

/*--------------------------------------------------------------------------
 * indent_calculateTabStop()
 * calculate the previous tabstop for a column given in screen indentation units.
 * Let's assume, the column is 7 and one has a tab size of 4, then 4 is returned.
 */
extern int 	indent_calculatePreviousTabStop(int col, INDENTATION* pIndentation);

/*--------------------------------------------------------------------------
 * indent_calculateTabStop()
 * calculate next Tabstop
 */
extern int 	indent_calculateTabStop(int col, INDENTATION* pIndentation);

/*--------------------------------------------------------------------------
 * indent_calculateNextTabStop()
 * calculate next tabstop after the given column.
 */
extern int indent_calculateNextTabStop(int col, INDENTATION* l);

/*--------------------------------------------------------------------------
 * indent_toggleTabStop()
 */
extern void indent_toggleTabStop(INDENTATION* indentation, int col);

/*--------------------------------------------------------------------------
 * ww_tabsChanged()
 * The tab configuration has changed. Update our internal data structures
 * and possibly repaint.
 */
int ww_tabsChanged(WINFO* wp, EDIT_CONFIGURATION* lp);

typedef struct tagWINFO {
	struct tagWINFO *next;
	int		win_id;
    HWND    edwin_handle,ww_handle,ru_handle,st_handle,lineNumbers_handle,codecomplete_handle;
     
    int     dispmode;				/* flags see edierror.h... */
    int     charset;                // a special charset to use or 0 for now particular charset.
    int     workmode;
    BOOL	bXtndBlock;			/* Xtending blocks */
    int		scrollflags;
    int		cursaftersearch;
    int		tabDisplayFillCharacter;			    /* Tab fill char */
     
    CARET caret; 		        // the caret - to be moved to the view 
    CARET secondaryCaret;       // A "secondary caret" - used to mark places on the screen like matching brackets.
    int   secondaryCaretWidth;  // the width of the secondary caret (measured in columns)
    MARK* fmark;
    MARK* blstart, * blend;   	/* Marks for Block Operations			*/
    int   blcol1, blcol2;		/* column for Blockmarks				*/
    
    char*  statusline;			/* alt. status line */
    int    cx,cy,cmx,cmy,cheight,cwidth;
    int    ctype;        	        /* caret - type */
    
	int		vscroll,hscroll;		/* # of lines and columns to scroll */
	int		scroll_dx,			/* distance cursor-window border */
			scroll_dy;			/* for scrolling */
    INDENTATION indentation;
    int     lmargin;
    int     rmargin;
    RENDER_LINE_FUNCTION renderFunction;
    HIGHLIGHTER* highlighter;
    LINE*     lpMinln;                  // caching line pointer allowing us, to quickly access the line pointer for the minimum line.
    long      cachedLineIndex;          // line index of the caching line pointer.      
    long      minln,maxln,mincursln,maxcursln,
              mincol,maxcol,mincurscol,maxcurscol;
    long   maxVisibleLineLen;       // The maximum length of a line in columns - used to calculate the size of the horizontal scrollbar
                                    // Is reset to -1 on model changes and recalculated when needed.
     FSTYLE	markstyles[2];			/* text block appearance */
     void	*	fp;
     int		win_state;
     float      zoomFactor;         // The text font size zoom factor.
} WINFO;

/* valid working range types */
#define	RNG_INVALID		   -1
#define	RNG_ONCE			0
#define	RNG_LINE			1
#define	RNG_CHAPTER		    2
#define	RNG_BLOCK			3
#define	RNG_TOCURS		    4
#define	RNG_FROMCURS		5
#define	RNG_GLOBAL		    6
#define	RNG_FREE			7
#define	RNG_BLOCK_LINES		8       // the current selection, but for the purpose of applying an operation one a list of lines (e.g. toggle-comment or shift-lines).
                                    // In this case the last selected line is treated as "not selected", if the column is 0.

/*--------------------------------------------------------------------------
 * find_setTextSelection()
 *
 * Select a range of text in the file identified by fp. For rangeType constants, see above.
 */
extern int find_setTextSelection(WINFO* wp, int rangeType, MARK** markstart, MARK** markend);

/*--------------------------------------------------------------------------
 * find_selectRangeWithMarkers()
 */
extern int find_selectRangeWithMarkers(int rangeType, MARK** mps, MARK** mpe);

/*------------------------------------------------------------
 * render_paintWindow()
 * Performs the actual painting of the window.
 */
extern void render_paintWindow(WINFO* wp);

/*--------------------------------------------------------------------------
 * render_repaintFromLineTo()
 * Send a repaint to the given range of lines.
 */
extern void render_repaintFromLineTo(FTABLE* wp, long min, long max);

/*--------------------------------------------------------------------------
 * render_repaintAllForFile()
 * Send a repaint to all windows / views on a file
 */
extern void render_repaintAllForFile(FTABLE* fp);

/*
 * Repaint the line numbers in the given view.
 */
extern int render_repaintLineNumbers(WINFO* wp, void* pUnused);

/*--------------------------------------------------------------------------
 * render_repaintCurrentFile()
 * Send a repaint to the current file.
 */
extern void render_repaintCurrentFile(void);

/*--------------------------------------------------------------------------
 * render_repaintLinePart()
 * Send a repaint to a part of a line.
 */
extern void render_repaintLinePart(FTABLE* fp, long ln, int col1, int col2);

/*--------------------------------------------------------------------------
 * render_repaintLineRange()
 * Send a repaint to a range of lines specified by line pointer. We assume,
 * the first line is before the last line.
 */
extern void render_repaintLineRange(FTABLE* fp, LINE* lpStart, LINE* lpEnd);

/*--------------------------------------------------------------------------
 * render_repaintLine()
 * Send a repaint to the specific line.
 */
extern void render_repaintLine(FTABLE* fp, LINE* lpWhich);

/*------------------------------------------------------------
 * render_updateCaret()
 * macro_updateSelectedMacro the current caret for the passed editor window (dependening on insert mode
 * etc...)
 */
extern void render_updateCaret(WINFO* wp);

/**
 * A property of our editor document has changed. Update the window appropriately.
 */
extern int ww_documentPropertyChanged(WINFO* wp, PROPERTY_CHANGE* pChange);

/*----------------------------
 * bl_hideSelection()
 * Hide the current selection and optionally
 * remove the selection marker from "marked lines"
 *----------------------------*/
extern int bl_hideSelection(WINFO* wp, int removeLineSelectionFlag);

/* pointer to first and last line to enq	*/
/* first column and last col to enq		*/
/*--------------------------------------------------------------------------
 * bl_undoIntoUnqBuffer()
 * enqueue next Pastebuffer to undolist
 */
extern int bl_undoIntoUnqBuffer(WINFO* wp, LINE* lnfirst, LINE* lnlast, int cfirst, int clast, int blockflg);

/*--------------------------------------------------------------------------
 * bl_delete()
 * Delete the text of the current selection.
 */
extern int bl_delete(WINFO* wp, LINE* lnfirst, LINE* lnlast, int cfirst,
    int clast, int blkflg, int saveintrash);

/*--------------------------------------------------------------------------
 * render_singleLineOnDevice()
 */
extern int render_singleLineOnDevice(HDC hdc, int x, int y, WINFO* wp, LINE* lp, long lineNo);

extern void wt_tcursor(WINFO* wp, int type);

/*------------------------------------------------------------
 * wt_scrollxy()
 * Scroll the current window by a number of lines and columns.
 */
void wt_scrollxy(WINFO* wp, int nlines, int ncolumns);

/*-----------------------------------------------------------
 * ww_redrawAllWindows()
 * Redraw all editor windows. If update is true, not only send
 * a repaint message bu also update by painting right away.
 */
extern void ww_redrawAllWindows(int update);

/*-----------------------------------------------------------
 * ww_applyDisplayProperties()
 * Apply all flags from the document descriptor of the edited document on the
 * window referred to by the passed pointer.
 */
extern void ww_applyDisplayProperties(WINFO* wp);

/*------------------------------------------------------------
 * wt_curpos()
 */
extern void wt_curpos(WINFO* wp, long ln, long col);

/*------------------------------------------------------------
 * wt_deleteline()
 */
extern void wt_deleteline(FTABLE* fp, int caretLine, int additional, int nlines);

/*------------------------------------------------------------
 * wt_insline()
 */
extern void wt_insline(FTABLE* fp, int caretLine, int nlines);

/*--------------------------------------------------------------------------
 * render_repaintAllForFile()
 */
extern void render_repaintAllForFile(FTABLE* fp);

/*--------------------------------------------------------------------------
 * render_repaintCurrentLine()
 * Redraw the line containing the cursor, in the "current active" editor window.
 */
extern void render_repaintCurrentLine(WINFO* wp);

/*
 * Repaint one window.
 */
extern void render_repaintWindow(WINFO* wp);

/*--------------------------------------------------------------------------
 * render_repaintWindowLine()
 * Repaint the given line in the window passed as a parameter.
 */
extern void render_repaintWindowLine(WINFO* wp, long ln);

/**
 * Returns the view num steps from the step - 0 to return the current to level view, 1 to return
 * the one below the top window.
 */
extern WINFO* ww_getWindowFromStack(int num);

/*------------------------------------------------------------
 * ww_setScrollCheckBounds()
 * calculate scrollops checking bounds
 */
extern void ww_setScrollCheckBounds(WINFO* wp);

/*
 * Creates an editor window with the given title, instance count, creation parameter and window
 * placement.
 */
extern HWND ww_createEditWindow(char* pTitle, int nCount, LPVOID lParam, const char* pszDockName);

/*-----------------------------------------------------------
 * ww_setwindowtitle()
 * Update the title of a window.
 */
extern int ww_setwindowtitle(WINFO* wp, void *pUnused);

/*-----------------------------------------------------------
 * ww_getNumberOfOpenWindows()
 * Returns the number of currently open windows.
 */
extern int ww_getNumberOfOpenWindows(void);

/*--------------------------------------------------------------------------
 * ww_getstate()
 * Return the window placement state for a window.
 */
extern void ww_getstate(WINFO* wp, WINDOWPLACEMENT* wsp);

/*------------------------------------------------------------
 * ww_requestFocusInTopWindow()
 * Request focus in the top most window.
 */
extern void ww_requestFocusInTopWindow(void);

/*---------------------------------
 * ww_hasSelection()
 * Check whether a block selection exists in the given window.
 *---------------------------------*/
extern int ww_hasSelection(WINFO* fp);

/*---------------------------------
 * ww_checkSelectionWithError()
 * Check whether a block selection exists. If not
 * report an error to the user.
 *---------------------------------*/
extern int 	ww_checkSelectionWithError(WINFO* fp);

/*
 * Answer 1, if a possible selection in the given window
 * has a "column layout".
 */
extern int ww_isColumnSelectionMode(WINFO* wp);

/*------------------------------------------------------------
 * ww_requestToClose()
 * The user requests to close a window of a file.
 * If the file is modified and cannot be saved or some other error
 * occurs, return 0, otherwise, if the window can be closed return 1.
 */
extern int ww_requestToClose(WINFO* fp);

/*------------------------------------------------------------
 * ww_close()
 * Close the passed editor window.
 */
extern int ww_close(WINFO* wp);

/*
 * Returns the indices of the first selected line and the last selected line.
 * If no selection exists, return 0 and return -1 in pFirstIndex and pLastIndex.
 */
extern int ww_getSelectionLines(WINFO* wp, long* pFirstIndex, long* pLastIndex);

/*
 * Returns the LINE pointer for a line with a given index.
 */
LINE* ww_getMinLine(WINFO* wp, int idx);

/*--------------------------------------------------------------------------
 * ww_getRightMargin()
 * Returns the right margin as current configured the way it should be used for e.g. painting.
 */
extern int ww_getRightMargin(WINFO* fp);

/*
 * Returns the "active" editor window having the focus.
 */
extern WINFO* ww_getCurrentEditorWindow();

extern int 	uc_shiftLinesByIndent(WINFO* fp, long ln, long nlines, int dir);

/*-----------------------------------------------------------
 * ww_register()
 * Register the window classes for PKS edit editor windows.
 */
extern int ww_register(void);

/*------------------------------------------------------------
 * ww_popup()
 * Bring a child to top - if iconized restore.
 */
extern void ww_popup(HWND hwndChild);

/*-----------------------------------------------------------
 * sl_winchanged()
 */
extern void sl_winchanged(WINFO* wp, long dy, long dx);

/*-----------------------------------------------------------
 * sl_scrollwinrange()
 */
int sl_scrollwinrange(WINFO* wp, long* dy, long* dx);

/*-----------------------------------------------------------
 * sl_winchanged()
 */
extern void sl_winchanged(WINFO* wp, long dy, long dx);

/*------------------------------------------------------------
 * sl_moved
 * Invoked, when a scrollbar had been dragged - update the WINFO
 * data structure.
 */
extern int sl_moved(WINFO* wp, long dy, long dx, int cursor_adjust);

/*
 * Increase / decrease the zoom factor of the current window.
 */
extern int ww_zoomWindow(int anIncreaseFactor);

/*------------------------------------------------------------
 * sl_size()
 * Update the internal sizeds of the WINFO structure (maxrows etc...).
 * Also update slider thumbs for the window.
 */
int sl_size(WINFO* wp);

#define FTPOI(wp)		(FTABLE*)(wp->fp)


#define	_WINFO_H
#endif	_WINFO_H

