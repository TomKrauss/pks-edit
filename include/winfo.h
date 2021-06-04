/*------------------------*/
/* WINDOW - Flags         */
/*------------------------*/

# ifndef 	_WINFO_H

#include "editorfont.h"
#include "lineoperations.h"

typedef struct tagGRECT {
	int g_x,g_y,g_w,g_h;
} GRECT;

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

typedef struct wininfo {
	struct wininfo *next;
	int		win_id;
    HWND      edwin_handle,ww_handle,ru_handle,st_handle,lineNumbers_handle;
     
     int       dispmode;				/* flags see edierror.h... */
     int       workmode;
     BOOL		bXtndBlock;			/* Xtending blocks */
     int		scrollflags;
     int		cursaftersearch;
     int		tfillchar;			/* Tab fill char */
     
     char		*statusline;			/* alt. status line */

     int       cx,cy,cmx,cmy,cheight,cwidth;
     int       owncursor,ctype;        	/* owncursor and caret - type */

	EDFONT	fnt;
	HFONT	fnt_handle;     
	
	int		vscroll,hscroll;		/* # of lines and columns to scroll */
	int		scroll_dx,			/* distance cursor-window border */
			scroll_dy;			/* for scrolling */
			
     long      ln,minln,maxln,mincursln,maxcursln,
               col,mincol,maxcol,mincurscol,maxcurscol;
     GRECT     workarea;
     FSTYLE	markstyles[2];			/* text block appearance */
     void	*	fp;
     int		win_state;
} WINFO;


/*--------------------------------------------------------------------------
 * render_sendRedrawToWindow()
 */
extern void render_sendRedrawToWindow(HWND hwnd);

/*------------------------------------------------------------
 * render_paintWindow()
 */
extern void render_paintWindow(WINFO* wp);

/*--------------------------------------------------------------------------
 * render_paintFromLineTo()
 */
extern void render_paintFromLineTo(WINFO* wp, long min, long max);

/*--------------------------------------------------------------------------
 * render_repaintAllForFile()
 */
extern void render_repaintAllForFile(FTABLE* fp);

/*--------------------------------------------------------------------------
 * render_redrawAndPaintCurrentFile()
 */
extern void render_redrawAndPaintCurrentFile(void);

/*--------------------------------------------------------------------------
 * render_linePart()
 */
extern void render_linePart(FTABLE* fp, long ln, int col1, int col2);

/*------------------------------------------------------------
 * render_updateCaret()
 * Update the current caret for the passed editor window (dependening on insert mode
 * etc...)
 */
extern void render_updateCaret(WINFO* wp);

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

/*------------------------------------------------------------
 * wt_curpos()
 */
extern void wt_curpos(WINFO* wp, long ln, long col);

/*------------------------------------------------------------
 * wt_deleteline()
 */
extern void wt_deleteline(WINFO* wp, int additional, int nlines);

/*------------------------------------------------------------
 * wt_insline()
 */
extern void wt_insline(WINFO* wp, int nlines);

/*--------------------------------------------------------------------------
 * render_repaintAllForFile()
 */
extern void render_repaintAllForFile(FTABLE* fp);

/*--------------------------------------------------------------------------
 * render_redrawCurrentLine()
 * Redraw the line containing the cursor, in the "current active" editor window.
 */
extern void render_redrawCurrentLine(void);

/**
 * Returns the view num steps from the step - 0 to return the current to level view, 1 to return
 * the one below the top window.
 */
extern WINFO* ww_stackwi(int num);

/*------------------------------------------------------------
 * font_createFontWithStyle()
 * create a logical font. If the style is present it is taken
 * from the second parameter, if not, it is taken from the font.
 */
extern HFONT font_createFontWithStyle(EDFONT* pFont, EDFONTSTYLE* pStyle);

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

int ww_blkcolomn(WINFO *wp);

#define	_WINFO_H
#endif	_WINFO_H

