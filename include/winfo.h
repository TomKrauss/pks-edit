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
#define	SHOWCARET_PRESERVE_COLUMN	0x2
#define	SHOWHEX			            0x4
#define	SHOWRULER			        0x8
#define	SHOWHIDEHSLIDER	            0x100
#define	SHOWHIDEVSLIDER	            0x200
#define SHOWWYSIWYG                0x400
#define	SHOWLINENUMBERS		        0x800
#define	SHOWCARET_LINE_HIGHLIGHT	0x1000
#define	SHOW_SYNTAX_HIGHLIGHT       0x2000

#define	PLAINCONTROL(mode)	(((mode) & SHOWCONTROL) && !((mode) & SHOWRULER))

/*----- working modes --------------*/

#define	WM_INSERT		    0x1
#define	WM_SHOWMATCH	    0x2
#define	WM_ABBREV		    0x4
#define	WM_AUTOWRAP	        0x8
#define	WM_AUTOINDENT	    0x10
#define	WM_WATCH_LOGFILE    0x40
#define	WM_LINE_SELECTION	0x80
#define	WM_AUTOFORMAT	    0x100
#define	WM_COLUMN_SELECTION	0x400
#define	WM_OEMMODE	        0x800
#define WM_DELETE_MULTIPLE_SPACES  0x2000   // optimize delete and backspace operations to handle spaces as if they were tabs.
#define WM_STICKY			0x4000		    // window should not be automatically closed, when the number 
                                            // of open windows exceeds the maximum configured.

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

typedef struct tagNAVIGATION_INFO_PARSE_RESULT {
    char* ni_reference;                     // The file/link specification to open
    long  ni_lineNumber;                    // The line number to jump to
    char* ni_wp;                            // The identifier for a dock, in which the window to open should be placed.
    int ni_displayMode;                     // The requested display mode or -1 if no special display mode is requested.
    char* ni_anchor;                        // An optional anchor to jump to
} NAVIGATION_INFO_PARSE_RESULT;

typedef struct tagTHEME_DATA THEME_DATA;

typedef struct tagRENDER_CONTEXT {
    HDC     rc_hdc;
    BOOL    rc_printing;
    WINFO*  rc_wp;
    THEME_DATA*  rc_theme;
} RENDER_CONTEXT;

typedef int (*RENDER_LINE_FUNCTION)(RENDER_CONTEXT* pRC, int x, int y, LINE* lp, long lineNo);
typedef void (*RENDER_PAGE_FUNCTION)(RENDER_CONTEXT* pRC, RECT* pBoundingRect, HBRUSH hBrushBg, int y);
/*
 * Input parameters are a pointer to view, to the line (in screen coordinates) to move to,
 * the line buffer offset, a pointer to the screen column. If 'updateVirtualOffset' is 1,
 * the virtual column on the screen should be updated. 'xDelta' is a hint defining, whether
 * the caret was moved horizontally to left or right or not.
 */
typedef int (*PLACE_CARET_FUNCTION)(WINFO* wp, long* ln, long offset, long* col, int updateVirtualOffset, int xDelta);

/*
 * Callback implemented by renderers to answer, whether a special "display mode" (SHOW...) flag is supported by this renderer.
 * This will allow renderers to hide the line number area or the ruler area.
 */
typedef int (*RENDERER_SUPPORTS_MODE)(int nMode);

typedef long (*CALCULATE_MAX_LINE_FUNCTION)(WINFO* wp);

/*
 * Invoked, when the cursor is positioned using slider or mouse to update the
 * caret position. 
 */
typedef int (*CARET_MOUSE_CLICKED_FUNCTION)(WINFO* wp, long* ln, long* col, int updateVirtualColumn);

typedef struct line LINE;

typedef long (*CALCULATE_MAX_COL_FUNCTION)(WINFO* wp, long ln, LINE* lp);

typedef struct tagINTERNAL_BUFFER_POS {
    LINE* ibp_lp;                       // The line pointer 
    int   ibp_lineOffset;               // The offset into the buffer in the line
    int   ibp_logicalColumnInLine;      // The "logical" column on the screen.
    long  ibp_byteOffset;               // The "byte offset" of the position relative to the beginning of the file.
} INTERNAL_BUFFER_POS;
/*
 * Calculate the logical offset into the "current" line displayed on the screen in character positions.
 * If the screen position can be successfully converted, 1 is returned, if not, 0 is returned.
 */
typedef int (*SCREEN_OFFSET_TO_BUFFER_FUNCTION)(WINFO* wp, long screenLn, long screenCol, INTERNAL_BUFFER_POS* pPosition);

typedef void* (*RENDERER_CREATE)(WINFO* wp);

typedef void (*RENDERER_DESTROY)(WINFO* wp);

typedef void (*RENDERER_SCROLL)(WINFO* wp, int nlines, int ncolumns);

typedef int (*RENDERER_SCROLL_SET_BOUNDS)(WINFO* wp);

typedef void (*RENDERER_CARET_UPDATE_UI)(WINFO* wp, int* pCX, int* pCY, int* pWidth, int* pHeight);

typedef void (*RENDERER_HIT_TEST)(WINFO* wp, int cX, int cY, long* pLine, long* pCol);

typedef void (*RENDERER_NAVIGATE_ANCHOR)(WINFO* wp, const char* pszAnchor);

typedef void (*RENDERER_WINDOW_SIZE_CHANGED)(WINFO* wp);

typedef void (*RENDERER_MOUSEMOVE)(WINFO* wp, int x, int y);

typedef BOOL(*RENDERER_FIND_LINK)(WINFO* wp, char* pszDest, size_t nMaxChars, NAVIGATION_INFO_PARSE_RESULT* pResult);

typedef struct tagRENDERER {
    const RENDER_LINE_FUNCTION r_renderLine;
    const RENDER_PAGE_FUNCTION r_renderPage;
    const PLACE_CARET_FUNCTION r_placeCaret;
    const CALCULATE_MAX_LINE_FUNCTION r_calculateMaxLine;
    const CALCULATE_MAX_LINE_FUNCTION r_calculateMaxScreenColumn; // Returns the maximum logical screen column position over all lines displayed
    const CALCULATE_MAX_COL_FUNCTION r_calculateMaxColumn;        // Returns the maximum "internal" buffer position.
    const CARET_MOUSE_CLICKED_FUNCTION r_placeCaretAfterClick;
    const SCREEN_OFFSET_TO_BUFFER_FUNCTION r_screenToBuffer;      // Responsible for translating logical screen coordinates (line and column on the screen) to buffer pointers.
    const RENDERER_CREATE r_create;                               // Called, when the renderer is created. Returns the internal data structure r_data. May be null.
    const RENDERER_DESTROY r_destroy;                             // Called when the renderer is destroy. Frees the internal data structure r_data. If null, free is called by default to release the structure.
    const RENDERER_SCROLL r_scroll;
    const RENDERER_WINDOW_SIZE_CHANGED r_windowSizeChanged;       // The window size has changed: adjust the min and max lines and columns visible
    const RENDERER_SCROLL_SET_BOUNDS r_adjustScrollBounds;        // Set the new minimum and maximum line and columns used when navigating the caret if the 
                                                                  // caret does not fit in the current window.
    const RENDERER_CARET_UPDATE_UI r_updateCaretUI;               // Set x and y coordinates of the caret depending on the line and column.
    const RENDERER_SUPPORTS_MODE r_supportsMode;
    const RENDERER_HIT_TEST r_hitTest;                            // Return the info about a clicked element.
    const BOOL r_canEdit;                                         // whether this renderer supports editing.
    const void (*r_modelChanged)(WINFO* wp, MODEL_CHANGE* pMC);   // The method to invoke, when the model changes.
    const char* r_context;                                        // Optional action context to allow for specific input actions for this renderer. Maybe null.
    const RENDERER_FIND_LINK r_findLink;                          // Optional callback to find a link at the "current" caret position.
    const RENDERER_NAVIGATE_ANCHOR r_navigateAnchor;              // Optional callback to navigate to an "anchor" specification
    const RENDERER_MOUSEMOVE r_mouseMove;                         // Optional callback invoked during mouse move.
} RENDERER;

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

/*
 * Used to "synchronize" two files to be compared with each other. 
 */
typedef struct tagCOMPARISON_LINK {
    struct tagWINFO* cl_wpLeft;
    struct tagWINFO* cl_wpRight;
    BOOL cl_synchronizeCaret;
} COMPARISON_LINK;

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
     
    int     dispmode;				// Display options: see edierror.h...
    int     charset;                // a special charset to use or 0 for now particular charset.
    int     workmode;
    BOOL	bXtndBlock;			    // we are currently in selection extension mode
    int		scrollflags;
    int		cursaftersearch;
    int		tabDisplayFillCharacter;// Tab fill char
     
    CARET caret; 		            // the caret - to be moved to the view 
    CARET secondaryCaret;           // A "secondary caret" - used to mark places on the screen like matching brackets.
    int   secondaryCaretWidth;      // the width of the secondary caret (measured in columns)
    MARK* fmark;
    MARK* blstart, * blend;   	    // Marks for Block Operations
    int   blcol1, blcol2;		    // column for Blockmarks
    
    char*  statusline;			    // status line format, if a special format  should be used - otherwise NULL.
    int    cx,cy,cmx,cmy,cheight,cwidth;
    int    ctype;        	        // caret - type
    
	int		vscroll,hscroll;		// # of lines and columns to scroll
	int		scroll_dx,			    // distance cursor-window border
			scroll_dy;			    // for scrolling
    INDENTATION indentation;
    int     lmargin;
    int     rmargin;
    RENDERER* renderer;
    HIGHLIGHTER* highlighter;
    LINE*     lpMinln;              // caching line pointer allowing us, to quickly access the line pointer for the minimum line.
    long      cachedLineIndex;      // line index of the caching line pointer.      
    long      minln,maxln,mincursln,maxcursln,
              mincol,maxcol,mincurscol,maxcurscol;
    long   maxVisibleLineLen;       // The maximum length of a line in columns - used to calculate the size of the horizontal scrollbar
                                    // Is reset to -1 on model changes and recalculated when needed.
     FSTYLE	markstyles[2];			// text block appearance
     void	*	fp;
     float      zoomFactor;         // The text font size zoom factor.
     COMPARISON_LINK* comparisonLink;
     void* r_data;                  // optionally used by the renderer for internal book keeping
     char actionContext[32];        // allows us to have window specific key-, mouse- and other action bindings.
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
 * render_repaintLineRangeWindow()
 * Send a repaint to a range of lines specified by line pointer to one window only.
 */
extern void render_repaintLineRangeWindow(WINFO* wp, LINE* lpStart, LINE* lpEnd);

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

/*
 * The display / workmode of a window has changed - update appropriately.
 */
extern void ww_modeChanged(WINFO* wp);

/*
 * Force a window to use a new display mode.
 */
extern void ww_changeDisplayMode(WINFO* wp, int nNewDisplayMode);

/*----------------------------
 * bl_hideSelection()
 * Hide the current selection and optionally
 * remove the selection marker from "marked lines"
 *----------------------------*/
extern int bl_hideSelection(WINFO* wp, int removeLineSelectionFlag);

/*--------------------------------------------------------------------------
 * bl_delete()
 * Delete the text of the current selection.
 */
extern int bl_delete(WINFO* wp, LINE* lnfirst, LINE* lnlast, int cfirst,
    int clast, int blkflg, int bSave);

/*--------------------------------------------------------------------------
 * render_singleLineOnDevice()
 */
extern int render_singleLineOnDevice(RENDER_CONTEXT* pRC, int x, int y, LINE* lp, long lineNo);

/*
 * Render the current window in normal ascii / code mode assuming a fixed character spacing font.
 */
void render_asciiMode(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y);

/*
 * Render the current window in hexadecimal display.
 */
void render_hexMode(RENDER_CONTEXT* pCtx, RECT* pClip, HBRUSH hBrushBg, int y);

extern void wt_setCaretVisibility(WINFO* wp, int type);

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

/*------------------------------------------------------------
 * Check, whether the current caret lies within the visible range
 * in the window and if not - adjust the bounds given by wp->minln and wp->maxln.
 * Return true, if the bounds were adjusted.
 */
extern int render_adjustScrollBounds(WINFO* wp);

/*------------------------------------------------------------
 * render_makeCaretVisible()
 * scroll the window, so the given caret is visible.
 */
int render_makeCaretVisible(WINFO* wp, CARET* pCaret);

/*
 * Creates an editor window with the given title, instance count, creation parameter and window
 * placement.
 */
extern HWND ww_createEditWindow(char* pTitle, LPVOID lParam, const char* pszDockName);

/*-----------------------------------------------------------
 * ww_setwindowtitle()
 * Update the title of a window.
 */
extern int ww_setwindowtitle(WINFO* wp, BOOL bRepaint);

/*
 * Called initially once after the main frame has been shown to adjust the scroll bounds
 * of all windows.
 */
extern void ww_adjustWindowSizes();

/*-----------------------------------------------------------
 * ww_getNumberOfOpenWindows()
 * Returns the number of currently open windows.
 */
extern int ww_getNumberOfOpenWindows(void);

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

/*
 * Can be used to test, whether a selection contains a caret position given in
 * screen line and column positions.
 */
extern int bl_selectionContainsLogicalPoint(WINFO* wp, long ln, long col);

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

/*
 * Returns the WINFO data structure for a HWND.
 */
extern WINFO* ww_getWinfoForHwnd(HWND hwnd);

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

/*
 * Set a fixed zoom factor for a window.
 */
extern void ww_setZoom(WINFO* wp, float newFactor);

/*
 * Connect two windows with a comparison link. This is used to allow for synchronized scrolling
 * etc to provide a consistent view on the differences of two files.
 */
extern void ww_connectWithComparisonLink(WINFO* wp1, WINFO* wp2);

/*
 * One window of two windows being compared with each other is closed. Perform the proper cleanup.
 */
extern void ww_releaseComparisonLink(WINFO* wp, BOOL bDetachSource);

/*
 * Register a renderer.
 */
extern void ww_registerRenderer(const char* pszName, RENDERER* pRenderer);

/*
 * Final cleanup of the editor window sub-system.
 */
extern void ww_destroyAll();

/*------------------------------------------------------------
 * sl_size()
 * Update the internal sizeds of the WINFO structure (maxrows etc...).
 * Also update slider thumbs for the window.
 */
extern int sl_size(WINFO* wp);

#define FTPOI(wp)		(FTABLE*)(wp->fp)


#define	_WINFO_H
#endif	_WINFO_H

