/*------------------------*/
/* WINDOW - Flags         */
/*------------------------*/

# ifndef 	_WINFO_H

#include "editorfont.h"
#include "lineoperations.h"

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

typedef struct wininfo WINFO;

typedef int (*RENDER_LINE_FUNCTION)(HDC hdc, int x, int y, WINFO* wp, LINE* lp);

typedef struct wininfo {
	struct wininfo *next;
	int		win_id;
    HWND      edwin_handle,ww_handle,ru_handle,st_handle,lineNumbers_handle;
     
    int       dispmode;				/* flags see edierror.h... */
    int       workmode;
    BOOL		bXtndBlock;			/* Xtending blocks */
    int		scrollflags;
    int		cursaftersearch;
    int		tfillchar;			    /* Tab fill char */
     
    CARET caret; 		/* the caret - to be moved to the view */
    MARK* fmark;
    MARK* blstart, * blend;   	/* Marks for Block Operations			*/
    int  	blcol1, blcol2;		/* column for Blockmarks				*/
    
    char		*statusline;			/* alt. status line */
     char*  win_themeName;
     int       cx,cy,cmx,cmy,cheight,cwidth;
     int       owncursor,ctype;        	/* owncursor and caret - type */

	 EDFONT	fnt;
     HFONT	fnt_handle;
	
	int		vscroll,hscroll;		/* # of lines and columns to scroll */
	int		scroll_dx,			/* distance cursor-window border */
			scroll_dy;			/* for scrolling */
	
    RENDER_LINE_FUNCTION renderFunction;
    long      minln,maxln,mincursln,maxcursln,
              mincol,maxcol,mincurscol,maxcurscol;
     FSTYLE	markstyles[2];			/* text block appearance */
     void	*	fp;
     int		win_state;
} WINFO;

/*--------------------------------------------------------------------------
 * find_setTextSelection()
 *
 * Select a range of text in the file identified by fp.
 */
extern int find_setTextSelection(int rngetype, WINFO* fp, MARK** markstart, MARK** markend);

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
extern int render_singleLineOnDevice(HDC hdc, int x, int y, WINFO* wp, LINE* lp);

/*--------------------------------------------------------------------------
 * render_selectCustomCaret()
 * Select a non-standard caret symbol.
 */
extern void render_selectCustomCaret(int on);

/*------------------------------------------------------------
 * render_updateCustomCaret()
 * When a custom caret is being displayed - re-render it now.
 */
extern void render_updateCustomCaret(WINFO* wp, HDC hdc);

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

/*---------------------------------
 * ft_checkSelection()
 * Check whether a block selection exists.
 *---------------------------------*/
extern int ft_checkSelection(WINFO* fp);

/*---------------------------------
 * ft_checkSelectionWithError()
 * Check whether a block selection exists. If not
 * report an error to the user.
 *---------------------------------*/
extern int 	ft_checkSelectionWithError(WINFO* fp);

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

/*-----------------------------------------------------------
 * ww_setwindowtitle()
 * Update the title of a window.
 */
extern int ww_setwindowtitle(WINFO* wp, void *pUnused);

/*-----------------------------------------------------------
 * ww_savewinstates()
 * Save the state of the currently displayed editor windows for later restore.
 */
extern void ww_savewinstates(void);

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

/*------------------------------------------------------------
 * ww_timer()
 * Trigger a timer action.
 */
extern HWND ww_timer(void);

/*-----------------------------------------------------------
 * ww_winstate()
 */
extern void ww_winstate(int nId, WINDOWPLACEMENT* wsp);

/*------------------------------------------------------------
 * ww_close()
 * Close the passed editor window.
 */
extern int ww_close(WINFO* wp);

/*--------------------------------------------------------------------------
 * ww_getRightMargin()
 * Returns the right margin as current configured the way it should be used for e.g. painting.
 */
extern int ww_getRightMargin(WINFO* fp);

/*---------------------------------
 * ft_formatText()
 * Formt the text in the current file.
 *---------------------------------*/
extern int ft_formatText(WINFO* wp, int scope, int type, int flags);

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
 * Returns the "active" editor window having the focus.
 */
extern WINFO* ww_getCurrentEditorWindow();

extern int 	uc_shiftLinesByIndent(WINFO* fp, long ln, long nlines, int dir);

/*
 * Invoked, when one of the views viewing on this document is closed.
 */
extern void ft_windowClosed(FTABLE* fp, WINFO* wp);

/*------------------------------------------------------------
 * font_createFontWithStyle()
 * create a logical font. If the style is present it is taken
 * from the second parameter, if not, it is taken from the font.
 */
extern HFONT font_createFontWithStyle(EDFONT* pFont, EDFONTSTYLE* pStyle);

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

/*------------------------------------------------------------
 * sl_size()
 * Update the internal sizeds of the WINFO structure (maxrows etc...).
 * Also update slider thumbs for the window.
 */
int sl_size(WINFO* wp);

/*------------------------------------------------------------
 * font_selectSystemFixedFont()
 */
extern void font_selectSystemFixedFont(HDC hdc);

/*------------------------------------------------------------
 * font_selectDefaultEditorFont()
 * select a font and return handle to old Font. Optionally pass a font style (may be NULL)
 */
extern HFONT font_selectDefaultEditorFont(WINFO* wp, HDC hdc, EDFONTSTYLE* pStyle);

/*------------------------------------------------------------
 * font_createFontWithStyle()
 * create a logical font. If the style is present, it is used - otherwise use style info from the font.
 */
extern HFONT font_createFontWithStyle(EDFONT* pFont, EDFONTSTYLE* pStyle);

/*------------------------------------------------------------
 * font_selectStandardFont()
 */
extern void font_selectStandardFont(HWND hwnd, WINFO* wp);

#define FTPOI(wp)		(FTABLE*)(wp->fp)

int ww_hasColumnSelection(WINFO *wp);

#define	_WINFO_H
#endif	_WINFO_H

