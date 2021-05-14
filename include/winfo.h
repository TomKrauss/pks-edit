/*------------------------*/
/* WINDOW - Flags         */
/*------------------------*/

# ifndef 	_WINFO_H

#include "editorfont.h"

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
     HWND      edwin_handle,ww_handle,ru_handle,st_handle;
     
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
	int		fnt_handle;     
	
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
 * SendRedraw()
 */
extern void SendRedraw(HWND hwnd);

/*--------------------------------------------------------------------------
 * EdRedrawWindow()
 */
extern void EdRedrawWindow(WINFO* wp);

#define FTPOI(wp)		(FTABLE*)(wp->fp)

int ww_blkcolomn(WINFO *wp);

#define	_WINFO_H
#endif	_WINFO_H

