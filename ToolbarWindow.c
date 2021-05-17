/*
 * ToolbarWindow.c
 *
 * PROJEKT: PKS-EDIT for Windows 95
 *
 * purpose: toolbars
 *
 * 										created      : 
 * 										last modified:
 *										author	   : TOM
 *
 * (c) Pahlen & Krauss
 *
 * 								Author: TOM
 */

#include <windows.h>
#include <commctrl.h>
#include "winterf.h"
#include "pksedit.h"
#include "pksrc.h"
#include "edierror.h"
#include "editorconfiguration.h"

HWND	hwndToolbar;

/*------------------------------------------------------------
 * Returns the current size of the toolbar.
 */
void tb_wh(WORD *width, WORD *height)
{
	RECT		rect;

	if (hwndToolbar == 0 || (GetConfiguration()->layoutoptions & OL_TOOLBAR) == 0) {
		*width = 0;
		*height = 0;
		return;
	}
	GetWindowRect(hwndToolbar, &rect);
	*width = (WORD)(rect.right - rect.left) + 1;
	*height = (WORD)(rect.bottom - rect.top) + 1;
}

/*--------------------------------------------------------------------------
 * tb_init()
 */
void tb_init(HWND hwndDaddy)
{
	TBADDBITMAP	tbabmp;
	TBBUTTON	tbb[20];
	int			iIndex;
	int			nButton;

	if (hwndToolbar) {
		return;
	}

	InitCommonControls();
    hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, (LPSTR) NULL, 
		WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_ADJUSTABLE,
        0, 0, 0, 0, hwndDaddy, (HMENU) IDM_TOOLBAR, hInst, NULL);
	if (!hwndToolbar) {
		return;
	}
    SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, 
        (WPARAM) sizeof(TBBUTTON), 0);

	tbabmp.hInst = HINST_COMMCTRL;
	tbabmp.nID = IDB_STD_SMALL_COLOR;
	iIndex = SendMessage(hwndToolbar, TB_ADDBITMAP, 15, (LPARAM)&tbabmp);

	nButton = 0;
    tbb[nButton].iBitmap = iIndex + STD_FILEOPEN;
    tbb[nButton].idCommand = MOPENF;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_FILESAVE;
    tbb[nButton].idCommand = MSAVERES;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_CUT;
    tbb[nButton].idCommand = MTDEL;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_COPY;
    tbb[nButton].idCommand = MTCUT;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_PASTE;
    tbb[nButton].idCommand = MTPASTE;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_FIND;
    tbb[nButton].idCommand = MFIND;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_REPLACE;
    tbb[nButton].idCommand = MREPLACE;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_UNDO;
    tbb[nButton].idCommand = IDM_UNDO;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_REDOW;
    tbb[nButton].idCommand = IDM_DOAGAIN;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = 0;
    tbb[nButton].idCommand = 0;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_SEP;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_PRINT;
    tbb[nButton].idCommand = IDM_PRINTTEXT;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    tbb[nButton].iBitmap = iIndex + STD_HELP;
    tbb[nButton].idCommand = IDM_HLPINDEX;
    tbb[nButton].fsState = TBSTATE_ENABLED;
    tbb[nButton].fsStyle = TBSTYLE_BUTTON;
    tbb[nButton].dwData = 0;
    tbb[nButton].iString = 0;

	nButton++;
    SendMessage(hwndToolbar, TB_ADDBUTTONS, (WPARAM) nButton, (LPARAM) (LPTBBUTTON) &tbb);
    ShowWindow(hwndToolbar, SW_SHOW);
}

