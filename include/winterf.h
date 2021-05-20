//
//
// WINTERF.H
//
// window interfaces: exported classes, handles, ... 
//
//

#ifndef WINTERF_H

#include "editorfont.h"

#define FPE_FUNC	CALLBACK
#define WINFUNC    	LRESULT FPE_FUNC
#define ENUMFUNC   	BOOL FPE_FUNC
#define DLGFUNC   	BOOL FPE_FUNC

/* PKS-EDIT private messages */
#define	WM_EDWINREORG		WM_USER + 101	/* reorganize edit window */
#define	WM_PKSKEY			WM_USER + 102	/* private key binding */
#define	WM_PKSKEYDOWN		WM_USER + 103	/* private key down */
#define	WM_PKSOPTOGGLE		WM_USER + 104	/* option toggle pressed */
#define	WM_ICONDROP			WM_USER + 105	/* something dropped on icon */
#define	WM_ICONSELECT		WM_USER + 106	/* select icon picture */
#define	WM_PROFSAVE			WM_USER + 107	/* save profile item data */
#define	WM_ICONCLASSVALUE	WM_USER + 108	/* query icon class for desktop icons */
#define	WM_TASKFINISHED		WM_USER + 109	/* spawned task has finished */

/* PKS global private message */
#define	WM_PKSBROADCAST		_wm_pksbroadcast	/* talk with brothers */
#define	WMBRD_BYEBYE		0			/* ask for hwndFrame */
#define	WMBRD_HELLO			1			/* ask for hwndFrame */
#define	WMBRD_SELECTFILE	2			/* top an editing window */
#define	WMBRD_FINDFILE		3			/* editing file(lParam) ? */

#define	GWL_ICPARAMS		0
#define	GWL_ICCLASSVALUES	GWL_ICPARAMS+sizeof(void*)

/* charsel custom messages and notifications */
#define	WM_CHARCHANGE		WM_USER + 1	/* new character activated */
#define	CS_QUERYCHAR		WM_USER + 2	/* query selected character */
#define	CSN_CHARSELECT		1			/* character selected */

/* color selector custum */
#define	CO_GETCOLOR		WM_USER + 1	/* set color value */
#define	COL_RGB			0			/* GETCOLOR subcodes */
#define	COL_R			1
#define	COL_G			2
#define	COL_B			3
#define	CO_SETCOLOR		WM_USER + 2	/* set color value */

/* keycode custom */
#define 	KC_QUERYKEY		WM_USER + 101	/* Keycode query message */

#define 	TIM_FRAME			23

extern char   szAppName[];
extern char   szFrameClass[];
extern char   szEditClass[];
extern char   szStatusClass[];
extern char   szRulerClass[];
extern char   szWorkAreaClass[];

typedef long (FPE_FUNC *WINFUNCP)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (FPE_FUNC *ENUMFUNCP)(HWND, DWORD);

extern LRESULT	FrameWndProc(HWND, UINT, WPARAM, LPARAM);
extern WINFUNC 	IconWndProc(HWND, UINT, WPARAM, LPARAM);
extern WINFUNC 	EditWndProc(HWND, UINT, WPARAM, LPARAM);
extern WINFUNC 	WorkAreaWndProc(HWND, UINT, WPARAM, LPARAM);
extern WINFUNC 	RulerWndProc(HWND, UINT, WPARAM, LPARAM);
extern int 		EdMkWinClass( char *szClassName, WNDPROC WinProc, 
					LPSTR lpCursorName, HANDLE hbrBackground, 
					LPSTR szIcon, int nExtra);

extern HWND   	hwndAbort,hwndFkeys,hwndFrame,hwndClient,hwndToolbar,
				hwndDlg,hwndFkeys,hwndStatus;

/*------------------------------------------------------------
 * EdCreateFont()
 * create a logical font
 */
extern HFONT EdCreateFont(EDFONT* pFont);

extern HINSTANCE hInst;
extern UINT		WM_PKSBROADCAST;

#define WINTERF_H

#endif
