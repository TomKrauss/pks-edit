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

typedef struct tagBG_TASK {
	void (*bg_taskFunction)(struct tagBG_TASK* pParam);
} BG_TASK;

/* PKS Edit private messages */
#define	WM_EDWINREORG				WM_USER + 101	/* reorganize edit window */
#define	WM_PKSKEY					WM_USER + 102	/* private key binding */
#define	WM_PKSOPTOGGLE				WM_USER + 104	/* option toggle pressed */
#define	WM_ICONDROP					WM_USER + 105	/* something dropped on icon */
#define	WM_LANGUAGE_CHANGED			WM_USER + 106	/* sent to the main window, if the language has changed */
#define	WM_BACKGROUND_TASK_FINISHED	WM_USER + 109	/* a task from has finished. LPARAM is a pointer to a BG_TASK structure */

/* PKS global private message */
#define	WM_PKSBROADCAST		_wm_pksbroadcast	/* talk with brothers */
#define	WMBRD_BYEBYE		0			/* ask for hwndMain */
#define	WMBRD_HELLO			1			/* ask for hwndMain */
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

// The ID and default waiting time of our main timer.
#define 	TIM_FRAME			23
#define		TIMER_INTERVALL		1000

extern char   szAppName[];

typedef long (FPE_FUNC *WINFUNCP)(HWND, UINT, WPARAM, LPARAM);
typedef BOOL (FPE_FUNC *ENUMFUNCP)(HWND, DWORD);

extern LRESULT	FrameWndProc(HWND, UINT, WPARAM, LPARAM);
extern WINFUNC 	EditWndProc(HWND, UINT, WPARAM, LPARAM);
extern WINFUNC 	render_defaultWindowProc(HWND, UINT, WPARAM, LPARAM);
extern WINFUNC 	RulerWndProc(HWND, UINT, WPARAM, LPARAM);
extern int 		win_registerWindowClass( char *szClassName, WNDPROC WinProc, 
					LPSTR lpCursorName, HANDLE hbrBackground, 
					LPSTR szIcon, int nExtra);

extern HWND   	hwndAbort,hwndFkeys, hwndMain,hwndRebar,
				hwndDlg,hwndFkeys,hwndStatus;

extern HINSTANCE hInst;
/*
 * Return the module instance handle from which the language resources are loaded.
 */
extern HINSTANCE ui_getResourceModule();

/*
 * Switch to the given language. Language is specified for example as "en-US" or "de-DE".
 */
extern void ui_switchToLanguage(char* pszLanguage);

extern int		_startupComplete;
extern UINT		WM_PKSBROADCAST;

#define WINTERF_H

#endif
